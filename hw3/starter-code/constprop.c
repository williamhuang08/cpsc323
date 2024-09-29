/*
********************************************************************************
  CONSTPROP.C : IMPLEMENT THE DOWNSTREAM CONSTANT PROPOGATION OPTIMIZATION HERE
*********************************************************************************
*/

#include "constprop.h"

refConst *lastNode, *headNode;
/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO FREE THE CONSTANTS-ASSOCIATED VARIABLES LIST
************************************************************************
*/
void FreeConstList()
{
   refConst* tmp;
   while (headNode != NULL)
    {
       tmp = headNode;
       headNode = headNode->next;
       free(tmp);
    }

}

/*
*************************************************************************
  FUNCTION TO ADD A CONSTANT VALUE AND THE ASSOCIATED VARIABLE TO THE LIST
**************************************************************************
*/
void UpdateConstList(char* name, long val) {
    refConst* node = malloc(sizeof(refConst));
    if (node == NULL) return;
    node->name = name;
    node->val = val;
    node->next = NULL;
    if(headNode == NULL) {
        lastNode = node;
        headNode = node;
    }
    else {
        lastNode->next = node;
        lastNode = node;
    }
}

/*
*****************************************************************************
  FUNCTION TO LOOKUP IF A CONSTANT ASSOCIATED VARIABLE IS ALREADY IN THE LIST
******************************************************************************
*/
refConst* LookupConstList(char* name) {
    refConst *node;
    node = headNode; 
    while(node!=NULL){
        if(!strcmp(name, node->name))
            return node;
        node = node->next;
    }
    return NULL;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/


/*
************************************************************************************
  THIS FUNCTION IS MEANT TO UPDATE THE CONSTANT LIST WITH THE ASSOCIATED VARIABLE
  AND CONSTANT VALUE WHEN ONE IS SEEN. IT SHOULD ALSO PROPOGATE THE CONSTANTS WHEN 
  WHEN APPLICABLE. YOU CAN ADD A NEW FUNCTION IF YOU WISH TO MODULARIZE BETTER.
*************************************************************************************
*/
void TrackConst(NodeList* statements) {
       Node* node;
       while(statements != NULL) {
        node = statements->node;
            /*
             ****************************************
                     TODO : YOUR CODE HERE
             ****************************************
            */
        if (node->type == STATEMENT && node->stmtCode == ASSIGN) {
          Node* stmtNodeRight = node->right;

          if (stmtNodeRight->exprCode == CONSTANT) {
            if (!LookupConstList(node->name)) {
              UpdateConstList(node->name, stmtNodeRight->value);
            }
          }

          if (stmtNodeRight->exprCode == OPERATION && stmtNodeRight->left != NULL && stmtNodeRight->left->exprCode == VARIABLE) {
            refConst* lfound = LookupConstList(stmtNodeRight->left->name);
            if (lfound != NULL) {
              stmtNodeRight->left->exprCode = CONSTANT;
              stmtNodeRight->left->value = lfound->val;
              madeChange = true;
            }
          }
          if (stmtNodeRight->exprCode == OPERATION && stmtNodeRight->right != NULL && stmtNodeRight->right->exprCode == VARIABLE) {
            refConst* rfound = LookupConstList(stmtNodeRight->right->name);
            if (rfound != NULL) {
              stmtNodeRight->right->exprCode = CONSTANT;
              stmtNodeRight->right->value = rfound->val;
              madeChange = true;
            }
          }
        }
        
        statements = statements->next;
    }
}


bool ConstProp(NodeList* worklist) {
    while(worklist!=NULL){
            /*
             ****************************************
                     TODO : YOUR CODE HERE
             ****************************************
             */
        NodeList* statements = worklist->node->statements;
        TrackConst(statements);
        worklist = worklist->next;
    }
    FreeConstList(headNode);
    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
