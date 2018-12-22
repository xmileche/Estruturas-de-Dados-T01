/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
 *
 * RA: 726573
 * Aluno: Michele Argolo Carvalho
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY    11
#define TAM_NOME         51
#define TAM_MARCA         51
#define TAM_DATA         11
#define TAM_ANO         3
#define TAM_PRECO         8
#define TAM_DESCONTO     4
#define TAM_CATEGORIA     51


#define TAM_REGISTRO     192
#define MAX_REGISTROS     1000
#define MAX_CATEGORIAS     30
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

#define F 0
#define T 1


/* Saídas para o usuario */
#define OPCAO_INVALIDA                 "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE         "Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO         "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO                 "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA            "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO                 "Arquivo vazio!\n"
#define INICIO_BUSCA                  "**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM              "**********************LISTAR**********************\n"
#define INICIO_ALTERACAO             "**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO              "**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO              "**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO    "*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO                       "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA                          "FALHA AO REALIZAR OPERACAO!\n"



/* Registro do Produto */
typedef struct {
    char pk[TAM_PRIMARY_KEY];
    char nome[TAM_NOME];
    char marca[TAM_MARCA];
    char data[TAM_DATA];    /* DD/MM/AAAA */
    char ano[TAM_ANO];
    char preco[TAM_PRECO];
    char desconto[TAM_DESCONTO];
    char categoria[TAM_CATEGORIA];
} Produto;


/*----- Registros dos Índices -----*/

/* Struct para índice Primário */
typedef struct primary_index{
    char pk[TAM_PRIMARY_KEY];
    int rrn;
} Ip;

/* Struct para índice secundário */
typedef struct secundary_index{
    char pk[TAM_PRIMARY_KEY];
    char string[TAM_NOME];
} Is;

/* Struct para índice secundário de preços */
typedef struct secundary_index_of_final_price{
    float price;
    char pk[TAM_PRIMARY_KEY];
} Isf;

/* Lista ligada para o Índice abaixo*/
typedef struct linked_list{
    char pk[TAM_PRIMARY_KEY];
    struct linked_list *prox;
} ll;

/* Struct para lista invertida */
typedef struct reverse_index{
    char cat[TAM_CATEGORIA];
    ll* lista;
} Ir;


/*----- GLOBAL -----*/
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto);

/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn);

/* Gera chave de acordo com a struct no parâmetro */
void gerarChave(Produto *j);

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int* nregistros);
void criar_iSecs(Is *inomeProd, Is *imarcas, Ir *icategorias, Isf *iprecos ,int *nregistros, int *ncategorias);

/* Realiza os scanfs na struct Produto */
void ler_entrada(char* registro, Produto *novo);

/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat);

/* Rotinas de inserção */
void insere_produto(char* registro, int *rrn, int *nregistros, int *ncategorias, Ip *iprimary, Is *iproduct, Is *imarcas, Ir *icategorias, Isf* ipreco);
void insere_arquivo(char* registro, Produto novo);
void insere_iprimary(Ip *indice_primario, int* nregistros, Produto novo, int *rrn);
void insere_icategorias(Ir *icategoria,  int* ncategorias, Produto novo);
void  insere_iSecs(Is *iproduct, Is *imarcas, Isf* ipreco, int *nregistros, Produto novo);

/* Rotinas de alteração */
int alterar(char chave[], Ip *iprimary, int nregistros, Isf *iprice);

/* Rotinas de exclusão */
int remover(char chave[] ,Ip *iprimary, int *nregistros);

/* Rotinas de busca */
void busca_codigo(char codigo[], Ip *iprimary, int nregistros);
void busca_iproduct(Ip *iprimary, Is *iproduct, int nregistros, char produto[]);
void busca_ibrand_icategory(Ip *iprimary, Is *ibrand, Ir *icategory, int nregistros, int ncat, char marca[], char categoria[]);

/* Rotinas de listagem */
void lista_codigo(Ip *iprimary, int nregistros);
void lista_categoria(Ip *iprimary, Ir *icategoria, int ncategorias, int nregistros, char categoria[]);
void lista_marca(Ip *iprimary, Is *imarcas, int nregistros);
void lista_iprice(Ip *iprimary, Isf *iprecos, int nregistros);

