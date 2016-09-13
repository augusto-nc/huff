#include "compact.h"
#include "heap.h"
#include "three.h"
#include "help.h"



/*
Funcao que determina se um no fica na frente do outro.
retorna 1 se b deve ficar na frente de a ou -1 se a deve ficar na frente de b.

*/
int compare(void* a,void* b){
    if(a==NULL)
        return 1;
    if(b==NULL)
        return -1;

    if(((Node*)a)->qtd<((Node*)b)->qtd )
        return -1;
    if(((Node*)a)->qtd>((Node*)b)->qtd )
        return 1;

    if(length((Node*)a)<length((Node*)b))//caso a qantidade de ocorrencia for igual. o criterio é
        return 1;                       //o tamanho  da arvore
    return -1;                          // se for igual: "a" fica na frente.

}



/*
Cria a tabela de reprentacao de bytes e calcula o tamanho da arvore binaria
*/
void createTable(int* qtd_bits,double_byte* bits,int pos,double_byte current,Node* node,unsigned short* three_size){

        if(node->valor==asterisco || node->valor==escape) //quando o caractere e um asteriscos u um escape, entao sera utilizado dois caracteres para grava-lo
           (*three_size)++;
        (*three_size)++;
        if(node->left==NULL && node->right==NULL){//Quando left e right são NULL significa que e uma folha
                byte valor=node->valor;
                bits[valor]=current;              //Coloca os bits no local correpondente a dterminado byte
                qtd_bits[valor]=pos+1;            //grava a quantidade bits que são utilizados na representacao
        }else{
            createTable(qtd_bits,bits,pos+1,current,node->left,three_size);
            createTable(qtd_bits,bits,pos+1,set_bit(current,pos+1),node->right,three_size);
        }
}

/*
Escreve a arvore binaria no array de bytes
*/

void writeThree(Node* node,byte* data,int* pos){
        if(node->left==NULL && node->right==NULL){//no folha
              if(node->valor==asterisco || node->valor==escape){
                    (*pos)++;
                    data[*pos]=(byte)escape; //necessario adicionar o caractere de escape
              }
               (*pos)++;
               data[*pos]=node->valor;

        }else{
            (*pos)++;
            data[*pos]=(byte)asterisco;   //grava o asterisco
            writeThree(node->left,data,pos);
            writeThree(node->right,data,pos);
        }
}


byte* compact(byte* data,long size_of_data,long* size_of_file_out){
    Node* nodes[256];      //array com todos os tipos de bytes

    for(int i=0;i<256;i++){//criando um no para cada tipo de byte existente
        nodes[i]=createNode();
        nodes[i]->valor=(byte)i;
    }

    byte d;
    for(long i=0;i<size_of_data;i++){//contagem de quantos bytes existem por tipo de byte
        d=data[i];
        nodes[d]->qtd++;
    }

    Heap* heap= createHeap(256,compare);//Cria a heap

      for(int i=0;i<256;i++){//Percorre todos os tipos de bytes
        if(nodes[i]->qtd!=0){ //Adiciona na fila os nodes que aparecem pelo menos uma vez
            enqueue(heap,(void*)nodes[i]);
        }
    }

    Node* aux1;
    Node* aux2;
    Node* mergeNode;
    int size=heap->size_;
    while(heap->size_>1){ //Realiza o merge dos nodes com menor quantidade de ocorrencias
        int size=heap->size_;
        aux1= (Node*)dequeue(heap);
        aux2= (Node*)dequeue(heap);
        byte valor=aux1->valor;
        valor=aux2->valor;
        mergeNode=createNode();
        mergeNode->qtd=aux1->qtd+aux2->qtd;
        mergeNode->left=aux1;
        mergeNode->right=aux2;
        enqueue(heap,mergeNode);
    }


    int qtd_bits[256];// quantidade de bits para cada byte
    for(int i=0;i<256;i++)
        qtd_bits[i]=0;// quantidade bits comeca com 0

    double_byte bits[256]; //novo mapeamento de bits para cada byte
    Node* root=(Node*)dequeue(heap);

    unsigned short three_size=0; //tamanho da arvore binaria

    createTable(qtd_bits,bits,-1,0,root,&three_size);
    long bytes_file_out=0;
    byte trash=0;
    int bit=0;

    for(int i=0;i<size_of_data;i++){// contando quantidade bytes que os dados terao na saida
        d=data[i];
        bit+=qtd_bits[d];
        bytes_file_out+=bit/8;
        bit=bit%8;
    }

    if(bit>0){//verifica se sobrou bits
        trash=8-bit; //tamanho do trash
        bytes_file_out++;
    }
    byte* bytes=(byte*)malloc(sizeof(byte)*(bytes_file_out+2+three_size));// Alocao os bytes do arquivo de saida

    bytes[0]=trash;       //colocar o trash no comeco do arquivo
    bytes[0]=bytes[0]<<5; //move trash para esquerda para ocupar apenas os 3 primeiros bits

    byte aux=(byte)(three_size>>8);//

    bytes[0]=bytes[0] | aux; //coloca os 5 bits do tamanho da arvore no primeiro byte do arquivo
    bytes[1]=(byte)three_size; //coloca o resto do tamanho da arvore no outro byte do arquivo
    int index=1;

    writeThree(root,bytes,&index); //escreve a arvore no array de bytes de saida

    index=2+three_size;

    double_byte aux_dt;
    byte* aux_byte=&aux_dt;
    bit=0;
    int qAux;
    for(int i=0;i<size_of_data;i++){
        d=data[i];
        aux_dt=bits[d];

        qAux=qtd_bits[d];
        while(qAux!=0){
            bytes[index]= bytes[index] | aux_byte[1]>>bit;
            if(8-bit>qAux){
                bit+=qAux;
                qAux=0;
            }else{
                qAux-=8-bit;
                aux_dt=aux_dt<<(8-bit);
                bit=0;
                index++;
            }
        }
    }
    *size_of_file_out=bytes_file_out+2+three_size;

    return bytes;
}

