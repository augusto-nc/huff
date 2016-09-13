#include "heap.h"
#include <stdlib.h>
#include "math.h"
#include <stdio.h>

void swap_(void** a,void** b){
    void* aux= *a;
    *a=*b;
    *b=aux;
}


Heap* createHeap(int len,int (*compare)(void*,void*)){
    Heap* heap= (Heap*) malloc(sizeof(Heap));

    int real_len=1;
    int leaf=1;
    while(real_len<len){ //calcula tamanho valido para a Heap
        leaf*=2;
        real_len+=leaf;
    }
    heap->length=real_len;
    heap->size_=0;       //inicia a Heap com 0 elementos
    heap->array_=(void**)malloc(sizeof(void*)*(real_len+1)); //aloca vetor da Heap, considerando espaco vazio na posicao 0
    heap->compare=compare;  //define a funcao de comparacao
    for(int i=1;i<=real_len;i++){
        heap->array_[i]=NULL;   //incializa elementos com NULL.
    }
    return heap;
}


void enqueue(Heap* heap,void* valor){
    if(heap->size_==heap->length){
        printf("Sem espaco no array da Heap\n");
        exit(0);
    }
    heap->size_++;
    int i=heap->size_;
    int parent;
    heap->array_[i]=valor;

    while(i>1){     // Re-ordena Heap
        parent=parent_index(i);
        if(heap->compare(heap->array_[parent],heap->array_[i])>0){
            swap_(&(heap->array_[parent]),&(heap->array_[i]));
            i=parent;
        }else{
            break;
        }
    }


}


void* dequeue(Heap* heap){
    if(heap->size_==0){
        printf("Dequeue com Heap Vazia\n");
        exit(0);
    }


    void* max= heap->array_[1];
    heap->array_[1]=heap->array_[heap->size_];
    heap->array_[heap->size_]=NULL;
    int i=1;
    int left;
    int right;
    heap->size_--;
     left=get_left_index(i);
     right=get_right_index(i);
     while(1){     // Re-ordena Heap

        if(left>heap->length || (heap->array_[left]==NULL && heap->array_[right]==NULL)){
            break;
        }


        if(heap->compare(heap->array_[left],heap->array_[right])>0){
            swap_(&(heap->array_[i]),&(heap->array_[right]));
            //heap->array_[i]=heap->array_[right];
            i=right;
        }else{
            swap_(&(heap->array_[i]),&(heap->array_[left]));
           // heap->array_[i]=heap->array_[left];
            i=left;
        }
         left=get_left_index(i);
         right=get_right_index(i);
    }

    return max;
}


int is_empty_(Heap* heap){
    return heap->size_==0;
}