/* Rotinas de liberação de espaço */
void liberar_espaco(Ip *iprimary, Is *ibrand, Isf* iprice, Is *iproduct, Ir *icategory, int *ncat, int *nregistros, int *rrn);

/* Rotinas complementares de comparação */
int cmpfuncString (const void * a, const void * b);
int cmpfunc (const void * a, const void * b);
int comparaIPRIMARY(const void * a, const void * b);
int cmpIgualCodandRRN(const void * a, const void * b);
int cmpIgual (const void * a, const void * b);
int cmpIgualCategoria (const void * a, const void * b);
int cmpIgualCategoriaIr (const void * a, const void * b);
int comparaPreco (const void * a, const void * b);



/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main(){
    /* Arquivo */
    char chave[TAM_PRIMARY_KEY] = "\0";
    char categoria[TAM_CATEGORIA] = "\0";
    char produto[TAM_NOME] = "\0", marca[TAM_MARCA] = "\0";

    /*CONT*/
    int carregarArquivo = 0, nregistros = 0, ncat = 0, rrn = 0;
    scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
    if (carregarArquivo)
        nregistros = carregar_arquivo(); //nregistros é variável que atribui a qtdade de registros no arquivo

    char *registro;
    rrn = nregistros;
    /* Índice primário */
    Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
    if (!iprimary) {
        perror(MEMORIA_INSUFICIENTE);
        exit(1);
    }

    criar_iprimary(iprimary, &nregistros);

    /*Alocar e criar índices secundários*/
    /* Índice secundário para nome dos produtos */
    Is *iproduct = (Is *) malloc (MAX_REGISTROS * sizeof(Is));
    if (!iproduct) {
        perror(MEMORIA_INSUFICIENTE);
        exit(1);
    }

    /* Índice secundário para marcas */
    Is *ibrand = (Is *) malloc (MAX_REGISTROS * sizeof(Is));
    if (!ibrand) {
        perror(MEMORIA_INSUFICIENTE);
        exit(1);
    }

    /* Índice secundário para categorias */
    Ir *icategory = (Ir *) malloc (MAX_CATEGORIAS * sizeof(Ir));
    if (!icategory) {
        perror(MEMORIA_INSUFICIENTE);
        exit(1);
    }

    /* Índice secundário para preços com descontos aplicados */
    Isf *iprice = (Isf *) malloc (MAX_REGISTROS * sizeof(Isf));
    if (!iprice) {
        perror(MEMORIA_INSUFICIENTE);
        exit(1);
    }

    /*Ciação de todos os índices secundários de uma vez*/
    criar_iSecs(iproduct, ibrand, icategory, iprice, &nregistros, &ncat);

    /* Execução do programa */
    int opcao = 0, op;
    while(1)
    {
        scanf("%d%*c", &opcao);
        switch(opcao)
        {
            case 1:
                /*cadastro*/
                registro = ARQUIVO + rrn * TAM_REGISTRO;
                insere_produto(registro, &rrn, &nregistros, &ncat, iprimary, iproduct, ibrand, icategory, iprice);
                break;
            case 2:
                /*alterar desconto*/
                printf(INICIO_ALTERACAO);
                scanf("%11[^\n]%*c", chave);

                if(alterar(chave, iprimary, nregistros, iprice))
                    printf(SUCESSO);
                else
                    printf(FALHA);

                break;
            case 3:
                /*excluir produto*/
                printf(INICIO_EXCLUSAO);
                scanf("%11[^\n]%*c", chave);
                if(remover(chave, iprimary, &nregistros))
                    printf(SUCESSO);
                else
                    printf(FALHA);

                break;
            case 4:
                /*busca*/
                printf(INICIO_BUSCA );
                scanf("%d%*c", &op);

                switch(op){
                    case 1:   // Buscar por código
                        scanf("%11[^\n]%*c", chave);
                        busca_codigo(chave, iprimary, nregistros);
                        break;

                    case 2:
                        scanf("%51[^\n]%*c", produto);
                        busca_iproduct(iprimary, iproduct, nregistros, produto);
                        break;

                    case 3:
                        scanf("%51[^\n]%*c", marca);
                        scanf("%51[^\n]%*c", categoria);
                        busca_ibrand_icategory(iprimary, ibrand, icategory, nregistros, ncat, marca, categoria);
                        break;

                    default:
                        printf(OPCAO_INVALIDA);
                        break;
                }
                break;
            case 5:
                /*listagens*/
                printf(INICIO_LISTAGEM);
                scanf("%d%*c", &op);

                switch(op){
                    case 1:   // Listar por código
                        lista_codigo(iprimary, nregistros);
                        break;

                    case 2:
                        scanf("%51[^\n]%*c", categoria);
                        lista_categoria(iprimary, icategory, ncat, nregistros, categoria);
                        break;

                    case 3:
                        lista_marca(iprimary, ibrand, nregistros);
                        break;

                    case 4:
                        lista_iprice(iprimary, iprice, nregistros);
                        break;

                    default:
                        printf(OPCAO_INVALIDA);
                        break;

                }
                break;
            case 6:
                /*libera espaço*/
                liberar_espaco(iprimary, ibrand, iprice, iproduct, icategory, &ncat, &nregistros, &rrn);
                break;
            case 7:
                /*imprime o arquivo de dados*/
                printf(INICIO_ARQUIVO);
                if(nregistros == 0)
                    printf(ARQUIVO_VAZIO);
                else
                    printf("%s\n", ARQUIVO);
                break;
            case 8:
                /*imprime os índices secundários*/
                imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);

                break;
            case 9:
                /*Liberar memória e finalizar o programa */
                free(iprimary);
                free(icategory);
                free(iprice);
                free(ibrand);
                free(iproduct);
                return 0;
                break;
            default:
                printf(OPCAO_INVALIDA);
                break;
        }
    }
    return 0;
}

