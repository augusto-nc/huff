#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED
#include <stdio.h>
#define asterisco 0x2A
#define escape 0x5C

typedef unsigned char byte;
typedef unsigned int four_byte;
typedef unsigned short double_byte;

byte* read_bytes(char* name_file,long* size_of_bytes);

void write_bytes(char* name_file,long size_of_bytes,byte* bytes);

















#endif // INPUT_H_INCLUDED
