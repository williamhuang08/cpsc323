/*
***********************************************************************
  CODEGEN.C : IMPLEMENT CODE GENERATION HERE
************************************************************************
*/
#include "codegen.h"

int argCounter = 0;
int lastUsedOffset = -8;
char lastOffsetUsed[100];
FILE *fptr;
regInfo *regList, *regHead, *regLast;
varStoreInfo *varList, *varHead, *varLast;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE THE ASSEMBLY FILE WITH FUNCTION DETAILS
************************************************************************
*/
void InitAsm(char* funcName) {
    fprintf(fptr, "\n.globl %s", funcName);
    fprintf(fptr, "\n%s:", funcName); 

    // Init stack and base ptr
    fprintf(fptr, "\npushq %%rbp");  
    fprintf(fptr, "\nmovq %%rsp, %%rbp"); 
}

/*
***************************************************************************
   FUNCTION TO WRITE THE RETURNING CODE OF A FUNCTION IN THE ASSEMBLY FILE
****************************************************************************
*/
void RetAsm() {
    fprintf(fptr,"\npopq  %%rbp");
    fprintf(fptr, "\nretq\n");
} 

/*
***************************************************************************
  FUNCTION TO CONVERT OFFSET FROM LONG TO CHAR STRING 
****************************************************************************
*/
void LongToCharOffset() {
     lastUsedOffset = lastUsedOffset - 8;
     snprintf(lastOffsetUsed, 100,"%d", lastUsedOffset);
     strcat(lastOffsetUsed,"(%rbp)");
}

/*
***************************************************************************
  FUNCTION TO CONVERT CONSTANT VALUE TO CHAR STRING
****************************************************************************
*/
void ProcessConstant(Node* opNode) {
     char value[10];
     LongToCharOffset();
     snprintf(value, 10,"%ld", opNode->value);
     char str[100];
     snprintf(str, 100,"%d", lastUsedOffset);
     strcat(str,"(%rbp)");
     AddVarInfo("", str, opNode->value, true);
     fprintf(fptr, "\nmovq  $%s, %s", value, str);
}