int cmpIgualCodandRRN(const void * a, const void * b){
    const char* str = (const char*)a;
    Ip *ordenaB = (Ip *)b;
    return(strcmp(str, ordenaB->pk));
}

int comparaIPRIMARY(const void * a, const void * b)
{
    Ip *ordenaA = (Ip *)a;
    Ip *ordenaB = (Ip *)b;
    return (strcmp(ordenaA->pk, ordenaB->pk));
}

int cmpIgual (const void * a, const void * b)
{
    const char* str = (const char*)a;
    Ip *ordenaB = (Ip *)b;
    return (strcmp(str, ordenaB->pk));
}

int cmpIgualCategoria (const void * a, const void * b)
{
    const char* str = (const char*)a;
    Ir *ordenaB = (Ir *)b;
    return(strcmp(str, ordenaB->cat));
}

int cmpIgualCategoriaIr (const void * a, const void * b)
{
    Ir *ordenaA = (Ir *)a;
    Ir *ordenaB = (Ir *)b;
    return(strcmp(ordenaA->cat, ordenaB->cat));
}

/*Funções minhas*/
void lista_iprice(Ip *iprimary, Isf *iprecos, int nregistros){
    int tentativa = 0;
    if(nregistros == 0) {
        printf(REGISTRO_N_ENCONTRADO);
        return;
    }

    for(int i=0; i<nregistros; i++) {
        Ip *busca_iprimary = bsearch(iprecos[i].pk, iprimary, nregistros, sizeof(Ip), cmpIgualCodandRRN);
        if (busca_iprimary) {
            if (busca_iprimary->rrn != -1) {
                if(tentativa)
                    printf("\n");
                exibir_registro(busca_iprimary->rrn, T);
                tentativa = 1;
            }
        }
    }
    if(!tentativa)
        printf(REGISTRO_N_ENCONTRADO);


}

void lista_marca(Ip *iprimary, Is *imarcas, int nregistros){
    int tentativa = 0;
    if(nregistros == 0) {
        printf(REGISTRO_N_ENCONTRADO);
        return;
    }

    for(int i=0; i<nregistros; i++){
        Ip *busca_iprimary = bsearch (imarcas[i].pk, iprimary, nregistros, sizeof (Ip), cmpIgualCodandRRN);
        if(busca_iprimary){
            if(busca_iprimary->rrn != -1) {
                if (tentativa)
                    printf("\n");
                exibir_registro(busca_iprimary->rrn, F);
                tentativa = 1;
            }
        }
    }
    if(!tentativa)
        printf(REGISTRO_N_ENCONTRADO);

}

