#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#define SIMBOLOS 256 
int nBlocks(FILE *cod){
    int n_blocks = 0;
    fseek (cod, 3,SEEK_SET);
    char c;
    while ((c = fgetc (cod)) != 64){
        n_blocks = n_blocks * 10 + (c - 48);
    }
    return n_blocks;
}
void printMatriz(int (*matrix)[SIMBOLOS], int numberOfLines , int numberColumns){
    int row, columns;
    for (row=0; row<numberOfLines; row++)
    {
        for(columns=0; columns<numberColumns; columns++)
        {
            printf("%d     ", matrix[row][columns]);
        }
        printf("\n");
    }
}
void importCode( char *codFile){
    FILE *cod;
    int n_blocks,blocos,simbolo = 0;
    char c,ant;
    cod = fopen(codFile,"rb");
    if(!cod) printf("nao existe"); 
    else {
        n_blocks = nBlocks(cod);
        printf("%d\n",n_blocks);
        int matrizCodigos[n_blocks][256];
        while((c = fgetc(cod)) != 64);
        blocos = 1;
        ant = ';';
        
        while(blocos <= n_blocks){
            c = fgetc(cod);
            //printf(" (%c) ",c);
            if(c == ';' && ant == ';'){ 
                matrizCodigos[blocos - 1][simbolo] = -1;
                //printf("%d 1ºif ",matrizCodigos[blocos -1][simbolo]);
                simbolo++;
                ant = c;
            }
            else if(c == '@'){
                if(ant == ';') matrizCodigos[blocos - 1][simbolo] = -1;
                for(c = fgetc(cod) ; c != EOF && c != 64 ; c = fgetc(cod)); // avançar para o proximo bloco
                blocos++;
                simbolo = 0;
                ant = ';';
            }
            else{
                matrizCodigos[blocos - 1][simbolo] = 0;
                while(c != 59 && c != 64){
                    matrizCodigos[blocos - 1][simbolo] = matrizCodigos[blocos -1][simbolo] * 10 + ( c - 48 );
                    c = fgetc(cod);
                }
                printf("%d ",matrizCodigos[blocos - 1][simbolo]); 
                simbolo++;
                ant = c;
            }
        }
        printMatriz(matrizCodigos, n_blocks,SIMBOLOS);  
    }
    
    fclose(cod);
}


int main(int argc , char *argv[]){
    int size = strlen(argv[1]) + 4;
    char codFile[size];
    strcpy(codFile,argv[1]);
    strcat(codFile,".cod"); 
    importCode(codFile);
    return 0;
}
 
