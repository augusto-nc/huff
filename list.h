#ifndef TREE_FILE_H_INCLUDED
#define TREE_FILE_H_INCLUDED
#include <stdio.h>

typedef struct Item Item;
typedef struct Item Iterate;
typedef struct List List;


typedef struct List{
    Item* head;
    Item* last;
    int size_;
} List;

List *createList();
void* getVal(Iterate* it);
Iterate* iterate(List* l);
int hasNext(Iterate** it);
void add(List* c,void* val);

typedef struct Item{
    Item* next;
    void* val;
} Item;



#endif // TREE_FILE_H_INCLUDED
