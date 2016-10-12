
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "compact.h"
#include "heap.h"
#include "three.h"
#include "string.h"
#include "openssl/md5.h"
#define error_no_argument 1
#define error_open_file 2
#define error_extensao 3
#define error_senha 4


void error(int erro);

void inicializarInteface(){





}

//Recebe dois hashs de senha e retorna 1 se os hashs forem iguais
int senha_compare(byte* senha1,byte* senha2,int size){
    for(int i=0;i<size;i++){
        if(senha1[i]!=senha2[i])
            return 0;
    }
    return 1;
}


void compress(int nivel,char* adr_file,char* senha){
    int len_name=strlen(adr_file);
    char new_adr_file[256];  //endereco do novo arquivo
    char ext[7];            //nome da extensao
    char name_file[128];    //nome do arquivo

//############################################################
//###### OBTER EXTENSAO DO ARQUIVO
    char* last=strrchr(adr_file,'.'); //ponteiro que  aponta para ultimo ponto
    byte sizeExt=0;
    if(last!=NULL){    //se achar um ponto
        byte aux=(last-adr_file); //quantidade de caracteres ate o ponto
        sizeExt=len_name-aux-1; //calcula tamanho da extensao
    }
    if(!(sizeExt>0 && sizeExt<7))// se extensao tiver fora do intervalo
        error(error_extensao);
    strcpy(ext,&(adr_file[len_name-sizeExt]));//copia a extensao para a string ext
//#################################################################################



//#########################################################
//######## CRIAR ENDERECO DO ARQUIVO DE SAIDA
    strcpy(new_adr_file,adr_file);
    strcpy(&(new_adr_file[len_name-sizeExt]),"huff");
//###########################################################################



//#########################################################
//############# CALCULAR O TAMANHO DO NOME DO ARQUIVO
    int nameSize;//tamanho do nome do arquivo
    last=strrchr(adr_file,'/');//acha a ultima barra
    if(last==NULL){//caso nao ache barra:
        last=strrchr(adr_file,'\\');//acha a ultima contra barra
    }
    if(last!=NULL){//se achar uma barra
       int aux=(last-adr_file);//posicao ate a barra
       nameSize= len_name-aux-sizeExt-2;//calcula o tamanho do nome do arquivo
    }else{//se não achar:
        nameSize=len_name-sizeExt-1;//entao o nome do é o arquivo todo
                                    //menos a extensao menos o ponto
    }
    strcpy(name_file,&(adr_file[len_name-nameSize-sizeExt-1]));//copia o nome do arquivo para name_file
    name_file[nameSize]='\0';
//###########################################################################



//##################################################################################
//################## LER E COMPACTAR O ARQUIVO
    long size_of_file;
    byte* data=read_bytes(adr_file,&size_of_file);//ler o arquivo e coloca em "data"
    if(data==NULL){
        error(error_open_file);
    }
    byte* out;
    long size_file_out;
    for(int i=0;i<nivel;i++){//aplica a compactacao  quantidade de vezes de "nivel"
        size_file_out=0;
        out=compact(data,size_of_file,&size_file_out);
        size_of_file=size_file_out;
        free(data);
        data=out;
    }
    byte byte1= (out[0]<<3);//tira o tamanho do lixo
    byte1= byte1>>3 ;
    double_byte sizeHeader= (byte1 <<8) | out[1];//size Header é o tamanho do cabecalho do
                                                //Huffman Original. ou seja: (tamanho da arvore + 2)
    sizeHeader+=2;
//##########################################################################


//###########################################################################
//##################### CALCULAR O HASH MD5 DA SENHA
    byte senhaSize=MD5_DIGEST_LENGTH; //tamanho do Hash da senha
    byte senhaMd5[MD5_DIGEST_LENGTH]; //Hash da senha em MD5
    MD5_CTX c;    //Struct
    MD5_Init(&c);  //inicia Md5
    MD5_Update(&c, senha,(unsigned long)strlen(senha));//passa a senha para fazer o Hash
    MD5_Final(&(senhaMd5[0]), &c);//coloca o hash da senha em senhaMd5
//###################################################



//##########################################################################
//################# DESLOCAR BYTES PARA COLOCAR NOVO CABECALHO
    int shift=1+sizeExt+senhaSize+1+nameSize;//tamanho do novo cabecalho
    for(long i=size_file_out-1;i>=sizeHeader;i--){//deslocando bytes
        out[i+shift]=out[i];
    }
 //####################################################################


 //############################################################################
 //############### GRAVANDO NOVO CABECALHO
      out[sizeHeader]=sizeExt<<5 | senhaSize ; //tamanho da extensao e e tamanho da senha
    for(int i=0;i<sizeExt;i++){//gravando extensao
       out[sizeHeader+1+i]= ext[i];
    }
    for(int i=0;i<senhaSize;i++){//gravando senha
       out[sizeHeader+sizeExt+1+i]= senhaMd5[i];
    }
    long index=sizeHeader+sizeExt+1+senhaSize;
    out[index]=nameSize;
    index++;
    for(int i=0;i<nameSize;i++){
       out[index+i]= name_file[i];
    }
//########################################################################


    write_bytes(new_adr_file,size_file_out+shift,out);
    free(out);
}

