#include "list.h"


List* createList(){
    List* l = (List*)malloc(sizeof(List));
    l->size_=0;
    l->head=NULL;
    l->last=NULL;
    return l;
}

void add(List* l,void* val){
    Item* item= (Item*)malloc(sizeof(Item));
    item->val=val;
    l->size_++;
    item->next=NULL;
    if(l->head==NULL){
        l->head=item;
        l->last=item;
    }else{
        l->last->next=item;
        l->last=item;
    }
}

Iterate* iterate(List* l){
    return l->head;
}

void* getVal(Iterate* it){
    if(it!=NULL)
        return it->val;
     return NULL;
}

int hasNext(Iterate** it){
    if((*it)->next!=NULL){
        (*it)=((*it)->next);
        return 1;
    }
    return 0;
}






