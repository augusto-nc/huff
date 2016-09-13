
#include "three.h"


Node* createNode(){
Node* n=(Node*) malloc(sizeof(Node));
n->left=NULL;
n->right=NULL;
n->qtd=0;
return n;
}

int length(Node* three){
    if(three==NULL){
        return 0;
    }
    int l= length(three->left);
    int r= length(three->right);
    return l>r ? l+1 :r+1;
}
