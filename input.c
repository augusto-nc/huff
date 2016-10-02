#include "input.h"


byte* read_bytes(char* file,long* size_of_bytes){

FILE* fileptr = fopen(file, "rb");  // Abre arquivo no modo binario

if(fileptr==NULL){
    return NULL;
}


fseek(fileptr, 0, SEEK_END);          // vai para o final do arquivo
*size_of_bytes = ftell(fileptr);      // Pega o offset da posicao atual do arquivo
rewind(fileptr);                      // volta para o comeco do arquivo
int size=*size_of_bytes;
byte* buffer;
buffer = (byte*)malloc((*size_of_bytes)*sizeof(byte)); // Aloca memoria
fread(buffer, sizeof(byte), *size_of_bytes, fileptr); // Ler o arquivo
fclose(fileptr);
return buffer;

}

void write_bytes(char* name_file,long size_of_bytes,byte* bytes){
    FILE* fileptr = fopen(name_file, "wb");  // Abre arquivo no modo binario
    fwrite(bytes,sizeof(byte),size_of_bytes,fileptr); //Escreve os bytes do arquivo
    fclose(fileptr); //fecha o arquivo.
}