void lista_categoria(Ip *iprimary, Ir *icategoria, int ncategorias, int nregistros, char categoria[]){
    Ir *busca = bsearch (categoria, icategoria, ncategorias, sizeof (Ir), cmpIgualCategoria);
    ll *cat = NULL;
    if(busca)
        cat = busca->lista;
    int tentativa = 0;
    if(busca){
        while(cat){
            Ip *busca_iprimary = bsearch (cat->pk, iprimary, nregistros, sizeof (Ip), cmpIgual);
            if(busca_iprimary){
                if(busca_iprimary->rrn != -1){
                    if(tentativa)
                        printf("\n");
                    exibir_registro(busca_iprimary->rrn, F);
                    tentativa = 1;
                }

                cat = cat->prox;

            }
        }
    }
    if(!tentativa)
        printf(REGISTRO_N_ENCONTRADO);
}

void busca_ibrand_icategory(Ip *iprimary, Is *ibrand, Ir *icategory, int nregistros, int ncat, char marca[], char categoria[]){
    int encontrou = 0;
    Ir *buscaCategoria = bsearch(categoria, icategory, ncat, sizeof(Ir), cmpIgualCategoria);
    if(!buscaCategoria){
        printf(REGISTRO_N_ENCONTRADO);
        return;
    }

    ll *cat = buscaCategoria->lista;

    // ACHO Q ISSO É MUITO CUSTOSO, TENHO QUE OLHAR DEPOIS SOCORROS
    while(cat != NULL){
        for(int i = 0; i < nregistros; i++){
            if(strcmp(ibrand[i].pk, cat->pk) == 0 && strcmp(ibrand[i].string, marca) == 0){
                Ip *busca_iprimary = bsearch (cat->pk, iprimary, nregistros, sizeof (Ip), cmpIgual);

                if(busca_iprimary->rrn != -1) {
                    if (encontrou)
                        printf("\n");
                    exibir_registro(busca_iprimary->rrn, F);
                    encontrou = 1;
                    break;
                }
            }
        }

        cat = cat->prox;
    }

    if(!encontrou)
        printf(REGISTRO_N_ENCONTRADO);


}

void lista_codigo(Ip *iprimary, int nregistros){
    int tentativa = 0;
    if(nregistros == 0) {
        printf(REGISTRO_N_ENCONTRADO);
        return;
    }

    for(int i=0; i<nregistros; i++){
        if(iprimary[i].rrn != -1){
            if(tentativa)
                printf("\n");
            exibir_registro(iprimary[i].rrn, F);
            tentativa = 1;
        }
    }

    if(!tentativa)
        printf(REGISTRO_N_ENCONTRADO);
}


int cmpIgualComp (const void * a, const void * b)
{
    const char* str = (const char*)a;
    Ip *ordenaB = (Ip *)b;
    return(strcmp(str, ordenaB->pk));
}

void busca_iproduct(Ip *iprimary, Is *iproduct, int nregistros, char produto[]){
    int encontrou = 0;
    for(int i = 0; i < nregistros; i++){
        if(strcmp(produto, iproduct[i].string) == 0){  // São iguais
            Ip *busca = bsearch (iproduct[i].pk, iprimary, nregistros, sizeof (Ip), cmpIgual);
            if(busca->rrn != -1) {
                if(encontrou)
                    printf("\n");
                exibir_registro(busca->rrn, F);
                encontrou = 1;
            }

        }
    }

    if(!encontrou)
        printf(REGISTRO_N_ENCONTRADO);
}

void busca_codigo(char codigo[], Ip *iprimary, int nregistros){
    Ip *buscaArquivo;
    buscaArquivo = bsearch (codigo, iprimary, nregistros, sizeof (Ip), cmpIgual);

    if(buscaArquivo != NULL && buscaArquivo->rrn != -1)
        exibir_registro(buscaArquivo->rrn, F);
    else
        printf(REGISTRO_N_ENCONTRADO);
}

