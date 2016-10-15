#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "compact.h"
#include "heap.h"
#include "three.h"
#include "string.h"
#include "openssl/md5.h"
#include <sys/stat.h>
#include <dirent.h>
#include "list.h"
#define error_no_argument 1
#define error_open_file 2
#define error_extensao 3
#define error_senha 4
typedef struct InfoFile{
    char file;
    char* path;
} InfoFile;



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



//#########################################
//############ nivel : quantas vez o algoritmos do huffman sera executado
//############ adr_file : endereco completo do arquivo que sera compacatado ex: C:/pasta/imagem.bmp
//############ relativePath : é NULL se tiver sendo compactado apenas um arquivo, e é o caminho ate o arquivo partindo da pasta incial
//se tiver sendo compactado uma pasta ex: pasta/imagem.bmp
//############ senha que sera gravada no arquivo .huff

void compress(int nivel,char* adr_file,char* relativePath,char* senha){
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
    int nameSize=0;//tamanho do nome do arquivo
    int auxNameSize=0;
    if(relativePath!=NULL){//se relativePath for difernte de NULL, então é acrecentada ao nome do arquivo
        auxNameSize=strlen(relativePath)+1;
        strcpy(name_file,relativePath);
        strcat(name_file,"/");
    }else{
        name_file[0]='\0';
    }

    last=strrchr(adr_file,'/');//acha a ultima barra
    if(last==NULL){//caso nao ache barra:
        last=strrchr(adr_file,'\\');//acha a ultima contra barra
    }
    if(last!=NULL){//se achar uma barra
       int aux=(last-adr_file);//posicao ate a barra
       nameSize+= len_name-aux-sizeExt-2;//calcula o tamanho do nome do arquivo
    }else{//se não achar:
        nameSize+=len_name-sizeExt-1;//entao o nome do é o arquivo todo
                                    //menos a extensao menos o ponto
    }
    strcat(name_file,&(adr_file[len_name-nameSize-sizeExt-1]));//copia o nome do arquivo para name_file
    nameSize+=auxNameSize;
    name_file[nameSize]='\0';
    printf("name file %s\n",name_file);
    printf("relative path %s\n",relativePath);


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


//#########################################
//############## nivel: quantas vez o algoritmos do huffman sera executado
//############## adr_file : endereco completo do arquivo que sera compacatado ex: C:/pasta/imagem.bmp
//############## senha : que sera comparado com a que foi gravada no .huff
//############## data : arquivo .huff que sera descompactado
//############## size_of_file :  tamanho do array 'data'
void descompress(int nivel,char* adr_file,char* senha,byte* data,long size_of_file){
    int len_name=strlen(adr_file);
    char new_adr_file[256]; //endereco completo do arquivo decompactado
    char ext[7];   //extensao do arquivo descompactado
    char name_file[128]; //nome do arquivo descompactado

    //###############################################################
    //################# OBTENDO TAMANHO DO CABECALHO DO HUFF ORIGINAL (2 bytes + tamanho da arvore)
    byte byte1= (data[0]<<3);
    byte1= byte1>>3 ;
    double_byte sizeHeader= (byte1 <<8) | data[1];
    sizeHeader+=2;
    //###############################################################


    //##################################################################
    //################### OBTENDO TAMANHO DA EXTENSAO DA SENHA
    byte size_ext=data[sizeHeader]>>5;
    byte senha_size=data[sizeHeader]<<3;
    senha_size=senha_size>>3;
    //#############################################################


    //#########################################################
    //################## COPIANDO EXTENSAO E SENHA PARA SEUS ARRAYS
    byte senhaMd5[senha_size];//array com o hash da senha em Md5
    int i;
    for(i=0;i<size_ext;i++){
       ext[i]= data[sizeHeader+1+i];
    }
    ext[i]='\0';
    for(i=0;i<senha_size;i++){
       senhaMd5[i]= data[sizeHeader+size_ext+1+i];;
    }
    //##########################################################

    //###############################################################
    //##################### GERANDO COMPARANDO MD5 DA SENHA PASSADO PELO USUARIO
    byte senhaSize=MD5_DIGEST_LENGTH;   // MD5_DIGEST_LENGTH = tamanho do hash MD5
    byte senhaMd5_user[MD5_DIGEST_LENGTH]; //hash da senha passada no momento da descompactacao
    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, senha, (unsigned long)strlen(senha));
    MD5_Final(&(senhaMd5_user[0]), &c);

    if(!senha_compare(senhaMd5_user,senhaMd5,senha_size)){ //comparando hash MD5 das senhas
        error(error_senha);
    }
    //################################################################


    //####################################################################
    //################### OBTENDO NOME ORIGINAL DO ARQUIVO E CRIANDO O ENDERECO DE SAIDA
    int index=sizeHeader+size_ext+senha_size+1;// atualiza a posicao no array,para o que ja foi lido
    byte name_size=data[index]; //tamanho do nome do arquivo
    index++;
    for(i=0;i<name_size;i++){ //copiando nome do arquivo
       name_file[i]= data[index+i];;
    }
    index+=name_size; //atualizando posicao nos bytes de entrada
    name_file[i]='\0';

     char* last=strrchr(adr_file,'/');//achando ultima barra no endereco .huff
    if(last==NULL){
        last=strrchr(adr_file,'\\');
    }
    if(last==NULL){// se não houver barra, o .huff esta na pasta do executavel
        //entao new_adr_file tera o nome do arquivo original + a extensao
        strcpy(new_adr_file,name_file);
        strcpy(&(new_adr_file[strlen(name_file)]),".");
        strcpy(&(new_adr_file[strlen(name_file)+1]),ext);
    }else{
        int pos=last-adr_file;
        strcpy(new_adr_file,adr_file);//copia o endereco do .huff
        strcpy(&(new_adr_file[pos+1]),name_file); //copia o nome original depois da ultima barra
        strcpy(&(new_adr_file[strlen(name_file)+pos+1]),".");//coloca o ponto
        strcpy(&(new_adr_file[strlen(name_file)+pos+2]),ext); //coloca a extensao
    }
    //#########################################################################

    //######################################################################
    //################### REMOVE O NOVO CABECALHO
    int shift=1+size_ext+senha_size+1+name_size; //tamanho do novo cabecalho
    for(long i=0;i<size_of_file-sizeHeader-shift;i++){
        data[sizeHeader+i]=data[i+sizeHeader+shift];
    }
    //#################################################################


    //####################################################################
    //###########################  EXECUTA O ALGORITMO DO HUFFMAN E ESCREVE OS BYTES DESCOMPACTADOS
    byte* out;
    size_of_file=size_of_file-shift;
    long size_file_out;
    for(int i=0;i<nivel;i++){//executa o algoritmo de HuffMan 'nivel' vezes
        size_file_out=0;
        out=descompact(data,size_of_file,&size_file_out);
        size_of_file=size_file_out;
        free(data);
        data=out;
    }
    write_bytes(new_adr_file,size_file_out,out);//escreve o arquivo descompactado em new_adr_file
    //#####################################################################
    free(out);
}



