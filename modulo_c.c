 #include "modulo_c.h"
 #include <time.h>
 #include <math.h>

 void printCenas(int n_blocks, float taxaDeCompressao, double time, char *fileShaf, int *tamanhoBlocos, int *tamanhoBlocosCodificado){
    int i;
    printf ("\n\n\n\n");
    printf ("Bruno Filipe, a93298, João Delgado, a93240, MIEI/CD, 1-jan-2021 \n");
    printf ("Módulo: C (codificação dum ficheiro de símbolos)\n");
    printf ("Número de blocos: %d\n", n_blocks);
    for (i = 0; i < n_blocks - 1; i++) 
        printf ("Tamanho antes/depois & taxa de compressão (bloco %d): %d/%d\n", i+1, tamanhoBlocos[0], tamanhoBlocosCodificado[i]);
    printf ("Tamanho antes/depois & taxa de compressão (bloco %d): %d/%d\n", i+1, tamanhoBlocos[1], tamanhoBlocosCodificado[i]);
    printf ("Taxa de compressão global: %f\n", taxaDeCompressao); //print para o terminal
    printf ("Tempo de execução do módulo (milissegundos): %f\n", time*1000); //print para o terminal 
    printf ("%s\n", fileShaf);
 }

int nBlocks(FILE *cod){
    char c;
    int n_blocks = 0;
    fseek(cod,1,SEEK_CUR);
    while((c = fgetc(cod)) != 64){
        n_blocks = n_blocks * 10 + (c-48);
    }
    return n_blocks;
}

char tipoFicheiro(FILE *fich){
    char c;
    fseek(fich,1,SEEK_SET);
    c = fgetc(fich);
    return c;
}

int tamanhoBlocoCodBits(CODFREQ (*matriz)[SIMBOLOS], int bloco){
    int tamanhoCod = 0;
    for(int simbolo = 0; simbolo < SIMBOLOS; simbolo++){
        if( (matriz[bloco][simbolo]).cod != NULL){
            tamanhoCod += strlen((matriz[bloco][simbolo]).cod) * (matriz[bloco][simbolo]).freq;
        }
    }
    printf("Calculei o tamanho: %d\n", tamanhoCod);
    return tamanhoCod;
}

int tamanhoBlocoCod(CODFREQ (*matriz)[SIMBOLOS], int bloco){
    int tamanhoCod = 0;
    for(int simbolo = 0; simbolo < SIMBOLOS; simbolo++){
        if( (matriz[bloco][simbolo]).cod != NULL){
            tamanhoCod += strlen((matriz[bloco][simbolo]).cod) * (matriz[bloco][simbolo]).freq;
        }
    }
    tamanhoCod = (tamanhoCod/8) + 1;
    return tamanhoCod;
}

