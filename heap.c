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
    int parent=parent_index(i);
    heap->array_[i]=valor;

    while(parent>=1 && heap->compare(heap->array_[parent],heap->array_[i])>0 ){     // Re-ordena Heap
    // Re-ordena Heap
        swap_(&(heap->array_[parent]),&(heap->array_[i]));
        i=parent;
        parent=parent_index(i);
    }


}


void* dequeue(Heap* heap){
    if(heap->size_==0){
        printf("Dequeue com Heap Vazia\n");
        exit(0);
    }


    void* max= heap->array_[1];
    heap->array_[1]=heap->array_[heap->size_];
    heap->size_--;
    max_heapify(heap,1);

    return max;
}

void max_heapify(Heap* heap,int index){
    int max;
    int left_index=get_left_index(index);
    int right_index=get_right_index(index);
    if(left_index<=heap->size_ && heap->compare(heap->array_[index] ,heap->array_[left_index])>0){
        max=left_index;
    }else{
        max=index;
    }

     if(right_index<=heap->size_ && heap->compare(heap->array_[max] ,heap->array_[right_index])>0){
        max=right_index;
    }

    if(index!=max){
        swap_(&heap->array_[index],&heap->array_[max]);
        max_heapify(heap,max);
    }





}


int is_empty_(Heap* heap){
    return heap->size_==0;
}