//#########################################
//############## nivel: quantas vez o algoritmos do huffman sera executado
//############## adr_file : endereco completo do arquivo que sera descompactado ex: C:/pasta/imagem.bmp
//############## senha : que sera comparado com a que foi gravada no .huff
void descompressAll(int nivel,char* adr_file,char* senha){
    long size_of_file;
    int modePasta=0;
    int index=0;
    byte* data=read_bytes(adr_file,&size_of_file);
    double_byte* cod=data; // codigo para saber se é uma pasta ou um arquivo

    if(data==NULL){
        error(error_open_file);
    }
    if(*cod==0){//se tiver uma pasta compactada
        index=2;
        char path[256];// endereco que a pasta vai ser decompactada
        strcpy(path,adr_file);//endereco do .huff
        path[strlen(path)-5]='\0'; //remove a extensao
        mkdir(path); // cria a pasta
        //Ex: se o arquivo for c:/arquivo.huff sera criado a pasta 'c:/arquivo' a pasta compacatada
        // vai ser descompactada dentro dessa pasta ou seja: c:/arquivo/pastaCompactada

          while(index<size_of_file-1){//percorre ate o final do arquivo
            long* size=&data[index]; //Obtem o codigo '0' ou tamnho do proximo bloco
            index+=sizeof(long);
            if(*size==0){//se for '0' é uma pasta
                   byte lenFolder;//tamanho do endereco relativo da pasta
                   lenFolder=data[index];// obtem esse tamanho
                    index++;
                    char pathFolder[lenFolder+1];
                      int i;
                     for(i=0;i<lenFolder;i++){//copia o endereco da pasta para pathFolder
                        pathFolder[i]= data[index+i];;
                     }
                      pathFolder[lenFolder]='\0';
                    char full_path[strlen(path)+lenFolder+2];// endereco completo onde a pasta vai ser criada
                    strcpy(full_path,path);// copia path para full_path, ex: c:/arquivo
                    strcat(full_path,"/");
                    strcat(full_path,pathFolder); //concatena o endereco relativo em full_path ex: c:/arquivo/pasta
                    mkdir(full_path);// cria a pasta
                    index+=lenFolder;// atualiza posica de indes
            }else{// é um arquivo
                char auxPath[256];
                strcpy(auxPath,path);//coloca o endereco de descompactacao em auxPath
                strcat(auxPath,"/");
                strcat(auxPath,"a.qlq"); // coloca um nome qualquer para o arquivo,
                //esse nome sera subtituido dentro de decompress pelo endereco correto.
                descompress(nivel,auxPath,senha,&data[index],*size);//passa o ponteiro 'data' apontado para a posicao correta
                index+=*size;
            }
         }
    }else{// se for somente um arquivo
        descompress(nivel,adr_file,senha,data,size_of_file);
    }

    free(data);
}



