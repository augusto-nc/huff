#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "compact.h"
#include "heap.h"
#include "three.h"
#define error_no_argument 1
#define error_open_file 2


void error(int erro);

int compare2(void* a,void* b){
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




int main(int argc, const char* argv[])
{
    /*
    Node* nodes[256];      //array com todos os tipos de bytes

    for(int i=0;i<256;i++){//criando um no para cada tipo de byte existente
        nodes[i]=createNode();
        nodes[i]->qtd=i;
}

    Heap* heap= createHeap(256,compare2);//Cria a heap

     for(int i=0;i<256;i++){//Percorre todos os tipos de bytes
            enqueue(heap,(void*)nodes[i]);

    }

    for(int i=0;i<128;i++){//Percorre todos os tipos de bytes
            printf("%d\n",((Node*)dequeue(heap))->qtd);
    }
            printf("\n\n\n\n");


     for(int i=0;i<128;i++){//Percorre todos os tipos de bytes
            nodes[i]->qtd=i*5;
            enqueue(heap,(void*)nodes[i]);
    }
     for(int i=0;i<256;i++){//Percorre todos os tipos de bytes
            printf("%d\n",((Node*)dequeue(heap))->qtd);
    }


   return 0;
   */
    if(argc<2){
        error(error_no_argument);
    }
    long size_of_file;
    byte* data;                 // dados do arquivo lido.
    data=read_bytes(argv[1],&size_of_file);
    if(data==NULL){
        error(error_open_file);
    }
    int len_name=strlen(argv[1]);
    if(len_name>5){// verifica se o arquivo termina com huff, se verdade, deve ser descompactado.
        if(argv[1][len_name-1]=='f' && argv[1][len_name-2]=='f' && argv[1][len_name-3]=='u' && argv[1][len_name-4]=='h' ){
                long size_file_out=0;
                byte* b=descompact(data,size_of_file,&size_file_out);
                char new_name[256];
                strcpy(new_name,argv[1]);
                if(argc>2){
                     write_bytes(argv[2],size_file_out,b);
                }else{
                    new_name[len_name-5]='\0';
                    write_bytes(new_name,size_file_out,b);
                }
                return 0;
        }
    }

    long size_file_out;
    byte* b=compact(data,size_of_file,&size_file_out);
    //free(data);

    char new_name[256];
    int k=0;
    for(int i=0;i<len_name;i++){
        new_name[i]=argv[1][i];
        if(argv[1][i]=='.')
            k=i;
    }
    if(k!=0){
        new_name[k+1]='h'; new_name[k+2]='u'; new_name[k+3]='f'; new_name[k+4]='f';;new_name[k+5]='\0';
    }else{
         new_name[len_name]='.'; new_name[len_name+1]='h';new_name[len_name+2]='u'; new_name[len_name+3]='f'; new_name[len_name+4]='f';new_name[len_name+5]='\0';
    }
    write_bytes(new_name,size_file_out,b);
   // free(b);

    return 0;
}



void error(int erro){
    switch (erro){
        case error_no_argument:
            printf("Nenhum arquivo foi passado como argumento\n");
            break;
        case error_open_file:
            printf("Erro ao abrir o arquivon");
            break;
    }
     exit(0);
}



