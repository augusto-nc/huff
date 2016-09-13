#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "compact.h"
#define error_no_argument 1
#define error_open_file 2

void error(int erro);








int main(int argc, const char* argv[])
{
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
        if(argv[len_name-1]=='f' && argv[len_name-2]=='f' && argv[len_name-3]=='u' && argv[len_name-4]=='h' ){
                //descompactar arquivo.
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



