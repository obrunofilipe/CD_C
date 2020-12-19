#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIMBOLOS 256


int nBlocks(FILE *);
//void importCode(char*,int,int **);
int main(int , char **);

typedef struct codFreq{
    char *cod;
    int freq;
} CODFREQ;