/*
***************************************************************************
  FUNCTION TO SAVE VALUE IN ACCUMULATOR (RAX)
****************************************************************************
*/
void SaveValInRax(char* name) {
    char *tempReg;
    tempReg = GetNextAvailReg(true);
    if(!(strcmp(tempReg, "NoReg"))) {
        LongToCharOffset();
        fprintf(fptr, "\n movq %%rax, %s", lastOffsetUsed);
        UpdateVarInfo(name, lastOffsetUsed, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
    else {
        fprintf(fptr, "\nmovq %%rax, %s", tempReg);
        UpdateRegInfo(tempReg, 0);
        UpdateVarInfo(name, tempReg, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
}



/*
***********************************************************************
  FUNCTION TO ADD VARIABLE INFORMATION TO THE VARIABLE INFO LIST
************************************************************************
*/
void AddVarInfo(char* varName, char* location, long val, bool isConst) {
   varStoreInfo* node = malloc(sizeof(varStoreInfo));
   node->varName = varName;
   node->value = val;
   strcpy(node->location,location);
   node->isConst = isConst;
   node->next = NULL;
   node->prev = varLast;
   if(varHead==NULL) {
       varHead = node;
       varLast = node;;
       varList = node;
   } else {
       //node->prev = varLast;
       varLast->next = node;
       varLast = varLast->next;
   }
   varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO FREE THE VARIABLE INFORMATION LIST
************************************************************************
*/
void FreeVarList()
{  
   varStoreInfo* tmp;
   while (varHead != NULL)
    {  
       tmp = varHead;
       varHead = varHead->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO LOOKUP VARIABLE INFORMATION FROM THE VARINFO LIST
************************************************************************
*/
char* LookUpVarInfo(char* name, long val) {
    varList = varLast;
    if(varList == NULL) printf("NULL varlist");
    while(varList!=NULL) {
        if(varList->isConst == true) {
            if(varList->value == val) return varList->location;
        }
        else {
            if(!strcmp(name,varList->varName)) return varList->location;
        }
        varList = varList->prev;
    }
    varList = varHead;
    return "";
}

/*
***********************************************************************
  FUNCTION TO UPDATE VARIABLE INFORMATION 
************************************************************************
*/
void UpdateVarInfo(char* varName, char* location, long val, bool isConst) {
  
   if(!(strcmp(LookUpVarInfo(varName, val), ""))) {
       AddVarInfo(varName, location, val, isConst);
   }
   else {
       varList = varHead;
       if(varList == NULL) printf("NULL varlist");
       while(varList!=NULL) {
           if(!strcmp(varList->varName,varName)) {
               varList->value = val;
               strcpy(varList->location,location);
               varList->isConst = isConst;
               break;
        }
        varList = varList->next;
       }
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE VARIABLE INFORMATION LIST
************************************************************************
*/
void PrintVarListInfo() {
    varList = varHead;
    if(varList == NULL) printf("NULL varlist");
    while(varList!=NULL) {
        if(!varList->isConst) {
            printf("\t %s : %s", varList->varName, varList->location);
        }
        else {
            printf("\t %ld : %s", varList->value, varList->location);
        }
        varList = varList->next;
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO ADD NEW REGISTER INFORMATION TO THE REGISTER INFO LIST
************************************************************************
*/
void AddRegInfo(char* name, int avail) {

   regInfo* node = malloc(sizeof(regInfo));
   node->regName = name;
   node->avail = avail;
   node->next = NULL; 

   if(regHead==NULL) {
       regHead = node;
       regList = node;
       regLast = node;
   } else {
       regLast->next = node;
       regLast = node;
   }
   regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO FREE REGISTER INFORMATION LIST
************************************************************************
*/
void FreeRegList()
{  
   regInfo* tmp;
   while (regHead != NULL)
    {  
       tmp = regHead;
       regHead = regHead->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO UPDATE THE AVAILIBILITY OF REGISTERS IN THE REG INFO LIST
************************************************************************
*/
void UpdateRegInfo(char* regName, int avail) {
    while(regList!=NULL) {
        if(regName == regList->regName) {
            regList->avail = avail;
        }
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO RETURN THE NEXT AVAILABLE REGISTER
************************************************************************
*/
char* GetNextAvailReg(bool noAcc) {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        if(regList->avail == 1) {
            if(!noAcc) return regList->regName;
            // if not rax and dont return accumulator set to true, return the other reg
            // if rax and noAcc == true, skip to next avail
            if(noAcc && strcmp(regList->regName, "%rax")) { 
                return regList->regName;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return "NoReg";
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF ANY REGISTER APART FROM OR INCLUDING 
  THE ACCUMULATOR(RAX) IS AVAILABLE
************************************************************************
*/
int IfAvailReg(bool noAcc) {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        if(regList->avail == 1) {
            // registers available
            if(!noAcc) return 1;
            if(noAcc && strcmp(regList->regName, "%rax")) {
                return 1;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return 0;
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF A SPECIFIC REGISTER IS AVAILABLE
************************************************************************
*/
bool IsAvailReg(char* name) {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        if(!strcmp(regList->regName, name)) {
           if(regList->avail == 1) {
               return true;
           } 
        }
        regList = regList->next;
    }
    regList = regHead;
    return false;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE REGISTER INFORMATION
************************************************************************
*/
void PrintRegListInfo() {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        printf("\t %s : %d", regList->regName, regList->avail);
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO CREATE THE REGISTER LIST
************************************************************************
*/
void CreateRegList() {
    // Create the initial reglist which can be used to store variables.
    // 4 general purpose registers : AX, BX, CX, DX
    // 4 special purpose : SP, BP, SI , DI. 
    // Other registers: r8, r9
    // You need to decide which registers you will add in the register list 
    // use. Can you use all of the above registers?
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ***************************************
    */
}



/*
***********************************************************************
  THIS FUNCTION IS MEANT TO PUT THE FUNCTION ARGUMENTS ON STACK
************************************************************************
*/
int PutArgumentsFromStack(NodeList* arguments) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */
    while(arguments!=NULL) {
    /*
     ***********************************************************************
              TODO : YOUR CODE HERE
      THINK ABOUT WHERE EACH ARGUMENT COMES FROM. EXAMPLE WHERE IS THE 
      FIRST ARGUMENT OF A FUNCTION STORED.
     ************************************************************************
     */ 
        arguments = arguments->next;
    }
    return argCounter;
}


/*
*************************************************************************
  THIS FUNCTION IS MEANT TO GET THE FUNCTION ARGUMENTS FROM THE  STACK
**************************************************************************
*/
void PutArgumentsOnStack(NodeList* arguments) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */
    while(arguments!=NULL) {
    /*
     ***********************************************************************
              TODO : YOUR CODE HERE
      THINK ABOUT WHERE EACH ARGUMENT COMES FROM. EXAMPLE WHERE IS THE
      FIRST ARGUMENT OF A FUNCTION STORED AND WHERE SHOULD IT BE EXTRACTED
      FROM AND STORED TO..
     ************************************************************************
     */
        arguments = arguments->next;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO PROCESS EACH CODE STATEMENT AND GENERATE 
  ASSEMBLY FOR IT. 
  TIP: YOU CAN MODULARIZE BETTER AND ADD NEW SMALLER FUNCTIONS IF YOU 
  WANT THAT CAN BE CALLED FROM HERE.
 ************************************************************************
 */  
void ProcessStatements(NodeList* statements) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */
    // num_statements = 0;    
    while(statements != NULL) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */         
        if (statements->node->stmtCode == ASSIGN) {
            Node* stmtRight = statements->node->right;
            if (stmtRight->exprCode == OPERATION) {
                ProcessOperation(stmtRight, statements->node->name, statements->node->value);
            }
        }
        else {
            if (statements->node->left->exprCode == VARIABLE) {
                char* retloc = LookUpVarInfo(statements->node->left->name, statements->node->left->value);
                fprintf(fptr, "\nmovq %s, %%rax", retloc);
            } 
            else {
                fprintf(fptr, "\nmovq %ld, %%rax", statements->node->left->value);
            }
        }
        // num_statements++; 
        statements = statements->next;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO DO CODEGEN FOR ALL THE FUNCTIONS IN THE FILE
 ************************************************************************
*/
void Codegen(NodeList* worklist) {
    fptr = fopen("assembly.s", "w+");
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */
    if(fptr == NULL) {
        printf("\n Could not create assembly file");
        return; 
    }
    while(worklist != NULL) {
      /*
       ****************************************
              TODO : YOUR CODE HERE
       ****************************************
      */
        if (worklist->node->type == FUNCTIONDECL) {
            lastUsedOffset = -8;
            argCounter = 0;
            InitAsm(worklist->node->name);
            Push(worklist->node->arguments);
            int space = CreateStackSpace(worklist->node->statements, worklist->node->arguments);
            AddStatements(worklist->node->statements);
            ProcessStatements(worklist->node->statements);
            RestoreStackSpace(space);
            HandleArgs(worklist->node->arguments);
            RetAsm();
        }
        worklist = worklist->next; 
    }
    FreeVarList();
    fclose(fptr);
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS BELOW THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/
int CreateStackSpace(NodeList* statements, NodeList* arguments) {
    int numstatements = 0;
    while (statements != NULL && statements->node->stmtCode != RETURN) {
        numstatements++;
        statements = statements->next;
    }

    int space = (numstatements) * 8;
    fprintf(fptr, "\nsubq $%d, %%rsp", space);
    return space;
}

int RestoreStackSpace(int space) {
    fprintf(fptr, "\naddq $%d, %%rsp", space);
    return space;
}

int Push(NodeList *arguments) {
    while(arguments!=NULL) {
        if (argCounter == 0) {
            fprintf(fptr, "\npushq %%rdi");
        } else if (argCounter == 1){
            fprintf(fptr, "\npushq %%rsi");
        }  else if (argCounter == 2){
            fprintf(fptr, "\npushq %%rdx");
        } else if (argCounter == 3){
            fprintf(fptr, "\npushq %%rcx");
        } else if (argCounter == 4){
            fprintf(fptr, "\npushq %%r8");
        } else{
            fprintf(fptr, "\npushq %%r9");
        }
        
        bool isConst = false;
        if (arguments->node->exprCode == CONSTANT) {
            isConst = true;
        }

        char location[20];
        sprintf(location, "%d(%%rbp)", lastUsedOffset);

        AddVarInfo(arguments->node->name, location, arguments->node->value, isConst);
        arguments = arguments->next;
        argCounter++;
        lastUsedOffset -= 8;
    }
    return argCounter;
}

int AddStatements(NodeList *statements) {
    int sCounter = 0;
    while(statements!=NULL) {
        if (statements->node->stmtCode != RETURN && strcmp(LookUpVarInfo(statements->node->name, statements->node->value), "") == 0) {
            bool isConst = false;
            if (statements->node->exprCode == CONSTANT) {
                isConst = true;
            }
            char location[100];
            sprintf(location, "%d(%%rbp)", lastUsedOffset);
            AddVarInfo(statements->node->name, location, statements->node->value, isConst);
        }
        statements = statements->next;
        lastUsedOffset -= 8;
    }
    return sCounter;
}

void ProcessOperation(Node* op, char* name, long value) {
    Node* right = op->right;
    Node* left = op->left;
    char* loc;
    char* rloc;
    char* final;


    if (right != NULL) {
        if (left->exprCode == VARIABLE) {
            loc = LookUpVarInfo(left->name, left->value);
            fprintf(fptr, "\nmovq %s, %%rax", loc);     
        } else {
            int val = left->value;
            fprintf(fptr, "\nmovq $%d, %%rax", val);     
        }       
        if (op->opCode == MULTIPLY) {
            if (right->exprCode == CONSTANT) {
                fprintf(fptr, "\nimulq $%ld, %%rax", right->value);
            } else {
                rloc = LookUpVarInfo(right->name, right->value);
                fprintf(fptr, "\nimulq %s, %%rax", rloc);
            }
            final = LookUpVarInfo(name, value);
            fprintf(fptr, "\nmovq %%rax, %s", final);
        } else if (op->opCode == DIVIDE) {
            if (right->exprCode == CONSTANT) {
                fprintf(fptr, "\nmovq $%ld, %%rcx", right->value);
                fprintf(fptr, "\ncqto");
                fprintf(fptr, "\nidivq %%rcx");
            } else {
                rloc = LookUpVarInfo(right->name, right->value);
                fprintf(fptr, "\nmovq %s, %%rcx", rloc);
                fprintf(fptr, "\ncqto");
                fprintf(fptr, "\nidivq %%rcx");
            }
            final = LookUpVarInfo(name, value);
            fprintf(fptr, "\nmovq %%rax, %s", final);
        } else if (op->opCode == ADD) {
            if (right->exprCode == CONSTANT) {
                fprintf(fptr, "\naddq $%ld, %%rax", right->value);
            } else {
                rloc = LookUpVarInfo(right->name, right->value);
                fprintf(fptr, "\naddq %s, %%rax", rloc);
            }
            final = LookUpVarInfo(name, value);
            fprintf(fptr, "\nmovq %%rax, %s", final);
        } else if (op->opCode == SUBTRACT) {
            if (right->exprCode == CONSTANT) {
                fprintf(fptr, "\nsubq $%ld, %%rax", right->value);
            } else {
                rloc = LookUpVarInfo(right->name, right->value);
                fprintf(fptr, "\nsubq %s, %%rax", rloc);
            }
            final = LookUpVarInfo(name, value);
            fprintf(fptr, "\nmovq %%rax, %s", final);
        } else if (op->opCode == BOR) {
            if (right->exprCode == CONSTANT) {
                fprintf(fptr, "\norq $%ld, %%rax", right->value);
            } else {
                rloc = LookUpVarInfo(right->name, right->value);
                fprintf(fptr, "\norq %s, %%rax", rloc);
            }
            final = LookUpVarInfo(name, value);
            fprintf(fptr, "\nmovq %%rax, %s", final);
        } else if (op->opCode == BAND) {
            if (right->exprCode == CONSTANT) {
                fprintf(fptr, "\nandq $%ld, %%rax", right->value);
            } else {
                rloc = LookUpVarInfo(right->name, right->value);
                fprintf(fptr, "\nandq %s, %%rax", rloc);
            }
            final = LookUpVarInfo(name, value);
            fprintf(fptr, "\nmovq %%rax, %s", final);
        } else if (op->opCode == BXOR) {
            if (right->exprCode == CONSTANT) {
                fprintf(fptr, "\nxorq $%ld, %%rax", right->value);
            } else {
                rloc = LookUpVarInfo(right->name, right->value);
                fprintf(fptr, "\nxorq %s, %%rax", rloc);
            }
            final = LookUpVarInfo(name, value);
            fprintf(fptr, "\nmovq %%rax, %s", final);
        } else if (op->opCode == BSHR) {
            if (right->exprCode == CONSTANT) {
                fprintf(fptr, "\nsarq $%ld, %%rax", right->value);
            } else {
                rloc = LookUpVarInfo(right->name, right->value);
                fprintf(fptr, "\nsarq %s, %%rax", rloc);
            }
            final = LookUpVarInfo(name, value);
            fprintf(fptr, "\nmovq %%rax, %s", final);
        } else {
            if (right->exprCode == CONSTANT) {
                fprintf(fptr, "\nsalq $%ld, %%rax", right->value);
            } else {
                rloc = LookUpVarInfo(right->name, right->value);
                fprintf(fptr, "\nsalq %s, %%rax", rloc);
            }
            final = LookUpVarInfo(name, value);
            fprintf(fptr, "\nmovq %%rax, %s", final);
        }   
    }
    else {
        if (op->opCode == FUNCTIONCALL) {
            SaveArgs(op->arguments);
            CallFunction(op);
            final = LookUpVarInfo(name, value);
            fprintf(fptr, "\nmovq %%rax, %s", final);
        }
        else {
            char* before = LookUpVarInfo(op->left->name, op->left->value);
            fprintf(fptr, "\nmovq %s, %%rax", before);
            fprintf(fptr, "\nnegq %%rax");
            final = LookUpVarInfo(name, value);
            fprintf(fptr, "\nmovq %%rax, %s", final);
        }
    }
}

void HandleArgs(NodeList *arguments) {
    int count = argCounter;
    while(count >= 0) {
        if (count == 6) {
            fprintf(fptr, "\npopq %%r9");
        } else if (count == 5){
            fprintf(fptr, "\npopq %%r8");
        }  else if (count == 4){
            fprintf(fptr, "\npopq %%rcx");
        } else if (count == 3){
            fprintf(fptr, "\npopq %%rdx");
        } else if (count == 2){
            fprintf(fptr, "\npopq %%rsi");
        } else if (count == 1) {
            fprintf(fptr, "\npopq %%rdi");
        }
        count--;
    }
}

void CallFunction(Node* function) {
    fprintf(fptr, "\ncall %s", function->left->name);
}

void SaveArgs(NodeList* arguments) {
    int temp = 0;
    while(arguments!=NULL) {
        long val;
        char* final;
        if (arguments->node->exprCode == CONSTANT) {
            val = arguments->node->value;
        }
        else {
            final = LookUpVarInfo(arguments->node->name, arguments->node->value);
        }
        if (temp == 0) {
            if (arguments->node->exprCode == CONSTANT) {
                fprintf(fptr, "\nmovq $%ld, %%rdi", val);
            }
            else {
                fprintf(fptr, "\nmovq %s, %%rdi", final);
            }
        } else if (temp == 1){
            if (arguments->node->exprCode == CONSTANT) {
                fprintf(fptr, "\nmovq $%ld, %%rsi", val);
            }
            else {
                fprintf(fptr, "\nmovq %s, %%rsi", final);
            }
        }  else if (temp == 2){
            if (arguments->node->exprCode == CONSTANT) {
                fprintf(fptr, "\nmovq $%ld, %%rdx", val);
            }
            else {
                fprintf(fptr, "\nmovq %s, %%rdx", final);
            }
        } else if (temp == 3){
            if (arguments->node->exprCode == CONSTANT) {
                fprintf(fptr, "\nmovq $%ld, %%rcx", val);
            }
            else {
                fprintf(fptr, "\nmovq %s, %%rcx", final);
            }
        } else if (temp == 4){
            if (arguments->node->exprCode == CONSTANT) {
                fprintf(fptr, "\nmovq $%ld, %%r8", val);
            }
            else {
                fprintf(fptr, "\nmovq %s, %%r8", final);
            }
        } else{
            if (arguments->node->exprCode == CONSTANT) {
                fprintf(fptr, "\nmovq $%ld, %%r9", val);
            }
            else {
                fprintf(fptr, "\nmovq %s, %%r9", final);
            }
        }
        arguments = arguments->next;
        temp++;
    }
}
/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/


