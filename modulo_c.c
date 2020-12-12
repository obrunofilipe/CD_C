 #include "modulo_c.h"

int nBlocks(FILE *cod){
    char c;
    int n_blocks = 0;
    fseek(cod, 3, SEEK_SET);
    while((c = fgetc(cod)) != 64){
        n_blocks = n_blocks * 10 + (c-48);
    }
    return n_blocks;
}
void importCode(char* codFile, int n_blocks, int (*matrizCodigos)[SIMBOLOS]){
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
        int codigo;
        
        while(blocos < n_blocks){

            simbolos = 0;
            //avancar ate ao inicio do bloco
            while((c = fgetc(cod)) != '@');
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
}
char tipoFicheiro(char *filename){
    FILE *fich;
    char c;
    fich = fopen(filename,"rb");
    fseek(fich,1,SEEK_SET);
    c = fgetc(fich);
    fclose(fich);
    return c;
}


int moduloC(char *filename){
    int size = strlen(filename);
    int n_blocks;
    FILE *cod;
    char codFile[size];
    strcpy (codFile,filename);
    strcat(codFile,".cod");
    printf("Ficheiro: %s \n",codFile);
    cod = fopen(codFile,"rb");
    n_blocks = nBlocks(cod);
    fclose(cod);
    int matrizCodigos[n_blocks][SIMBOLOS];
    importCode(codFile, n_blocks,matrizCodigos);
    return 0;
}

int main(int argc, char *argv[]){
    moduloC(argv[1]);
}