int comparaPreco (const void * a, const void * b){
    Isf *ordenaA = (Isf *)a;
    Isf *ordenaB = (Isf *)b;
    if(ordenaA->price != ordenaB->price)
        return ( ordenaA->price > ordenaB->price ) - (ordenaA->price < ordenaB->price);
    else
        return strcmp(ordenaA->pk, ordenaB->pk);

}

void insere_produto(char* registro, int* rrn, int *nregistros, int *ncategorias, Ip *iprimary, Is *iproduct, Is *imarcas, Ir *icategorias, Isf* ipreco){
    Produto novo;
    ler_entrada(registro, &novo);  // Lê as entradas específicas e coloca as informações na strct novo

    Ip *buscaPrimary = bsearch (novo.pk, iprimary, *nregistros, sizeof (Ip), cmpIgualComp);
    // Antes de inserir nos índices confere se a chave já existe = busca binária no iprimary
    if(buscaPrimary == NULL) {
        insere_arquivo(registro, novo);     // Insere no arquivo de dados
        insere_iSecs(iproduct, imarcas, ipreco, nregistros, novo);
        insere_icategorias(icategorias, ncategorias, novo);   // Insere no índice sec categorias
        insere_iprimary(iprimary, nregistros, novo, rrn);  // Insere no índice primário

        *nregistros = *nregistros + 1;
        *rrn = *rrn + 1;
        return;

    }else if(buscaPrimary->rrn == -1){  // Então substitui no indice primario mas tbm nos secundarios
        insere_arquivo(registro, novo);
        buscaPrimary->rrn = *rrn;  // Feito no índice primario

        *rrn = *rrn + 1;

    } else {
        printf(ERRO_PK_REPETIDA, novo.pk);
    }

}

void  insere_iSecs(Is *iproduct, Is *imarcas, Isf* ipreco, int *nregistros, Produto novo){
    sprintf(iproduct[*nregistros].pk, "%s", novo.pk);
    sprintf(iproduct[*nregistros].string, "%s", novo.nome);
    sprintf(imarcas[*nregistros].pk, "%s", novo.pk);
    sprintf(imarcas[*nregistros].string, "%s", novo.marca);
    float precoFinal = (atof(novo.preco) * (100-atof(novo.desconto)))/(float)100;
    precoFinal = precoFinal * 100;
    precoFinal = ((int) precoFinal)/ (float) 100 ;

    sprintf(ipreco[*nregistros].pk, "%s", novo.pk);
    ipreco[*nregistros].price = precoFinal;

    // Ordena com a nova chave
    qsort(ipreco, (*nregistros + 1), sizeof(Isf), comparaPreco);
    // Ordena com a nova chave
    qsort(iproduct, (*nregistros + 1), sizeof(Is), cmpfuncString);
    // Ordena com a nova chave
    qsort(imarcas, (*nregistros + 1), sizeof(Is), cmpfuncString);
}

void insere_arquivo(char* registro, Produto novo){
    char str[193] = "\0";
    int i = 0;
    // Insere em str todas as informações digitadas separadas pelo delimitador "@"
    sprintf(str, "%s", novo.nome);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.marca);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.data);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.ano);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.preco);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.desconto);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.categoria);
    sprintf(str + strlen(str), "%s", "@");

    for(i =  strlen(str); i < TAM_REGISTRO; i++)
        str[i] = '#';

    str[i] = '\0';

    sprintf(registro, "%s" , str);
}
/* Lê entrada com as informação do usuário */
void ler_entrada(char* registro, Produto *novo){
    // Lê do usuários as informações de um registro

    scanf("%51[^\n]%*c", novo->nome);
    scanf("%51[^\n]%*c", novo->marca);
    scanf("%11[^\n]%*c", novo->data);
    scanf("%3[^\n]%*c", novo->ano);
    scanf("%8[^\n]%*c", novo->preco);
    scanf("%4[^\n]%*c", novo->desconto);
    scanf("%51[^\n]%*c", novo->categoria);

    //Gera chave

    novo->pk[0] = toupper(novo->nome[0]);
    novo->pk[1] = toupper(novo->nome[1]);
    novo->pk[2] = toupper(novo->marca[0]);
    novo->pk[3] = toupper(novo->marca[1]);
    novo->pk[4] = novo->data[0];
    novo->pk[5] = novo->data[1];
    novo->pk[6] = novo->data[3];
    novo->pk[7] = novo->data[4];
    novo->pk[8] = novo->ano[0];
    novo->pk[9] = novo->ano[1];
    novo->pk[10] = '\0';

}

