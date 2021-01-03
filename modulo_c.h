#ifndef MODULO_C_H_   
#define MODULO_C_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIMBOLOS 256

typedef struct codFreq{
    char *cod;
    int freq;
} CODFREQ;

void printCenas(int , float , double , char *, int *, int *);

char tipoFicheiro(FILE *);

int nDigitos(int);
//funcao que retorna, a partir da matriz dos codigos e frequencias, o tamanho do codigo final e retorna esse tamanho(em bytes)
int tamanhoBlocoCodBits(CODFREQ[][SIMBOLOS], int );

//funcao que retira, a partir da matriz dos codigos e frequencias, o tamanho do código final e retorna esse tamanho (em bits)
int tamanhoBlocoCod(CODFREQ[][SIMBOLOS], int);

//funcao que a partir do ficheiro .cod constroi o campo do codigo dos caracteres (.cod) da matriz de codigos e frequencias
void importCode(char*,int ,CODFREQ[][SIMBOLOS], int *);

//funcao que a partir do ficheiro .freq constroi o campo da frequencia dos caracteres (.freq) da matriz de codigos e frequencias
void importFreq(char* ,int ,CODFREQ[][SIMBOLOS]);

//funcao que constroi, a partir de um buffer (buffer) que contem os caracteres presentes num dado bloco, um outro buffer (Wbuffer) com os códigos concatenados. 
//A partir do Wbuffer, cria um buffer de bytes (bloco codificado), de forma a que este possa ser escrito no formato desejado.
unsigned char * codificaBloco(unsigned char * ,CODFREQ[][SIMBOLOS], int , int , int , int *);

//funcao que efetivamente codifica o ficheiro original bloco a bloco e cria o ficheiro binário final .shaf. Esta funcao le a partir do ficheiro origial para um buffer (com a funcao fread)
//e passa esse buffer como argumento para a funcao codificaFile, bem como os restantes argumentos necessarios a codificacao de um bloco. Esta funcao tambem armazena os tamanhos globais dos 
//ficheiros original e codificado de forma a determinar, posteriormente, a taxa de compressao do ultimo.
void codificaFile(char *, char , int, CODFREQ[][SIMBOLOS], int *, int*, char *, float *, float *);

//int moduloC(char *filename, int begin);
int main(int , char **);



#endif 