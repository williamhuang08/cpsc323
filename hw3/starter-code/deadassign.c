/*
***********************************************************************
  DEADASSIGN.C : IMPLEMENT THE DEAD CODE ELIMINATION OPTIMIZATION HERE
************************************************************************
*/

#include "deadassign.h"

int change = 0;
refVar *last, *head;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE HEAD AND LAST POINTERS FOR THE REFERENCED 
  VARIABLE LIST.
************************************************************************
*/

void init()
{ 
    head = NULL;
    last = head;
}

/*
***********************************************************************
  FUNCTION TO FREE THE REFERENCED VARIABLE LIST
************************************************************************
*/

void FreeList()
{
   refVar* tmp;
   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }

}

void FreeVarsList()
{
   refVar* tmp;
   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp->name);
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO IDENTIFY IF A VARIABLE'S REFERENCE IS ALREADY TRACKED
************************************************************************
*/
bool VarExists(char* name) {
   refVar *node;
   node = head;
   while(node != NULL) {
       if(!strcmp(name, node->name)) {
           return true;
       }
       node = node->next;
    }
    return false;
}

/*
***********************************************************************
  FUNCTION TO ADD A REFERENCE TO THE REFERENCE LIST
************************************************************************
*/
void UpdateRefVarList(char* name) {
    refVar* node = malloc(sizeof(refVar));
    if (node == NULL) return;
    node->name = name;
    node->next = NULL;
    if(head == NULL) {
        last = node;
        head = node;
    }
    else {
        last->next = node;
        last = node;
    }
}

/*
****************************************************************************
  FUNCTION TO PRINT OUT THE LIST TO SEE ALL VARIABLES THAT ARE USED/REFERRED
  AFTER THEIR ASSIGNMENT. YOU CAN USE THIS FOR DEBUGGING PURPOSES OR TO CHECK
  IF YOUR LIST IS GETTING UPDATED CORRECTLY
******************************************************************************
*/
void PrintRefVarList() {
    refVar *node;
    node = head;
    if(node==NULL) {
        printf("\nList is empty"); 
        return;
    }
    while(node != NULL) {
        printf("\t %s", node->name);
        node = node->next;
    }
}

/*
***********************************************************************
  FUNCTION TO UPDATE THE REFERENCE LIST WHEN A VARIABLE IS REFERENCED 
  IF NOT DONE SO ALREADY.
************************************************************************
*/
void UpdateRef(Node* node) {
      if(node->right != NULL && node->right->exprCode == VARIABLE) {
          if(!VarExists(node->right->name)) {
              UpdateRefVarList(node->right->name);
          }
      }
      if(node->left != NULL && node->left->exprCode == VARIABLE) {
          if(!VarExists(node->left->name)) {
              UpdateRefVarList(node->left->name);
          }
      }
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/


/*
********************************************************************
  THIS FUNCTION IS MEANT TO TRACK THE REFERENCES OF EACH VARIABLE
  TO HELP DETERMINE IF IT WAS USED OR NOT LATER
********************************************************************
*/
void recurse(NodeList* statements, char* name) {
  NodeList *begin = statements;
  while (statements != NULL) {
    if (statements->node->stmtCode == ASSIGN) {
      if (statements->node->name != NULL && strcmp(statements->node->name, name) == 0) {
        if (statements->node->right->exprCode == OPERATION && statements->node->right->left != NULL && statements->node->right->left->exprCode == VARIABLE) {
            if (VarExists(statements->node->right->left->name)) {
              return;
            }
          UpdateRefVarList(strdup(statements->node->right->left->name));
          // UpdateRef(statements->node->right);
          recurse(begin, statements->node->right->left->name);
        }
        if (statements->node->right->exprCode == OPERATION && statements->node->right->right != NULL && statements->node->right->right->exprCode == VARIABLE) {
            if (VarExists(statements->node->right->right->name)) {
              return;
            }
            // UpdateRef(statements->node->left);
          UpdateRefVarList(strdup(statements->node->right->right->name));
          recurse(begin, statements->node->right->right->name);
        }
        // printf("ayyy");
        if (statements->node->right->opCode == FUNCTIONCALL) {
          // printf("HEREEEEE");
          NodeList* curr = statements->node->right->arguments;
          while (curr != NULL) {
            if (curr->node->exprCode == VARIABLE) {
              UpdateRefVarList(strdup(curr->node->name));
              recurse(begin, curr->node->name);
            }
            curr = curr->next;
          }
        }
      }
    }
    statements = statements->next;
  }
}

void TrackRef(Node* funcNode) {
        char* name;
        NodeList* statements = funcNode->statements;
        NodeList* begin = statements;
        while (statements->node->stmtCode != RETURN) {
          statements = statements->next;
        }

        if (statements->node->left->name != NULL) {
            name = statements->node->left->name;
            UpdateRefVarList(strdup(name));
            recurse(begin, name);
        }
}


/*
***************************************************************
  THIS FUNCTION IS MEANT TO DO THE ACTUAL DEADCODE REMOVAL
  BASED ON THE INFORMATION OF TRACKED REFERENCES
****************************************************************
*/
NodeList* RemoveDead(NodeList* statements) {
    refVar* varNode;
    Node* tmp;
    NodeList *prev, *first;

    prev = AllocateNodeList();
    prev->node = NULL;
    prev->next = NULL;
    first = prev;

    while (statements != NULL) {
      // printf("NAME === %s ", statements->node->name);
      while (statements != NULL && statements->node->stmtCode == ASSIGN && !VarExists(statements->node->name)) {
        // printf("dead condition");
        FreeAssignment(statements->node);
        change = 1;
        NodeList* willfree = statements;
        statements = statements->next;
        free(willfree); 
      }
      prev->next = statements;
      statements = statements->next;
      prev = prev->next;
    }
    NodeList * dummy = first;
    first = first->next;
    free(dummy);
    return first;
}




/*
********************************************************************
  THIS FUNCTION SHOULD ENSURE THAT THE DEAD CODE REMOVAL PROCESS
  OCCURS CORRECTLY FOR ALL THE FUNCTIONS IN THE PROGRAM
********************************************************************
*/
bool DeadAssign(NodeList* worklist) {
   bool madeChange = false;
   change = 0;
   while(worklist != NULL) {
            /*
             ****************************************
              TODO : YOUR CODE HERE
             ****************************************
             */
        if (worklist->node->type == FUNCTIONDECL) {
            NodeList* statements = worklist->node->statements;
            TrackRef(worklist->node);
            // PrintRefVarList();
            NodeList* nodestatements = RemoveDead(worklist->node->statements);
            worklist->node->statements = nodestatements;
            FreeVarsList();
        }
        // PrintStatements(worklist->node->statements);
        worklist = worklist->next;
    }
    if(change==1) madeChange=true;
    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
 