int cmpfunc (const void * a, const void * b)
{
    Ip *ordenaA = (Ip *)a;
    Ip *ordenaB = (Ip *)b;
    return strcmp(ordenaA->pk, ordenaB->pk);
}

/* Insere o índice primário pelo novo cadastro (e não pela leitura dos nregistros) */
void insere_iprimary(Ip *indice_primario, int* nregistros, Produto novo, int *rrn){
    // Insere no arquivo de índice
    sprintf(indice_primario[*nregistros].pk, "%s" , novo.pk);
    indice_primario[*nregistros].rrn = *rrn;

    // Ordena com a nova chave
    qsort(indice_primario, (*nregistros + 1), sizeof(Ip), comparaIPRIMARY);

}


int cmpfuncString (const void * a, const void * b)
{
    Is *ordenaA = (Is *)a;
    Is *ordenaB = (Is *)b;
    int compara = strcmp(ordenaA->string, ordenaB->string);
    if(!compara)
        return strcmp(ordenaA->pk, ordenaB->pk);
    else
        return compara;
}


void insere_icategorias(Ir *icategoria,  int* ncategorias, Produto novo){
    char *cat = strtok (novo.categoria, "|");

    while(cat != NULL){
        Ir *busca = bsearch (cat, icategoria, *ncategorias, sizeof (Ir), cmpIgualComp);
        ll *novo_lista = (ll *) malloc (sizeof(ll));
        sprintf(novo_lista->pk, "%s" ,novo.pk);
        ll *anterior = NULL;
        if(!busca){        // Se não achou elemento ainda, adiciona na categoria
            novo_lista->prox = NULL;
            sprintf(icategoria[*ncategorias].cat, "%s" , cat);
            icategoria[*ncategorias].lista = novo_lista;
            *ncategorias = *ncategorias + 1;
            qsort(icategoria, *ncategorias, sizeof(Ir), cmpIgualCategoriaIr);
        } else {

            ll *atual = busca->lista;
            while(atual != NULL && strcmp(atual->pk, novo.pk) < 0) {
                anterior = atual;
                atual = atual->prox;
            }

            if(atual == NULL){  // Então quer dizer que vou inserir no fim
                anterior->prox = novo_lista;
                novo_lista->prox = NULL;

            }

            else{  // Então quer dizer que vou inserir no meio ou começo
                if(anterior == NULL){ // Vou inserir no começo
                    novo_lista->prox = atual;
                    busca->lista = novo_lista;
                } else { // Vou inserir no meio
                    novo_lista->prox = atual;
                    anterior->prox = novo_lista;
                }
            }
        }

        cat = strtok (NULL, "|");

    }

    qsort(icategoria, *ncategorias, sizeof(Ir), cmpIgualCategoriaIr);

}

void criar_iprimary(Ip *indice_primario, int* nregistros){
    Produto j;
    char *percorreArquivo = ARQUIVO;

    // Jogo no Arq de índice
    if(*nregistros){        // Se há registros no arq de dados
        for(int i = 0; i < *nregistros; i++){
            sscanf(percorreArquivo, "%[^@]@", j.pk);
            j = recuperar_registro(i);
            strcpy(indice_primario[i].pk, j.pk);
            indice_primario[i].rrn = i;
            percorreArquivo = percorreArquivo + TAM_REGISTRO;
        }
    }

    // Reordeno o Arq de índice
    // Ordena o índice primário
    qsort(indice_primario, *nregistros, sizeof(Ip), cmpfunc);

}


