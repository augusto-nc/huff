#ifndef THREE_H_INCLUDED
#define THREE_H_INCLUDED
#include "input.h"
#include "stdio.h"

typedef struct Node Node;

typedef struct Node{
Node* left;
Node* right;
byte valor;
long qtd; //quantida de vezes que byte "valor" aparece
} Node;


Node* createNode();

int length(Node* three);


#endif // THREE_H_INCLUDED