void importCode(char* codFile, int n_blocks, CODFREQ (*matriz)[SIMBOLOS], int *tamanhoBlocos){
    FILE *cod = fopen (codFile, "rb");
    int arrobas = 0;
    if (!cod) printf ("Erro! Ficheiro .cod não encontrado (na importCode)\n");
    else{
        fseek(cod,0,SEEK_SET);
        while(arrobas != 3){
            if(fgetc(cod) == '@') arrobas++;
        }
        char c;
        int blocos = 0;
        int simbolos = 0;
        int codigo,tamanho;
        
        while(blocos < n_blocks){

            simbolos = 0;
            //avancar ate ao inicio do bloco armazenando o tamanho do bloco
            tamanho = 0;
            while((c = fgetc(cod)) != '@'){
                tamanho *= 10;
                tamanho += (c-48);
                tamanhoBlocos[blocos] = tamanho;
            }
            c = fgetc(cod);
            printf ("  %c  ", c);

            while(c != '@'){
                if(c == ';') (matriz[blocos][simbolos]).cod = NULL;
                else{
                    (matriz[blocos][simbolos]).cod = malloc(50);
                    codigo = 0;
                    while(c != ';' && c != '@'){
                      (matriz[blocos][simbolos]).cod[codigo] = c;
                      codigo++;
                      c = fgetc(cod);
                      //(matriz[blocos][simbolos]).cod = realloc((matriz[blocos][simbolos]).cod,1);
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
    if (!cod) printf ("Erro! Ficheiro .freq não encontrado\n");
    else{
        fseek(cod,0,SEEK_SET);
        while(arrobas != 3){
            if(fgetc(cod) == '@') arrobas++;
        }
        
        int blocos = 0;
        int simbolos = 0;
        int freq;
        char c;
        while(blocos < n_blocks){
            simbolos = 0;
            //avancar ate ao inicio do bloco 
            while((c = fgetc(cod)) != '@');
            c = fgetc(cod);
            while(c != '@'){
                if(c == ';') (matriz[blocos][simbolos]).freq = (matriz[blocos][simbolos - 1]).freq;
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

unsigned char *codificaBloco(unsigned char *buffer, CODFREQ (*matriz)[SIMBOLOS], int tamanhoBloco, int bloco, int n_blocks, int *tamanhoBytes){
    printf ("Entrei na codificaBloco: bloco %d\n", bloco);
    //printf ("Hello\n");
    int tamanhoCodBits,simbolo,index, i, i_bit,byte;
    unsigned char *blocoCodificado;
    tamanhoCodBits = tamanhoBlocoCodBits(matriz,bloco);
    printf ("tamanhoCodBits: %d", tamanhoCodBits);
    printf ("Hello\n");
    char Wbuffer[tamanhoCodBits];
    printf ("Hello\n");
    for (int j = 0; j < tamanhoCodBits; j++) Wbuffer[j] = '\0';
    for(int i = 0; i < tamanhoBloco; i++){
        simbolo = buffer[i];
        strcat (Wbuffer, (matriz[bloco][simbolo]).cod);
        //free((matriz[bloco][simbolo]).cod);
    }
    (*tamanhoBytes) = tamanhoCodBits/8 + 1;
    printf ("tamanhoBytes: %d\n", *tamanhoBytes);
    //printf ("Hello\n");
    blocoCodificado = malloc(*tamanhoBytes);
    index = 0;
    i_bit = 7;
    byte = 0;
    for(i = 0; Wbuffer[i] != '\0' && index < *tamanhoBytes; i++){
        if(i_bit == -1){
            i_bit = 7;
            blocoCodificado[index] = byte; 
            index++;
            byte = 0;
        }
        if (Wbuffer[i] == '1') byte = byte + pow(2,i_bit);
        i_bit--;
    }
    if(index < (*tamanhoBytes)) blocoCodificado[index] = byte;
    return blocoCodificado; 
}

void codificaFile(char *filename, char tipo, int n_blocks, CODFREQ (*matriz)[SIMBOLOS], int *tamanhoBlocos, int *tamanhoBlocosCodificado, char *fileShaf, float *tamanhoGlobal, float *tamanhoGlobalCodificado){
    //printf ("Entrei na CodificaFile\n");
    unsigned char *buffer,*Wbuffer;
    int tamanho = 0,tamanhoBlocoCodificado;
    FILE *file = fopen(filename,"rb");
    FILE *shaf = fopen(fileShaf,"wb");
    for(int bloco = 0; bloco < n_blocks; bloco++){
        tamanho = tamanhoBlocos[bloco];
        buffer = malloc(tamanho);
        fread(buffer,1,tamanho,file);
        Wbuffer = codificaBloco(buffer,matriz,tamanho,bloco,n_blocks,&tamanhoBlocoCodificado);
        if (bloco == 0) fprintf(shaf,"@%d@%d@",n_blocks,tamanhoBlocoCodificado);
        else fprintf(shaf,"@%d@",tamanhoBlocoCodificado);
        fwrite(Wbuffer,1,tamanhoBlocoCodificado,shaf);
        free(Wbuffer);
        free(buffer);
        tamanhoBlocosCodificado[bloco] = tamanhoBlocoCodificado;
        *tamanhoGlobal += tamanho;
        *tamanhoGlobalCodificado += tamanhoBlocoCodificado;
    }
    fclose(file);
    fclose(shaf);
}



int moduloC(char *filename, int begin){
    int size = strlen(filename)+10;
    int n_blocks;
    float tamanhoGlobal = 0, tamanhoGlobalCodificado = 0, taxaDeCompressao;
    FILE *cod;
    char codFile[size],tipo,fileFreq[size],fileShaf[size];
    strcpy(codFile,filename);
    strcpy(fileFreq,filename);
    strcpy(fileShaf,filename);
    strcat(codFile,".cod"); // nome do ficheito .cod
    strcat(fileFreq,".freq");
    cod = fopen(codFile,"rb");
    if(!cod) printf("Erro ficheiro .cod nao encontrado!\n");
    else{
        tipo = tipoFicheiro(cod);
        n_blocks = nBlocks(cod);
        fclose(cod);
        //printf ("%d\n", n_blocks);
        //printf ("%c\n", tipo);
        CODFREQ matriz[n_blocks][SIMBOLOS];
        int tamanhoBlocos[n_blocks],tamanhoBlocosCodificado[n_blocks];
        //printf("%s\n", codFile);
        //printf("%s\n", fileFreq);
        importCode(codFile, n_blocks, matriz ,tamanhoBlocos); // importar os codigos shanonfannon
        //printf ("Passei pela importFreq");
        importFreq(fileFreq,n_blocks,matriz);// importar as frequencias
        for (int j = 0; j < n_blocks; j++){
            printf("\n\nBloco: %d\n\n", j);
            for (int i = 0; i < SIMBOLOS; i++)
                printf (" (%s,%d,%d,%c) ", matriz[j][i].cod,matriz[j][i].freq, i, i);
        }
        //printf ("Passei pela importFreq");
        
        codificaFile(filename,tipo,n_blocks,matriz,tamanhoBlocos, tamanhoBlocosCodificado, strcat(fileShaf,".shaf"), &tamanhoGlobal, &tamanhoGlobalCodificado);
        taxaDeCompressao = tamanhoGlobalCodificado/tamanhoGlobal;
        clock_t end = clock();
        double time = (double)(end - begin)/CLOCKS_PER_SEC;
        printCenas(n_blocks, taxaDeCompressao, time, fileShaf, tamanhoBlocos, tamanhoBlocosCodificado);
    }

    return 0;
}

int main(int argc, char *argv[]){
    clock_t begin = clock();
    moduloC(argv[1], begin);
    return 0;
}