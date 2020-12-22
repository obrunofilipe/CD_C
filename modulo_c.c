 #include "modulo_c.h"
 #include <time.h>
 #include <math.h>

int nBlocks(FILE *cod){
    char c;
    int n_blocks = 0;
    fseek(cod,1,SEEK_CUR);
    while((c = fgetc(cod)) != 64){
        n_blocks = n_blocks * 10 + (c-48);
    }
    return n_blocks;
}
void importCode(char* codFile, int n_blocks, CODFREQ (*matriz)[SIMBOLOS], int *tamanhoBlocos){
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
                if(c == ';') (matriz[blocos][simbolos]).cod = NULL;
                else{
                    (matriz[blocos][simbolos]).cod = malloc(1);
                    codigo = 0;
                    while(c != ';' && c != '@'){
                      (matriz[blocos][simbolos]).cod[codigo] = c;
                      codigo++;
                      c = fgetc(cod);
                      (matriz[blocos][simbolos]).cod = realloc((matriz[blocos][simbolos]).cod,1);
                    }
                    (matriz[blocos][simbolos]).cod[codigo] = '\0'; 
                    if(c == '@') fseek(cod, -1, SEEK_CUR);  
                }
                simbolos++;
                c = fgetc(cod);
            }
            if (simbolos < 256) (matriz[blocos][simbolos]).cod = NULL;
            blocos++;
        }
    }
    fclose(cod); 
}
void importFreq(char* codFile,int n_blocks, CODFREQ (*matriz)[SIMBOLOS]){
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
        int freq;
        
        while(blocos < n_blocks){
            simbolos = 0;
            //avancar ate ao inicio do bloco 
            while((c = fgetc(cod)) != '@');   
            c = fgetc(cod);

            while(c != '@'){
                if(c == ';') (matriz[blocos][simbolos]).freq = 0;
                else{
                    freq = 0;
                    while(c != ';' && c != '@'){//calcula a frequencia do simbolo em int
                        freq *= 10;
                        freq += (c-48);
                        c = fgetc(cod);
                    }
                    (matriz[blocos][simbolos]).freq = freq; 
                    if(c == '@') fseek(cod, -1, SEEK_CUR);//mexe o apontador uma posiçao para trás   
                }
                simbolos++;
                c = fgetc(cod);
            }
            if (simbolos < 256) (matriz[blocos][simbolos]).freq = 0;
            blocos++;
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

int tamanhoBlocoCodBits(CODFREQ (*matriz)[SIMBOLOS], int bloco){
    int tamanhoCod = 0;
    char *cod;
    for(int simbolo = 0; simbolo < SIMBOLOS; simbolo++){
        if( (matriz[bloco][simbolo]).cod != NULL){
            tamanhoCod += strlen((matriz[bloco][simbolo]).cod) * (matriz[bloco][simbolo]).freq;
        }
    }
    return tamanhoCod;
}

int tamanhoBlocoCod(CODFREQ (*matriz)[SIMBOLOS], int bloco){
    int tamanhoCod = 0;
    char *cod;
    for(int simbolo = 0; simbolo < SIMBOLOS; simbolo++){
        if( (matriz[bloco][simbolo]).cod != NULL){
            tamanhoCod += strlen((matriz[bloco][simbolo]).cod) * (matriz[bloco][simbolo]).freq;
        }
    }
    tamanhoCod = (tamanhoCod/8) + 1;
    return tamanhoCod;
}

unsigned char * codificaBloco(unsigned char *buffer, CODFREQ (*matriz)[SIMBOLOS], int tamanhoBloco, int bloco, int n_blocks, int *tamanhoBytes){
    int tamanhoCodBits,simbolo,index, i_bit,byte;
    unsigned char *blocoCodificado;
    tamanhoCodBits = tamanhoBlocoCodBits(matriz,bloco);
    printf("\n%d", tamanhoCodBits);
    char Wbuffer[tamanhoCodBits];
    for (int j = 0; j < tamanhoCodBits; j++) Wbuffer[j] = '\0';
    printf ("\n%d", tamanhoBloco);
    printf ("\n");
    for(int i = 0; i < tamanhoBloco; i++){
        simbolo = buffer[i];
        strcat (Wbuffer, (matriz[bloco][simbolo]).cod);
    }
    //printf ("\n%s \n", Wbuffer);
    (*tamanhoBytes) = tamanhoCodBits/8 + 1;
    blocoCodificado = malloc(*tamanhoBytes);
    index = 0;
    i_bit = 7;
    byte = 0;
    for(int i = 0; Wbuffer[i] != '\0' && index < *tamanhoBytes; i++){
        if(i_bit == -1){
            i_bit = 7;
            blocoCodificado[index] = byte; 
            index++; 
            printf("%d ",byte);
            byte = 0;
        }
        if (Wbuffer[i] == '1') byte = byte + pow(2,i_bit);
        i_bit--;
    }
    if(index < (*tamanhoBytes)) blocoCodificado[index] = byte;
    return blocoCodificado; 
}

void codificaFile(char *filename, char tipo, int n_blocks, CODFREQ (*matriz)[SIMBOLOS], int *tamanhoBlocos, char *fileShaf){
    unsigned char *buffer,*Wbuffer;
    int tamanho = 0,tamanhoBlocoCodificado;
    FILE *file = fopen(filename,"rb");
    FILE *shaf = fopen(fileShaf,"wb");
    for(int bloco = 0; bloco < n_blocks; bloco++){
        if (bloco < n_blocks - 1) {
            tamanho = tamanhoBlocos[0];
            buffer = malloc(tamanho);
        }
        else {
            tamanho = tamanhoBlocos[1];
            buffer = malloc(tamanho);
        }
        fread(buffer,1,tamanho,file);
        printf("\n");
        for(int i = 0; i < tamanho ; i++)
            printf(" %d",buffer[i]);
        printf("\ndeu\n");
        Wbuffer = codificaBloco(buffer,matriz,tamanho,bloco,n_blocks,&tamanhoBlocoCodificado);
        if (bloco == 0) fprintf(shaf,"@%d@%d@",n_blocks,tamanhoBlocoCodificado);
        else fprintf(shaf,"@%d@",tamanhoBlocoCodificado);
        fwrite(Wbuffer,1,tamanhoBlocoCodificado,shaf);
        free(Wbuffer);
        free(buffer);
    }
    fclose(file);
    fclose(shaf);
}

int moduloC(char *filename){
    int size = strlen(filename);
    int n_blocks;
    FILE *cod;
    char codFile[size],tipo,fileFreq[size],fileShaf[size];
    strcpy (codFile,filename); 
    strcpy(fileFreq,filename);
    strcpy(fileShaf,filename);
    strcat(codFile,".cod"); // nome do ficheito .cod
    printf("Ficheiro: %s \n",codFile);
    cod = fopen(codFile,"rb");
    tipo = tipoFicheiro(cod);
    n_blocks = nBlocks(cod);
    fclose(cod);
    int matrizCodigos[n_blocks][SIMBOLOS];
    int matrizFreq[n_blocks][SIMBOLOS];
    CODFREQ  matriz[n_blocks][SIMBOLOS];
    int tamanhoBlocos[2];
    importCode(codFile, n_blocks, matriz ,tamanhoBlocos); // importar os codigos shanonfannon
    importFreq(strcat(fileFreq,".freq"),n_blocks,matriz);// importar as frequencias
    for(int i = 0; i < n_blocks; i++){
        printf("simbolos: ");
        for(int j = 0; j < SIMBOLOS; j++){
            if ((matriz[i][j]).cod != NULL )printf("%s(%d), ", (matriz[i][j]).cod,j);
            }
        printf("\n");
    }
    
    codificaFile(filename,tipo,n_blocks,matriz,tamanhoBlocos,strcat(fileShaf,".shaf"));
    printf("\nTipo de ficheiro: %c",tipo);
    printf("\nTamanho Blocos: ");
    for(int i = 0; i < 2 ; i++)
        printf("%d ",tamanhoBlocos[i]);
    printf("\nTamanho bloco cod:%d",tamanhoBlocoCod(matriz,1));
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