void criar_iSecs(Is *inomeProd, Is *imarcas, Ir *icategorias, Isf *iprecos, int *nregistros, int *ncategorias){
    Produto j;
    char *percorreArquivo = ARQUIVO;
    float precoFinal;
    // Jogo no Arq de índice
    if(*nregistros){
        for(int i = 0; i < *nregistros; i++){
            sscanf(percorreArquivo, "%[^@]@", j.pk);
            j = recuperar_registro(i);
            strcpy(inomeProd[i].pk, j.pk);
            strcpy(inomeProd[i].string, j.nome);
            strcpy(imarcas[i].pk, j.pk);
            strcpy(imarcas[i].string, j.marca);
            strcpy(iprecos[i].pk, j.pk);
            precoFinal = (atof(j.preco) * (100-atof(j.desconto)))/(float)100;
            precoFinal = precoFinal * 100;
            precoFinal = ((int) precoFinal)/ (float) 100 ;
            iprecos[i].price = precoFinal;
            insere_icategorias(icategorias, ncategorias, j);
            percorreArquivo = percorreArquivo + TAM_REGISTRO;
        }
    }

    qsort(inomeProd, *nregistros, sizeof(Is), cmpfuncString);
    qsort(imarcas, *nregistros, sizeof(Is), cmpfuncString);
    qsort(iprecos, *nregistros, sizeof(Isf), comparaPreco);

}

// FÇ que gera chave do registro
void gerarChave(Produto *j){
    j->pk[0] = toupper(j->nome[0]);
    j->pk[1] = toupper(j->nome[1]);
    j->pk[2] = toupper(j->marca[0]);
    j->pk[3] = toupper(j->marca[1]);
    j->pk[4] = j->data[0];
    j->pk[5] = j->data[1];
    j->pk[6] = j->data[3];
    j->pk[7] = j->data[4];
    j->pk[8] = j->ano[0];
    j->pk[9] = j->ano[1];
    j->pk[10] = '\0';
}

int verificaDesconto(char desconto[]){
    if((atof(desconto) > 100) || atof(desconto) < 0){
        printf(CAMPO_INVALIDO);
        return 0;
    }

    if(strlen(desconto) != 3){
        printf(CAMPO_INVALIDO);
        return 0;
    }

    return 1;
}

// A alteração foi discutida com o colega Leandro Naidhig, RA: 726555
int alterar(char chave[], Ip *iprimary, int nregistros, Isf *iprice){
    char *pArquivo;
    char desconto[TAM_DESCONTO];
    Produto recupera;
    int tamanho, tentativa = 0;
    float precoNovo;

    // Busca a chave primária digitada no arq. índice primário
    Ip *buscaArquivo = bsearch (chave, iprimary, nregistros, sizeof (Ip), cmpIgualComp);
    if(!buscaArquivo || buscaArquivo->rrn == -1){
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }

    do {
        scanf("%4[^\n]%*c", desconto);
    } while (verificaDesconto(desconto) == 0);

    pArquivo = ARQUIVO + (TAM_REGISTRO * buscaArquivo->rrn);
    recupera = recuperar_registro(buscaArquivo->rrn);

    pArquivo = pArquivo + strlen(recupera.nome) + strlen(recupera.marca) + strlen(recupera.data) + strlen(recupera.ano) + strlen(recupera.preco) + 5;
    *pArquivo = desconto[0];
    pArquivo++;
    *pArquivo = desconto[1];
    pArquivo++;
    *pArquivo = desconto[2];

    // Altera no iprice
    // Busca pela chave primária no índice de preços

    for(int i=0; i<nregistros; i++){
        if(strcmp(iprice[i].pk, chave) == 0){
            precoNovo = (atof(recupera.preco) * (100-atof(desconto)))/(float)100;
            precoNovo = precoNovo * 100;
            precoNovo = ((int) precoNovo)/ (float) 100 ;
            iprice[i].price = precoNovo;
            break;
        }
    }

    qsort(iprice, nregistros, sizeof(Isf), comparaPreco);

    return 1;
}


int remover(char chave[] ,Ip *iprimary, int *nregistros) {
    Ip *buscaArquivo;
    buscaArquivo = bsearch (chave, iprimary, *nregistros, sizeof (Ip), cmpIgualComp);

    if(!buscaArquivo || buscaArquivo->rrn == -1) {
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }

    char *p = ARQUIVO + TAM_REGISTRO * buscaArquivo->rrn;
    *p = '*';
    p = p + 1;
    *p = '|';

    buscaArquivo->rrn = -1;

    return 1;
}

