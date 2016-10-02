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

"int* qtd_bits" é um vetor com tamanho de 256, que grava a quantidade de bits compactados para cada tipo de byte.
"double_byte* bits" é um vetor com tamanho de 256, que grava os bits compactados para cada tipo de byte.
"double_byte current" armazena a representacao atual byte

*/
void createTable(int* qtd_bits,four_byte* bits,int pos,four_byte current,Node* node,unsigned short* three_size){

        (*three_size)++;
        if(node->left==NULL && node->right==NULL){//Quando left e right são NULL significa que e uma folha

            if(node->valor==asterisco || node->valor==escape) //quando o caractere e um asteriscos u um escape, entao sera utilizado dois caracteres para grava-lo
                (*three_size)++;

                byte valor=node->valor;
                bits[valor]=current;              //Coloca os bits no local correpondente a dterminado byte
                qtd_bits[valor]=pos+1;            //grava a quantidade bits que são utilizados na representacao

        }else{
            createTable(qtd_bits,bits,pos+1,current,node->left,three_size);
            createTable(qtd_bits,bits,pos+1,set_bit(current,pos+1),node->right,three_size);// o "current" tem o bit setado com 1 na posicao "pos+1" para o nó do lado direito.
        }
}

/*
Escreve a arvore binaria no array de bytes
*/

void writeThree(Node* node,byte* data,int* pos){
        if(node->left==NULL && node->right==NULL){//no folha
              if(node->valor==asterisco || node->valor==escape){// quando o valor do byte é um asterisco ou um escape é preciso adicionar um caractere de escape antes
                    (*pos)++;
                    data[*pos]=(byte)escape; //necessario adicionar o caractere de escape
              }
               (*pos)++;
               data[*pos]=node->valor;

        }else{// nao é um no folha
            (*pos)++;
            data[*pos]=(byte)asterisco;   //grava o asterisco
            writeThree(node->left,data,pos);
            writeThree(node->right,data,pos);
        }
}


byte* compact(byte* data,long size_of_data,long* size_of_file_out){
    Node* nodes[256];      //array com todos os tipos de bytes

    for(unsigned short i=0;i<256;i++){//criando um no para cada tipo de byte existente
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
            int qtd1=nodes[i]->qtd;
            enqueue(heap,(void*)nodes[i]);
        }
    }

    Node* aux1;
    Node* aux2;
    Node* mergeNode;
    int size=heap->size_;
    while(heap->size_>1){ //Realiza o merge dos nodes com menor quantidade de ocorrencias
        aux1= (Node*)dequeue(heap);// obtem os nós com menores valores
        aux2= (Node*)dequeue(heap);
        int qtd1=aux1->qtd;
        int qtd2=aux2->qtd;

        mergeNode=createNode();
        mergeNode->qtd=aux1->qtd+aux2->qtd;
        mergeNode->left=aux1;
        mergeNode->right=aux2;
        enqueue(heap,mergeNode);
    }


    int qtd_bits[256];// quantidade de bits para cada byte
    four_byte bits[256]; //novo mapeamento de bits para cada byte
    for(int i=0;i<256;i++){
        qtd_bits[i]=0;// quantidade bits comeca com 0
        bits[i]=0x0;
    }


    Node* root=(Node*)dequeue(heap);// Obtem a raiz da arvore

    unsigned short three_size=0; //tamanho da arvore binaria

    createTable(qtd_bits,bits,-1,0x0,root,&three_size);
    /*
    for(int i=0;i<256;i++){
        if(qtd_bits[i]>0){
                printf("%d %c  qtd: %d cod: %ud\n",i,(byte)i,qtd_bits[i],bits[i]);
        }
    }
    */


    long bytes_file_out=0;
    byte trash=0;
    int bit=0;

    for(int i=0;i<size_of_data;i++){// contando quantidade bytes que os dados terao na saida
        d=data[i];
        bit+=qtd_bits[d];// incrementa "bit" com quantidade de bits do byte "d".
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

     for(long i=2+three_size;i<bytes_file_out+2+three_size;i++){
        bytes[i]=0;
    }

    index=2+three_size;

    four_byte aux_dt;
    byte* aux_byte=&aux_dt;
    bit=0;
    int qAux;

    //grava os bits compactados no vetor bytes.
    for(int i=0;i<size_of_data;i++){
        d=data[i];
        aux_dt=bits[d];// bits que reprensentam o byte "d" sao colocados no em aux_dt

        qAux=qtd_bits[d]; // qAux recebe a quantidade bits do byte "d"
        while(qAux>0){//continuar até gravar todos os bits
            bytes[index]= bytes[index] | (aux_byte[3]>>bit);//aux_byte[1]>>bit; // aux_byte[1] acessa o byte mais a esquerda de aux_dt.
            //                          "bit" armazena a quantidade de bits que ja foram gravados na posicao index,
            //                           por isso é necessario deslocar para direita para não sobescrever os bits ja gravados
            if(8-bit>qAux){     // quando quantidade de bits gravados é maior que os bits que restavam (qAux)
                bit+=qAux;      // incrementa a quantidade de bits gravados na posicao "index"
                qAux=0;         // todos o bits foram gravados
            }else{              //quando é menor
                qAux-=(8-bit); //subtrai a quantidade bits gravados de qAux.
                aux_dt=aux_dt<<(8-bit); //aux_dt é deslocado para esquerda na quantidade de bits que ja foram gravados
                bit=0;          //nao tem mais espaco em na posicao "index" para gravar, então passar para a proxima posicao
                index++;
            }
        }
    }
    *size_of_file_out=bytes_file_out+2+three_size;//tamanho final do arquivo é colocado no ponteiro que veio como argumento.

    return bytes;
}

