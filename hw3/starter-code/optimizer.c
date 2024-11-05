#include "optimizer.h"


void Optimizer(NodeList *funcdecls) {
/*
*************************************
     TODO: YOUR CODE HERE
*************************************
*/
     madeChange = true;
     while (madeChange == true) {
          madeChange = ConstantFolding(funcdecls);
          madeChange = ConstProp(funcdecls);
          madeChange = DeadAssign(funcdecls);
     }
}