//#########################################
//############## folder: pasta que tera seus arquivos compactados
//############## adr_folder : endereco completo ate a pasta que sera compacatado ex: C:/pasta/subPasta
//############## relative_path : endereco relativo da pasta que sera compatcatado ex: pasta/subPasta
//############## senha : senha passada pelo usuario pelo usuario
//############## listFiles :  Lista de arquivo e pastas compactados

void compactAllFiles(DIR* folder,char* adr_folder,char* relative_path,char* senha,List* listFiles){
        struct dirent *ent; //informacoes de um arquivo ou pasta.
        char subFile[256];
        char subRelativePath[256];// relative Path de uma pasta dentro de folder

        while (ent = readdir(folder))  //percorre todos os arquivos e pastas de folder ate a saida ser NULL
            if(strcmp(ent->d_name,".")==0  || strcmp(ent->d_name,"..")==0 )//Ignora informacao que nao sao é pastas ou arquivos
                continue;

            //##########
            //### CONCATENA O ENDERECO COMPLETO DA PASTA  COM UMA SUBPASTA OU ARQUIVO
            strcpy(subFile,adr_folder);
            strcat(subFile,"/");
            strcat(subFile,ent->d_name);
            //#########
            DIR* subFolder=opendir(subFile); //tenta abrir como uma pasta
            if(subFolder!=NULL){ // se for diferente de NULL entao é uma pasta

                //#######################
                //#### CONCATENA relative_path COM O NOME DA SUBPASTA
                strcpy(subRelativePath,relative_path);
                strcat(subRelativePath,"/");
                strcat(subRelativePath,ent->d_name);//concatena com o nome da SUBPASTA
                InfoFile* info= malloc(sizeof(InfoFile));
                info->file=0;                    //file=0 siginica que nao é um arquivo e sim uma pasta
                info->path=malloc(sizeof(char)*strlen(subFile)+4);
                strcpy(info->path,subRelativePath);//colocao o endereco relativo  em info->path
                add(listFiles,(void*)info); //adiciona à lista de pastas e arquivos
                //#######################

                compactAllFiles(subFolder,subFile,subRelativePath,senha,listFiles);//compacata todos os arquivos dessa subpasta
                closedir(subFolder);
            }else{
                InfoFile* info= malloc(sizeof(InfoFile));
                info->file=1; //file=1 significa que um arquivo
                info->path=malloc(sizeof(char)*strlen(subFile)+4);// aloca o tamanho de subfile
                //                                                +4 para garantir que vai caber a extensao .hufT

                char* last=strrchr(subFile,'.');//acha o ponto de subfile
                strcpy(info->path,subFile);    // copia o nome do arquivo para info->path
                strcpy(&(info->path[last-subFile+1]),"huff\0"); //substitui a extensao por .huff

                compress(1,subFile,relative_path,senha);// compacta o arquivo  e o grava no mesmo
                // endereco que info->path
                add(listFiles,(void*)info);// adiciona o arquivo na lista de arquivos
            }
        }
}




