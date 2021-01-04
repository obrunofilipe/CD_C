 #include "modulo_c.h"
 #include <time.h>
 #include <math.h>

//função que imprime tudo o que é pedido sobre o ficheiro no terminal
 void printFinal(int n_blocks, float taxaDeCompressao, double tempo, char *fileShaf, int *tamanhoBlocos, int *tamanhoBlocosCodificado){
    int i;
    time_t now = time ( NULL );
    struct tm *date = localtime (&now);
    printf ("\n\n\n\n");
    printf ("Bruno Filipe, a93298, João Delgado, a93240, MIEI/CD, %d-%d-%d\n", date->tm_mday,date->tm_mon + 1,date->tm_year + 1900);
    printf ("Módulo: C (codificação dum ficheiro de símbolos)\n");
    printf ("Número de blocos: %d\n", n_blocks);
    for (i = 0; i < n_blocks; i++) 
        printf ("Tamanho antes/depois & taxa de compressão (bloco %d): %d/%d\n", i+1, tamanhoBlocos[i], tamanhoBlocosCodificado[i]);
    printf ("Taxa de compressão global: %f\n", taxaDeCompressao);
    printf ("Tempo de execução do módulo (milissegundos): %f\n", tempo*1000);
    printf ("Ficheiro gerado: %s\n", fileShaf);
 }

// funçao que retorna o numero de blocos do ficheiro, neste momento o apontador do ficheiro .cod aponta para o segundo '@'
int nBlocks(FILE *cod){
    char c;
    int n_blocks = 0;
    fseek(cod,1,SEEK_CUR);                // avança o '@'
    while((c = fgetc(cod)) != 64){        // ciclo que armazena o número de blocos em n_blocks
        n_blocks = n_blocks * 10 + (c-48);
    }
    return n_blocks;
}

// funçao que retorna o tipo de ficheiro que vai ser codificado original/rle
char tipoFicheiro(FILE *fich){
    char c;
    fseek(fich,1,SEEK_SET);
    c = fgetc(fich);
    return c;
}

// função que calcula o tamanho em bits do bloco codificado
int tamanhoBlocoCodBits(CODFREQ (*matriz)[SIMBOLOS], int bloco){
    int tamanhoCod = 0;
    for(int simbolo = 0; simbolo < SIMBOLOS; simbolo++){
        if( (matriz[bloco][simbolo]).cod != NULL){
            tamanhoCod += strlen((matriz[bloco][simbolo]).cod) * (matriz[bloco][simbolo]).freq;
        }
    }
    return tamanhoCod;
}
// funçao que armazena os codigos associados a cada um dos simbolos numa matriz CODFREQ
void importCode(char* codFile, int n_blocks, CODFREQ (*matriz)[SIMBOLOS], int *tamanhoBlocos){
    FILE *cod = fopen (codFile, "rb");
    int arrobas = 0;
    if (!cod) printf ("Erro! Ficheiro .cod não encontrado (na importCode)\n");
    else{
        fseek(cod,0,SEEK_SET);
        while(arrobas != 3){   // avança até ao inicio dos blocos, ignorando a informação inicial (rle e numero de blocos)
            if(fgetc(cod) == '@') arrobas++;
        }
        char c;
        int blocos = 0,      // contador do número de blocos
            simbolos = 0,    // indice do simbolo na matriz a ser atualizado
            codigo,          // indice do codigo a preencher
            tamanho;         // tamanho do bloco que esta ser lido
        
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
                if(c == ';') (matriz[blocos][simbolos]).cod = NULL;    // caso não haja codigo, armazenar como null
                else{
                    (matriz[blocos][simbolos]).cod = malloc(255);      // aloca memória necessária ao código
                    codigo = 0;
                    while(c != ';' && c != '@'){                       // armazena o caracter analisado ao bit correto do codigo
                      (matriz[blocos][simbolos]).cod[codigo] = c;
                      codigo++;
                      c = fgetc(cod);
                    }
                    (matriz[blocos][simbolos]).cod[codigo] = '\0';     // adiciona o caracter nulo no final do codigo
                    if(c == '@') fseek(cod, -1, SEEK_CUR);  
                }
                simbolos++;
                c = fgetc(cod);
            }
            if (simbolos < 256) (matriz[blocos][simbolos]).cod = NULL; // caso o ultimo simbolo não tenha codigo, a variavel simbolos não será incrementada e, por isso, temos de armazenar como caso excecional
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
        while(arrobas != 3){ // avança até ao inicio dos blocos, ignorando a informação inicial (rle e numero de blocos)
            if(fgetc(cod) == '@') arrobas++;
        }
        
        int blocos = 0,   // contador do número de blocos
            simbolos = 0, // indice do simbolo na matriz a ser atualizado
            freq;         // frequencia do simbolo
        char c;           // char com que lemos ficheiro
        while(blocos < n_blocks){
            simbolos = 0; 
            while((c = fgetc(cod)) != '@'); //avancar ate ao inicio do bloco
            c = fgetc(cod);
            while(c != '@'){
                if(c == ';') (matriz[blocos][simbolos]).freq = (matriz[blocos][simbolos - 1]).freq; // se nao houver um numero entre dois ';'s tem a mesma frequência que o simbolo anterior
                else{
                    freq = 0;
                    while(c != ';' && c != '@'){ //calcula a frequencia do simbolo em int
                        freq *= 10;
                        freq += (c-48);
                        c = fgetc(cod);
                    }
                    (matriz[blocos][simbolos]).freq = freq; 
                    if(c == '@') fseek(cod, -1, SEEK_CUR); //mexe o apontador uma posiçao para trás   
                }
                simbolos++;
                c = fgetc(cod);
            }
            if (simbolos < 256) (matriz[blocos][simbolos]).freq = (matriz[blocos][simbolos - 1]).freq; // caso o ultimo simbolo não tenha frequencia, a variavel simbolos não será incrementada e, por isso, temos de armazenar como caso excecional
            blocos++;
        }
    }
    fclose(cod); 
}

