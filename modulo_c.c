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
        printf ("Tamanho antes/depois & taxa de compressão (bloco %d): %d/%d\n", i+1, tamanhoBlocos[i], tamanhoBlocosCodificado[i]);
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
            }
            tamanhoBlocos[blocos] = tamanho;
            c = fgetc(cod);

            while(c != '@'){
                if(c == ';') (matriz[blocos][simbolos]).cod = NULL;
                else{
                    (matriz[blocos][simbolos]).cod = malloc(50);
                    codigo = 0;
                    while(c != ';' && c != '@'){
                      (matriz[blocos][simbolos]).cod[codigo] = c;
                      codigo++;
                      c = fgetc(cod);
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
    printf ("tamanhoCodBits: %d\n", tamanhoCodBits);

    (*tamanhoBytes) = tamanhoCodBits/8 + 1;
    printf ("tamanhoBytes: %d\n", *tamanhoBytes);
    
    blocoCodificado = malloc(*tamanhoBytes);
    if (!blocoCodificado) printf ("Não malloc\n");

    char **bufferW = malloc((*tamanhoBytes) * sizeof(char *));
    for (int i = 0; i < *tamanhoBytes; i++)
        bufferW[i] = malloc(8 * sizeof(char));
    //char bufferW[*tamanhoBytes][8];
    int i_cod;
    index = 0;
    i_bit = 7;
    byte = 0;
    // tamanhoBloco -> tamanho do bloco do ficheiro original
    // tamanhoBytes -> tamanho do bloco codificado

    for(int i = 0; i < tamanhoBloco; i++){
        simbolo = buffer[i];
        for (i_cod = 0; (matriz[bloco][simbolo]).cod[i_cod] != '\0'; i_cod++, index++){
            if (index < 8){
                bufferW [byte][index] = (matriz[bloco][simbolo]).cod[i_cod];
            } 
            else {
                byte++;
                index = 0;
                bufferW [byte][index] = (matriz[bloco][simbolo]).cod[i_cod];
            }
        }
    }

    while(index < 8){ 
        bufferW[byte][index] = '0';
        index++;
    }

    byte = 0;
    for (i = 0; i < *tamanhoBytes; i++){
        for(i_bit = 0 ; i_bit < 8 ; i_bit++){
            if (bufferW[i][i_bit] == '1') byte = byte + pow(2,7-i_bit);
        }
        blocoCodificado[i] = byte;
        byte = 0;
    }

    for (int i = 0; i < *tamanhoBytes; i++)
        free(bufferW[i]);
    free (bufferW);

    return blocoCodificado;
}

void codificaFile(char *filename, char tipo, int n_blocks, CODFREQ (*matriz)[SIMBOLOS], int *tamanhoBlocos, int *tamanhoBlocosCodificado, char *fileShaf, float *tamanhoGlobal, float *tamanhoGlobalCodificado){
    //printf ("Entrei na CodificaFile\n");
    unsigned char *buffer,*Wbuffer;
    int tamanho = 0,tamanhoBlocoCodificado;
    FILE *file = fopen(filename,"rb");//abrir o ficheiro original ou rle 
    FILE *shaf = fopen(fileShaf,"wb");//criar o ficheiro .shaf
    for(int bloco = 0; bloco < n_blocks; bloco++){
        tamanho = tamanhoBlocos[bloco];//tamanho do bloco do ficheiro original ou .rle
        buffer = malloc(tamanho);//alocar um buffer para o tamano do bloco do ficheiro
        fread(buffer,1,tamanho,file);//preencher o buffer de bytes com a informaçao  do ficheiro
        Wbuffer = codificaBloco(buffer,matriz,tamanho,bloco,n_blocks,&tamanhoBlocoCodificado);//preencher o Wbuffer com os bytes do bloco codificado
        if (bloco == 0) fprintf(shaf,"@%d@%d@",n_blocks,tamanhoBlocoCodificado);//escrever o cabeçalho do ficheiro
        else fprintf(shaf,"@%d@",tamanhoBlocoCodificado);//escrever o tamanho do bloco no ficheiro 
        fwrite(Wbuffer,1,tamanhoBlocoCodificado,shaf);//escrever o Wbuffer no ficheiro
        free(Wbuffer);//libertar a memoria alocada
        free(buffer);//libertar memoria alocada
        tamanhoBlocosCodificado[bloco] = tamanhoBlocoCodificado; //guardar o tamanho do bloco codificado
        *tamanhoGlobal += tamanho;
        *tamanhoGlobalCodificado += tamanhoBlocoCodificado; 
    }
    printf ("%d", tamanhoBlocoCodificado);
    fclose(file);//fechar o ficheiro original
    fclose(shaf);//fechar o ficheiro .shaf
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
        //for (int j = 0; j < n_blocks; j++){
        //    printf("\n\nBloco: %d\n\n", j);
        //    for (int i = 0; i < SIMBOLOS; i++)
        //        printf (" (%s,%d,%d,%c) ", matriz[j][i].cod,matriz[j][i].freq, i, i);
        //}
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