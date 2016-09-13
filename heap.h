#ifndef HEAP_H_INCLUDED
#define HEAP_H_INCLUDED
#define parent_index(a) a/2
#define get_left_index(a) a*2
#define get_right_index(a) a*2+1


typedef struct Heap{
    void** array_;
    int size_;  // tamanho corrente da Heap
    int length; // Tamanho maximo da Heap
    int (*compare)(void*,void*); //funcao que compara elementos da Heap

} Heap;


Heap* createHeap(int len,int (*compare)(void*,void*));
void enqueue(Heap* heap,void* valor);
void* dequeue(Heap* heap);
int is_empty_(Heap* heap);





#endif // HEAP_H_INCLUDED