/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto)
{
    if(rrn<0)
        return 0;
    float preco;
    int desconto;
    Produto j = recuperar_registro(rrn);
    char *cat, categorias[TAM_CATEGORIA];
    printf("%s\n", j.pk);
    printf("%s\n", j.nome);
    printf("%s\n", j.marca);
    printf("%s\n", j.data);
    if(!com_desconto)
    {
        printf("%s\n", j.preco);
        printf("%s\n", j.desconto);
    }
    else
    {
        sscanf(j.desconto,"%d",&desconto);
        sscanf(j.preco,"%f",&preco);
        preco = preco *  (100-desconto);
        preco = ((int) preco)/ (float) 100 ;
        printf("%07.2f\n",  preco);

    }
    strcpy(categorias, j.categoria);

    cat = strtok (categorias, "|");

    while(cat != NULL){
        printf("%s", cat);
        cat = strtok (NULL, "|");
        if(cat != NULL){
            printf(", ");
        }
    }

    printf("\n");

    return 1;
}


int carregar_arquivo()
{
    scanf("%[^\n]\n", ARQUIVO);
    return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
    char temp[193], *p;
    strncpy(temp, ARQUIVO + ((rrn)*192), 192);
    temp[192] = '\0';
    Produto j;
    p = strtok(temp,"@");
    strcpy(j.nome,p);
    p = strtok(NULL,"@");
    strcpy(j.marca,p);
    p = strtok(NULL,"@");
    strcpy(j.data,p);
    p = strtok(NULL,"@");
    strcpy(j.ano,p);
    p = strtok(NULL,"@");
    strcpy(j.preco,p);
    p = strtok(NULL,"@");
    strcpy(j.desconto,p);
    p = strtok(NULL,"@");
    strcpy(j.categoria,p);
    gerarChave(&j);
    return j;
}


/* Imprimir indices secundarios */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat){
    int opPrint = 0;
    ll *aux;
    printf(INICIO_ARQUIVO_SECUNDARIO);
    scanf("%d", &opPrint);
    if(!nregistros)
        printf(ARQUIVO_VAZIO);
    switch (opPrint) {
        case 1:
            for(int i = 0; i < nregistros; i++){
                printf("%s %s\n",iproduct[i].pk, iproduct[i].string);
            }
            break;
        case 2:
            for(int i = 0; i < nregistros; i++){
                printf("%s %s\n",ibrand[i].pk, ibrand[i].string);
            }
            break;
        case 3:
            for(int i = 0; i < ncat; i++){
                printf("%s", icategory[i].cat);
                aux =  icategory[i].lista;
                while(aux != NULL){
                    printf(" %s", aux->pk);
                    aux = aux->prox;
                }
                printf("\n");
            }
            break;

        case 4:
            for(int i = 0; i < nregistros; i++){
                printf("%s %.2f\n",iprice[i].pk, iprice[i].price);
            }
            break;
    }
}

void liberar_espaco(Ip *iprimary, Is *ibrand, Isf* iprice, Is *iproduct, Ir *icategory, int *ncat, int *nregistros, int *rrn){
    *ncat = 0;
    *nregistros = 0;
    *rrn = 0;

    // Vamos percorrer o arquivo para refazer os índices
    // Remoção é feita com *| no início do arquivo

    char *pArquivo = ARQUIVO;
    char *pArquivo_two = NULL;
    if(*pArquivo != '\0')
        pArquivo_two = ARQUIVO+1;

    char *ultimoRegistro = ARQUIVO;
    while(*pArquivo != '\0'){
        if(*pArquivo == '*' && *pArquivo_two == '|'){   // Então arquivo está removido
            ultimoRegistro = pArquivo + TAM_REGISTRO;
            sprintf(pArquivo, "%s", ultimoRegistro);
        } else {
            pArquivo = pArquivo + TAM_REGISTRO;
            ultimoRegistro = pArquivo;
            if (*pArquivo != '\0')
                pArquivo_two = pArquivo + 1;
            *nregistros = *nregistros + 1;
        }

    }

    *rrn = *nregistros;

    criar_iprimary(iprimary, nregistros);
    criar_iSecs(iproduct, ibrand, icategory, iprice, nregistros, ncat);

}
