#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BLOCK_SIZE 512 //tamanho do bloco em bytes

// Estrutura para representar um bloco de disco
typedef struct no {
    bool ocupado;
    char data[BLOCK_SIZE];
    struct no *prox; //ponteiro para o proximo bloco
} No;

No *inicio = NULL;

// Função para inicializar o disco com blocos vazios
void iniciar_disco(int num_blocos) {
    for (int i = 0; i < num_blocos; i++) {
        No *novo_bloco = (No*)malloc(sizeof(No));
        novo_bloco->ocupado = false;
        for (int j = 0; j < BLOCK_SIZE; j++) {
            novo_bloco->data[j] = '\0'; // Inicializa todos os bytes com '\0'
        }
        novo_bloco->prox = inicio;
        inicio = novo_bloco;
    }
}

// Função para inserir dados em um bloco
void inserir_dados(const char *nome, int tamanho) {
    No *atual = inicio;
    No *aux = inicio;

    //calcular o numero de blocos necessários para armazenar
    int blocoNecessario = tamanho/BLOCK_SIZE + 1;

    int blocoDisponivel = 0;
    while(aux != NULL){
        if(!aux->ocupado){
            blocoDisponivel ++;
        }
        aux = aux->prox;
    }
    free(aux);

    if(blocoDisponivel >= blocoNecessario){
        int contBlocosEncontrados = 0;
        while(atual != NULL && contBlocosEncontrados < blocoNecessario){
            if(!atual->ocupado){
                atual->ocupado = true;
                strncpy(atual->data, nome, sizeof(atual->data));
                contBlocosEncontrados ++;
            }
            atual = atual->prox;
        }
        printf("Arquivo %s armazenado com sucesso.\n", nome);
    }
    else {
        printf("Nao ha espaco suficiente para armazenar o arquivo %s.\n", nome);
    }

}

// Função para remover dados de um bloco
void remover_dados(const char* nome) {
    No *atual = inicio;
    bool encontrou = false;
    while (atual != NULL) {
        if(atual->ocupado && strcmp(atual->data, nome) == 0){
            atual->ocupado = false;
            encontrou = true;
            for(int i = 0; i < BLOCK_SIZE; i++){
                atual->data[i] = '\0'; //limpa os dados do bloco
            }
        }
        atual = atual->prox;
    }
    if(encontrou){
        printf("Arquivo %s removido com successo!\n", nome);
    }
    else {
        printf("Arquivo %s nao encontrado no disco.\n", nome);
    }
}


void visualizar_dados(){
    No *atual = inicio;
    int num_bloco = 1;
    char status[10];
    while (atual != NULL) {
        if (atual->ocupado) {
            strcpy(status, "Ocupado");
        } else {
            strcpy(status, "Livre");
        }
        printf("Bloco %d:, Dados: %s, Status: %s\n", num_bloco, atual->data, status);
        atual = atual->prox;
        num_bloco++;
    }
    printf("\n");
}

void visualizar_lista(){
    No *atual = inicio;
    int num_bloco = 1;
    char status[10];
    while (atual != NULL) {
        printf("|Bloco %d| -> ", num_bloco);
        atual = atual->prox;
        num_bloco++;
    }
    printf("NULL");
}

void liberar_memoria(){
    // Liberar a memória alocada para os blocos
    No *temp;
    while (inicio != NULL) {
        temp = inicio;
        inicio = inicio->prox;
        free(temp);
    }
}


int main() {
    int num_blocos = 10;

    //INICIALIZAR O DISCO
    printf("Inicializando Disco:\n");
    iniciar_disco(num_blocos);
    //printf("\n\n");
    //visualizar_lista();
    printf("\n\n");
    visualizar_dados();

    //INSERIR ELEMENTOS
    printf("\n\nInserindo elementos:\n");
    inserir_dados("Arquivo 1.txt", 200);
    inserir_dados("Arquivo 2.pdf", 700);
    inserir_dados("Arquivo 3.txt", 1000);
    printf("\n");
    visualizar_dados();

    printf("\n\nVisualizar no modo lista:\n");
    visualizar_lista();

    printf("\n\nRemovendo Arquivo 2.pdf:\n");
    remover_dados("Arquivo 2.pdf");
    printf("\n");
    visualizar_dados();

    printf("\n\nInserindo novo arquivo 4:\n");
    inserir_dados("Arquivo 4.txt", 1300);
    printf("\n");
    visualizar_dados();

    printf("\n\nInserindo novo arquivo 5:\n");
    inserir_dados("Arquivo 5.txt", 10000);
    printf("\n");
    visualizar_dados();

    printf("\n\nInserindo novo arquivo 6:\n");
    inserir_dados("Arquivo 6.txt", 2000);
    printf("\n");
    visualizar_dados();

    printf("\n\nInserindo novo arquivo 7:\n");
    inserir_dados("Arquivo 7.txt", 100);
    printf("\n");
    visualizar_dados();

    printf("\n\nLiberando a memoria\n");
    liberar_memoria();
    visualizar_dados();


    return 0;
}