unsigned char *codificaBloco(unsigned char *buffer, CODFREQ (*matriz)[SIMBOLOS], int tamanhoBloco, int bloco, int n_blocks, int *tamanhoBytes){

    int tamanhoCodBits,             // tamanho em bits do bloco depois de ser codificado com os codigos shannon fanon 
        simbolo,                    // simbolo do ficheiro original a codificar 
        index,                      // indice onde se vai introduzir o byte codificado no buffer blocoCodificado
        i,                          // variavel de controlo de ciclos
        i_bit,                      // indice de um bit de um byte 
        byte, 
        i_cod;                      // indice utilizado para percorrer os codigos shannon fanon armazenados na matriz CODFREQ matrix    
    unsigned char *blocoCodificado; // buffer que armazena o bloco todo codificado 

    tamanhoCodBits = tamanhoBlocoCodBits(matriz,bloco);                   // calculo do tamanho do bloco codificado em bits
    if(tamanhoCodBits % 8 != 0)(*tamanhoBytes) = tamanhoCodBits/8 + 1;    // caso o numero de bits do bloco nao seja multiplo de 8 adiciona se um byte 
    else (*tamanhoBytes) = tamanhoCodBits/8;                              // caso o numero de bits seja multiplo de 8 nao é necessario adicionar um byte 
    blocoCodificado = malloc(*tamanhoBytes);                              // alocar memoria para o buffer que vai armazenar o bloco codificado
    if (!blocoCodificado) printf ("Não foi possivel alocar memória\n");   // erro caso nao seja possivel alocar memória 
   
    
    index = 0;                                                            // inicializar o indice dos bytes do bloco codificado a 0
    byte = 0;                                                             // inicializar a variavel que armazena o valor do byte a 0
    i_bit = 0;                                                            // inicializar o indice utilizado para percorrer os códigos Shannon-Fano

    for (int j = 0; j < *tamanhoBytes; j++) blocoCodificado[j] = 0; // inicializar o bloco codificado a 0s

	for(i = 0; i < tamanhoBloco; i++){                              // ciclo que vai percorrer um bloco do ficheiro original e construir a sequencia de bits armazenada 
        simbolo = buffer[i];
        
        for (i_cod = 0; (matriz[bloco][simbolo]).cod[i_cod] != '\0'; i_bit++, i_cod++){  // ciclo que percorre o código Shannon-Fano armazenado na matriz
            if (i_bit < 8){                                                              
            	if ((matriz[bloco][simbolo]).cod[i_cod]  == '1') byte += pow(2,7-i_bit); // se o bit do código for igual a 1 adiciona-se o seu valor ao byte 
            }
            else {                                                                       
                blocoCodificado[index] = byte;                                           // se o byte já tiver sido escrito no total dos 8 bits armazena-se e continua-se a escrever
                index++;                                                                 // incrementa-se ao indice ao qual se vai adicionar a seguir
                i_bit = 0;                                                               // inicializar o indice dos bits a 0
                byte = 0;                                                                // começar a calcular o proximo byte
                if ((matriz[bloco][simbolo]).cod[i_cod]  == '1') byte += pow(2,7-i_bit); // adicionar o bit que já está a ser visto
            }
        }
    }
    blocoCodificado[index] = byte;                                                       // chegando ao final do ciclo e o número de bits calculados do ultimo byte não for 8 é preciso atribuir ao indice o byte calculado como caso excecional
    return blocoCodificado;
}

