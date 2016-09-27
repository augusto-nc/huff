#ifndef HELP_H_INCLUDED
#define HELP_H_INCLUDED

#include "input.h"

double_byte set_bit(double_byte b,int i){
    double_byte mask= 1<< (15-i);
    return b | mask;
}

int is_bit_i_set(byte c, int i){
    byte mask = 1 << (7-i);
    return mask & c;
}

#endif // HELP_H_INCLUDED
