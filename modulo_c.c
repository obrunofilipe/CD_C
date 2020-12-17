 #include "modulo_c.h"
 #include "time.h"

int nBlocks(FILE *cod){
    char c;
    int n_blocks = 0;
    fseek(cod,1,SEEK_CUR);
    while((c = fgetc(cod)) != 64){
        n_blocks = n_blocks * 10 + (c-48);
    }
    return n_blocks;
}
void importCode(char* codFile, int n_blocks, int (*matrizCodigos)[SIMBOLOS], int *tamanhoBlocos){
    FILE *cod = fopen (codFile, "rb");
    int arrobas = 0;
    if (!cod) printf ("Error!\n");
    else{
        fseek(cod,0,SEEK_SET);
        printf("Sucess!\n");
        while(arrobas != 3){
            if(fgetc(cod) == '@') arrobas++;
        }

        printf ("Número de Blocos : %d\n", n_blocks);
        char c;
        int blocos = 0;
        int simbolos = 0;
        char ant = ';';
        int codigo,tamanho;
        
        while(blocos < n_blocks){

            simbolos = 0;
            //avancar ate ao inicio do bloco armazenando o tamanho do bloco 
            tamanho = 0;
            while((blocos == 0 || blocos == n_blocks - 1) && (c = fgetc(cod)) != '@'){
                tamanho *= 10;
                tamanho += (c-48);
                if(blocos == 0) tamanhoBlocos[0] = tamanho;
                else tamanhoBlocos[1] = tamanho;
            }
            c = fgetc(cod);

            while(c != '@'){
                if(c == ';') matrizCodigos[blocos][simbolos] = -1;
                else{
                    codigo = 0;
                    while(c != ';' && c != '@'){
                        codigo *= 10;
                        codigo += (c-48);
                        c = fgetc(cod);
                    }
                    if(c == '@') fseek(cod, -1, SEEK_CUR);
                    matrizCodigos[blocos][simbolos] = codigo;
                }
                simbolos++;
                c = fgetc(cod);
            }
            if (simbolos < 256) matrizCodigos[blocos][simbolos] = -1;
            blocos++;
        }

        for(int i = 0; i < n_blocks; i++){
            printf("simbolos: ");
            for(int j = 0; j < SIMBOLOS; j++)printf("%d, ", matrizCodigos[i][j]);
            printf("\n");
        }
    }
    fclose(cod); 
}

char tipoFicheiro(FILE *fich){
    char c;
    fseek(fich,1,SEEK_SET);
    c = fgetc(fich);
    return c;
}
//calcula os digitos de um inteiro
int nDigitos(int cod){
    int ndigitos = 0;
    if(cod == 0) ndigitos = 1; 
    while (cod != 0) {
        cod /=10;
        ndigitos++;
    }
    return ndigitos;
}

int tamanhoBlocoCod(int (*matrizCodigos)[SIMBOLOS], int (*matrizFreq)[SIMBOLOS], int bloco){
    int tamanhoCod = 0,cod;
    for(int i = 0; i < SIMBOLOS; i++){
        if((cod = matrizCodigos[bloco][i] )!= -1){
            tamanhoCod += nDigitos(cod) * matrizFreq[bloco][i];
        }
    }
    tamanhoCod = (tamanhoCod/8) + 1;
    return tamanhoCod;
}

void codificaBloco(unsigned char *buffer, int (*matrizCodigos)[SIMBOLOS], int (*matrizFreq)[SIMBOLOS] , int *tamanhoBlocos, int bloco , int n_blocks){
    unsigned char *bufferCod;
    bufferCod = malloc(tamanhoBlocoCod(matrizCodigos,matrizFreq,bloco));

}

void lerBlocos(char *filename, char tipo,int n_blocks,int (*matrizCodigos)[SIMBOLOS], int *tamanhoBlocos){
    for(int bloco = 0; bloco < n_blocks; bloco++){
    }
}

int moduloC(char *filename){
    int size = strlen(filename);
    int n_blocks;
    FILE *cod;
    char codFile[size],tipo,fileFreq[size];
    strcpy (codFile,filename); 
    strcpy(fileFreq,filename);
    strcat(codFile,".cod"); // nome do ficheito .cod
    printf("Ficheiro: %s \n",codFile);
    cod = fopen(codFile,"rb");
    tipo = tipoFicheiro(cod);
    n_blocks = nBlocks(cod);
    fclose(cod);
    int matrizCodigos[n_blocks][SIMBOLOS];
    int matrizFreq[n_blocks][SIMBOLOS];
    int tamanhoBlocos[2];
    importCode(codFile, n_blocks, matrizCodigos ,tamanhoBlocos); // importar os codigos shanonfannon
    importCode(strcat(fileFreq,".freq"),n_blocks,matrizFreq,tamanhoBlocos);// importar as frequencias



    //lerBlocos(filename,tipo,n_blocks, matrizCodigos, tamanhoBlocos);
    printf("\nTipo de ficheiro: %c",tipo);
    printf("\nTamanho Blocos: ");
    for(int i = 0; i < 2 ; i++)
        printf("%d ",tamanhoBlocos[i]);
    return 0;
    
}

int main(int argc, char *argv[]){
    clock_t begin = clock();
    moduloC(argv[1]);
    clock_t end = clock();
    double time = (double)(end - begin)/CLOCKS_PER_SEC;
    printf("\nTempo de execução: %f",time);
    return 0;
}
/*
printf("\n blocos: ");
    for(int i = 0 ; i < n_blocks ; i++){
        printf(" %d ",tamanhoBlocos[i]);
    }
*/