void codificaFile(char *filename, char tipo, int n_blocks, CODFREQ (*matriz)[SIMBOLOS], int *tamanhoBlocos, int *tamanhoBlocosCodificado, char *fileShaf, float *tamanhoGlobal, float *tamanhoGlobalCodificado){
    unsigned char *buffer,*Wbuffer;
    int tamanho = 0,tamanhoBlocoCodificado;
    FILE *file = fopen(filename,"rb");                                                         // abrir o ficheiro original ou rle 
    FILE *shaf = fopen(fileShaf,"wb");                                                         // criar o ficheiro .shaf
    for(int bloco = 0; bloco < n_blocks; bloco++){
        tamanho = tamanhoBlocos[bloco];                                                        // tamanho do bloco do ficheiro original ou .rle
        buffer = malloc(tamanho);                                                              // alocar um buffer para o tamano do bloco do ficheiro
        fread(buffer,1,tamanho,file);                                                          // preencher o buffer de bytes com a informaçao  do ficheiro
        Wbuffer = codificaBloco(buffer,matriz,tamanho,bloco,n_blocks,&tamanhoBlocoCodificado); // preencher o Wbuffer com os bytes do bloco codificado
    if (bloco == 0) fprintf(shaf,"@%d@%d@",n_blocks,tamanhoBlocoCodificado);                   // escrever o cabeçalho do ficheiro
        else fprintf(shaf,"@%d@",tamanhoBlocoCodificado);                                      // escrever o tamanho do bloco no ficheiro 
        fwrite(Wbuffer,1,tamanhoBlocoCodificado,shaf);                                         // escrever o Wbuffer no ficheiro
        free(Wbuffer);                                                                         // libertar a memoria alocada
        free(buffer);                                                                          // libertar memoria alocada
        tamanhoBlocosCodificado[bloco] = tamanhoBlocoCodificado;                               // guardar o tamanho do bloco codificado
        *tamanhoGlobal += tamanho;
        *tamanhoGlobalCodificado += tamanhoBlocoCodificado; 
    }
    fclose(file);                                                                              // fechar o ficheiro original
    fclose(shaf);                                                                              // fechar o ficheiro .shaf
}

void libertaMatriz (CODFREQ (*matriz)[SIMBOLOS], int n_blocks){ //funçao que liberta a memória alocada na matriz
    for (int i = 0; i < n_blocks; i++){
        for (int j = 0; j < SIMBOLOS; j++){
            if ((matriz[i][j]).cod != NULL) free ((matriz[i][j]).cod);
        }
    }
}


int moduloC(char *filename){
    clock_t begin = clock();
    int size = strlen(filename)+10;    // tamanho do da string necessaria para armazenar o nome dos ficheiros .cod e .freq
    int n_blocks;                      // número de blocos do ficheiro original 
    float tamanhoGlobal = 0,           // tamanho total do ficheiro original
          tamanhoGlobalCodificado = 0, // tamanho total do ficheiro codificado
          taxaDeCompressao;            // taxa de compressao
    FILE *cod;                         // apontador para o ficheiro a abrir (.cod)
    char codFile[size],                // nome do ficheiro .cod
         tipo,                         // tipo do ficheiro original/rle
         fileFreq[size],               // nome do ficheiro .freq
         fileShaf[size];               // nome do ficheiro .shaf
    strcpy(codFile,filename);          // copiar o nome do ficheiro para a string do ficheiro .cod
    strcpy(fileFreq,filename);         // copiar o nome do ficheiro para a string do ficheiro .freq 
    strcpy(fileShaf,filename);         // copiar o nome do ficheiro para a string do ficheiro .shaf
    strcat(codFile,".cod");            // adicionar ao nome do ficheiro a extensao .cod
    strcat(fileFreq,".freq");          // adicionar ao nome do ficheiro a extensao .freq
    cod = fopen(codFile,"rb");         // abrir o ficheiro .cod
    if(!cod) printf("Erro ficheiro .cod nao encontrado!\n");             // erro caso o ficheiro nao exista, a codificaçao nao é executada
    else{
        tipo = tipoFicheiro(cod);                                        // identifica o tipo de ficheiro (rle ou normal)
        n_blocks = nBlocks(cod);                                         // calcular o numero de blocos do ficheiro
        fclose(cod);                                                     // fechar o ficheiro .cod
        CODFREQ matriz[n_blocks][SIMBOLOS];                              // matriz que vai armazenar todos os codigos Shannon Fano lidos do ficheiro .cod dos respetivos simbolos e as suas frequencias frequencias
        int tamanhoBlocos[n_blocks],tamanhoBlocosCodificado[n_blocks];
        importCode(codFile, n_blocks, matriz ,tamanhoBlocos);            // importar os codigos Shanon-Fano
        importFreq(fileFreq,n_blocks,matriz);                            // importar as frequencias
        codificaFile(filename,tipo,n_blocks,matriz,tamanhoBlocos, tamanhoBlocosCodificado, strcat(fileShaf,".shaf"), &tamanhoGlobal, &tamanhoGlobalCodificado);
        libertaMatriz(matriz, n_blocks);                                 // liberta o espaço alocado à matriz de codigos e frequencias
        taxaDeCompressao = tamanhoGlobalCodificado/tamanhoGlobal;        // taxa de compressão do ficheiro -> tamanho que tem apos ser codificado a dividir pelo tamanho que tem agora
        clock_t end = clock();                                           
        double tempo = (double)(end - begin)/CLOCKS_PER_SEC;              // calcula o tempo de execução do módulo
        printFinal(n_blocks, taxaDeCompressao, tempo, fileShaf, tamanhoBlocos, tamanhoBlocosCodificado); // imprimir as mensagens finais
    }
    return 0;
}

int main(int argc, char *argv[]){
    moduloC(argv[1]);
    return 0;
}