int main(int argc, const char* argv[])
{
    if(argc<3){
       inicializarInteface();
       return 0;
    }

    DIR* dir=opendir(argv[1]);// tenta abri como uma pasta
    if(dir){ // Se for dirferente de NULL então é uma pasta

        //#################################################
        //############### DEFINE A relativePath
        //ex: argv[1]="c:/pasta" relativePath sera 'pasta'
        char* relativePath=strrchr(argv[1],'/');//acha a ultima barra
        if(relativePath==NULL)
            relativePath=strrchr(argv[1],'\\');
        if(relativePath==NULL){//se for NULL entao nao existe pasta anteriores
            relativePath=argv[1];// relativePath vai ser igua a argv[1]
        }else{
            relativePath++; //incrementa mais 1, para relativePath não apontar para a a barra
        }
        //###########################################



        List* listFiles=createList();
        compactAllFiles(dir,argv[1],relativePath,argv[2],listFiles);//compacta todos os arquivos e subPastas de relativePath
        closedir(dir);

        //########
        //#### O endereco do arquivo de saida sera o endereco da pasta contatenada com a extensao .huff
        char nameFileOut[256];//nome do arquivo de saida
        strcpy(fileOut,argv[1]);
        strcat(fileOut,".huff");
        //######


        FILE* fileFinal=fopen(nameFileOut,"wb");// arquivo de saida
        Iterate* it=iterate(listFiles); //Iterador da lista de arquivos

        double_byte cod=0;
        fwrite(&cod,sizeof(double_byte),1,fileFinal);//escreve dois bytes com 0 no comeco do arquivo, significa que é uma pasta

        //########################################
        //############## Insere o codigo para criar a pasta que foi compactada
        long cod_folder=0;// codigo de pasta um long =0
        byte pathSize=strlen(relativePath);
        fwrite(&cod_folder,sizeof(long int),1,fileFinal);
        fwrite(&pathSize,sizeof(byte),1,fileFinal); //tamanho do nome da pasta
        fwrite(relativePath,sizeof(char),pathSize,fileFinal); // endereco relativo da pasta
        //#############################


        do{
             InfoFile* info=(InfoFile*)getVal(it);
             if(info->file){// é um arquivo
                long file_in;
                byte* b =read_bytes(info->path,&file_in); //ler o arquivo .huff temporario gerado
                fwrite(&file_in,sizeof(long),1,fileFinal); //escreve o tamanho do arquivo .huff
                fwrite(b,sizeof(byte),file_in,fileFinal); //escreve .huff temporario  no arquivo final
                free(b);
                remove(info->path); //deleta o arquivo do .huff temporario
             }else{ //é uma pasta
                //insere o codigo para criar uma pasta
                cod_folder=0;
                fwrite(&cod_folder,sizeof(long int),1,fileFinal);// codigo de pasta um long =0
                pathSize=strlen(info->path);// tamanho do endereco relativo da pasta
                fwrite(&pathSize,sizeof(byte),1,fileFinal);// grava tamanho do endereco relativo da pasta
                fwrite(info->path,sizeof(char),pathSize,fileFinal);// endereco relativo da pasta
             }
        }while(hasNext(&it));//passa para o proximo e retorna 1.(se houver proximo), se não retorna 0.

        fclose(fileOut);
    }else{
        int len_adr=strlen(argv[1]);
        int nivel=1;
        if(argc>3){// ser tiver passado mais de 2 argumentos
            nivel=atoi(argv[3]); //pega oo n
        }

        if(strcmp(&(argv[1][len_adr-5]),".huff")==0){
            descompressAll(nivel,argv[1],argv[2]); // descompacta 1 arquivo (Pode ter uma pasta compactada)
        }else{
            compress(nivel,argv[1],NULL,argv[2]); // compacta 1 arquivo
        }
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