void descompress(int nivel,char* adr_file,char* senha){
    int len_name=strlen(adr_file);
    char new_adr_file[256];
    char ext[7];
    char name_file[128];

    long size_of_file;
    byte* data=read_bytes(adr_file,&size_of_file);


    if(data==NULL){
        error(error_open_file);
    }

    byte byte1= (data[0]<<3);
    byte1= byte1>>3 ;
    double_byte sizeHeader= (byte1 <<8) | data[1];
    sizeHeader+=2;

    byte size_ext=data[sizeHeader]>>5;
    byte senha_size=data[sizeHeader]<<3;
    senha_size=senha_size>>3;
    byte senhaMd5[senha_size];

    int i;

    for(i=0;i<size_ext;i++){
       ext[i]= data[sizeHeader+1+i];;
    }
    ext[i]='\0';

    for(i=0;i<senha_size;i++){
       senhaMd5[i]= data[sizeHeader+size_ext+1+i];;
    }


    byte senhaSize=MD5_DIGEST_LENGTH;
    byte senhaMd5_user[MD5_DIGEST_LENGTH];
    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, senha, (unsigned long)strlen(senha));
    MD5_Final(&(senhaMd5_user[0]), &c);


    if(!senha_compare(senhaMd5_user,senhaMd5,senha_size)){
        error(error_senha);
    }

    int index=sizeHeader+size_ext+senha_size+1;

    byte name_size=data[index];
    index++;
    for(i=0;i<name_size;i++){
       name_file[i]= data[index+i];;
    }
    index+=name_size;
    name_file[i]='\0';

    printf("Size Ext %d\n",size_ext);
    printf("Senha md5 %s\n",senhaMd5);
    printf("Name size %d\n",name_size);
    printf("Senha md5 %s\n",senhaMd5_user);
    printf("name file %s\n",name_file);



    printf("Size senha %d\n ",senha_size);
    printf("Size senha %s\n ",ext);

    int shift=1+size_ext+senha_size+1+name_size;

    for(long i=0;i<size_of_file-sizeHeader-shift;i++){
        data[sizeHeader+i]=data[i+sizeHeader+shift];
    }

    byte* out;
    size_of_file=size_of_file-shift;
    long size_file_out;
    for(int i=0;i<nivel;i++){
        size_file_out=0;
        out=descompact(data,size_of_file,&size_file_out);
        size_of_file=size_file_out;
        free(data);
        data=out;
    }

    char* last=strrchr(adr_file,'/');
    if(last==NULL){
        last=strrchr(adr_file,'\\');
    }

    if(last==NULL){
        strcpy(new_adr_file,name_file);
        strcpy(&(new_adr_file[strlen(name_file)]),".");
        strcpy(&(new_adr_file[strlen(name_file)+1]),ext);
    }else{
        int pos=last-adr_file;
        strcpy(new_adr_file,adr_file);
        strcpy(&(new_adr_file[pos+1]),name_file);
        strcpy(&(new_adr_file[strlen(name_file)+pos+1]),".");
        strcpy(&(new_adr_file[strlen(name_file)+pos+2]),ext);
    }
    printf("adr_new_file %s  ",new_adr_file);


    write_bytes(new_adr_file,size_file_out,out);
    free(out);
}




int main(int argc, const char* argv[])
{

    if(argc<3){

       inicializarInteface();
       return 0;
    }

    int len_adr=strlen(argv[1]);
    int nivel=1;
    if(argc>3){
        nivel=atoi(argv[3]);
    }

    if(strcmp(&(argv[1][len_adr-5]),".huff")==0){
        descompress(nivel,argv[1],argv[2]);
    }else{
        compress(nivel,argv[1],argv[2]);
    }

    return 0;
}



void error(int erro){
    switch (erro){
        case error_no_argument:
            printf("Nenhum arquivo foi passado como argumento\n");
            break;
        case error_open_file:
            printf("Erro ao abrir o arquivo\n");
            break;
        case error_extensao:
            printf("Extensao invalida\n");
        case error_senha:
            printf("Senha nao confere\n");

    }
     exit(0);
}



