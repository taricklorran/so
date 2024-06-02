#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> //biblioteca para conseguir criar as threads
#include <stdbool.h>

#include <semaphore.h>

#define num_cadeiras 6
#define num_clientes 30

//Tárick Lorran Batista Leite ---- 12021BSI227
//Izabela Pereira Maestri -------- 12021BSI239


sem_t cliente_s; //cliente em espera de atendimento
sem_t barbeiro_s; //barbeiro em espera do cliente
sem_t mutex; //para excluso mutua

volatile int inicio = 0, fim = 0, fila[num_cadeiras];
volatile int num_cliente_aguardando = 0;
volatile bool barbeiro_dormindo = false;

/**
Mensagens
**/

void acorda_barbeiro(int cliente){
    printf("Cliente %d acorda o barbeiro.\n", cliente);
    barbeiro_dormindo = false;
}

void cadeira_espera(int cliente){
    printf("Cliente %d senta em uma das cadeiras de espera.\n", cliente);
}

void cadeiras_ocupadas(int cliente){
    printf("Todas as cadeiras ocupadas. Cliente %d vai embora.\n", cliente);
}

void atendimento_barbeiro(int cliente){
    printf("Cliente %d senta na cadeira do barbeiro.\n", cliente);
    printf("Barbeiro fazendo a barba do cliente %d.\n", cliente);
    //sleep(random() % 4);
    printf("Barbeiro finalizar o serviço do cliente %d.\n\n", cliente);
}

void barbeiro_dorme(){
    printf("Não ha cliente para atendimento. Barbeiro irá dormir.\n\n");
    barbeiro_dormindo = true;
}

void cliente_prox_atendimento(int cliente){
    printf("Cliente %d vai direto para a cadeira do barbeiro.\n", cliente);
}


/**
CONCORRENCIA SEM SINCRONIZAÇÃO
**/


void cliente_chega_sem_sinc(int cliente){

    if(num_cliente_aguardando < num_cadeiras){
        if(num_cliente_aguardando == 0 && barbeiro_dormindo == true){
            acorda_barbeiro(cliente);
        }else {
            if(num_cliente_aguardando == 0 && barbeiro_dormindo == false){
                cliente_prox_atendimento(cliente);
            }
            else{
                cadeira_espera(cliente);
            }
        }
        fila[fim] = cliente; //posicao que se será inserido na fila adiciona o cliente na posição atual e depois aponta o end para próxima posição da fila. Quando chega na posição máxima aponta para o inicio novamente (fila circular).
        fim = (fim + 1) % num_cadeiras;
        num_cliente_aguardando += 1; //controle de quantidade de cliente

    }
    else{
        cadeiras_ocupadas(cliente);
    }
}

void *fbarbeiro_sem_sinc(void *arg){

    while(true){
        if(num_cliente_aguardando > 0){

            atendimento_barbeiro(fila[inicio]);
            inicio = (inicio + 1) % num_cadeiras; //aponta para o próximo da fila
            num_cliente_aguardando -= 1;

        }
        else {
            barbeiro_dorme();
            while(num_cliente_aguardando == 0 && barbeiro_dormindo == true){
                    //Retorna a execução quando entrar cliente.
            }
        }
    }
}

void *fcliente_sem_sinc(void *arg){
    int *p_id = (int *) arg;
    cliente_chega_sem_sinc(*p_id);
}


/**
CONCORRENCIA COM SINCRONIZAÇÃO
**/

void cliente_chega_com_sinc(int cliente){

    sem_wait(&mutex); //Entra na região crítica
    sem_wait(&barbeiro_s); //barbeiro dorme

    if(num_cliente_aguardando < num_cadeiras){
        if(num_cliente_aguardando == 0 && barbeiro_dormindo == true){
            acorda_barbeiro(cliente);
        }
        else
            //Caso que ocorre quando o barbeiro finaliza o ultimo cliente, porém, não ocorreu ainda a atualização da variável para dormir
            if(num_cliente_aguardando == 0 && barbeiro_dormindo == false){
                cliente_prox_atendimento(cliente);
            }
            else {
                //Lista de espera
                cadeira_espera(cliente);
            }

        fila[fim] = cliente; //posicao que se será inserido na fila adiciona o cliente na posição atual e depois aponta o end para próxima posição da fila. Quando chega na posição máxima aponta para o inicio novamente (fila circular).
        fim = (fim + 1) % num_cadeiras;
        num_cliente_aguardando += 1; //controle de quantidade de cliente

        sem_post(&barbeiro_s); //acorda o barbeiro
        sem_post(&mutex); //sai da região crítica

    }
    else{
        cadeiras_ocupadas(cliente);
        sem_post(&barbeiro_s);// acorda barbeiro
        sem_post(&mutex); //Sai da região crítica

    }
    pthread_exit(NULL);
}

void *fbarbeiro_com_sinc(void *arg){


    while(true){


        sem_wait(&mutex); //entra na regiao critica
        sem_wait(&cliente_s); //cliente dorme

        if(num_cliente_aguardando > 0){

            atendimento_barbeiro(fila[inicio]);

            inicio = (inicio + 1) % num_cadeiras; //aponta para o próximo da fila
            num_cliente_aguardando -= 1;

            sem_post(&cliente_s); //acorda cliente
            sem_post(&mutex); //sai da regiao critica

        }
        else {

            barbeiro_dorme();
            sem_post(&cliente_s); // acorda cliente
            sem_post(&mutex); //sai da região crítica
            while(num_cliente_aguardando == 0 && barbeiro_dormindo == true){
                //Retorna a execução quando entrar cliente.
            }
        }
    }
    pthread_exit(NULL);
}

void *fcliente_com_sinc(void *arg){
    int *p_id = (int *) arg;
    cliente_chega_com_sinc(*p_id);
    pthread_exit(NULL);
}



int main()
{
    pthread_t cliente[num_clientes]; //inicia uma lista de threads

    pthread_t barbeiro;

    int op;
    int cadeiras;
    cadeiras = num_cadeiras - 1;

    printf("########## Barbearia do João ##########\n\n");
    printf("Quantidade total de cadeiras: %d, sendo 1 do barbeiro e %d para clientes.\n", num_cadeiras, cadeiras);
    printf("Quantidade de clientes: %d.\n\n", num_clientes);

    printf("Escolha uma das opções:\n1 - Concorrência sem sincronização\n2 - Concorrência com sincronização\n");
    scanf("%d", &op);

    while(op != 1 && op != 2){
        printf("Opção invalida!\n\n");
        printf("Escolha uma das opções:\n1 - Concorrência sem sincronização\n2 - Concorrência com sincronização\n");
        scanf("%d", &op);
    }

    if(op == 1){

        pthread_create(&barbeiro, NULL, fbarbeiro_sem_sinc, NULL);

        int i = 1;
        for(i = 1; i <= num_clientes; i++){
            pthread_create(&cliente[i], NULL, fcliente_sem_sinc, (void*) &i);
            //sleep(random() % 2); //Simula a chegada de clientes
        }

        i = 1;

        for(i = 1; i <= num_clientes; i++){
            pthread_join(cliente[i], NULL);
        }

        pthread_join(barbeiro, NULL);

    }
    else{
        //inicialização dos semáforos: especifica o semáforo a ser iniciado, se é ou não compartilhado, o valor do atributo do semáforo recem inicializada
        sem_init(&cliente_s, 0, 1);
        sem_init(&barbeiro_s, 0, 1);
        sem_init(&mutex, 1, 1);

        pthread_create(&barbeiro, NULL, fbarbeiro_com_sinc, NULL);

        int i = 1;
        for(i = 1; i <= num_clientes; i++){
            pthread_create(&cliente[i], NULL, fcliente_com_sinc, (void*) &i);
            //sleep(random() % 2); //Simula a chegada de clientes
        }

        i = 1;

        for(i = 1; i <= num_clientes; i++){
            pthread_join(cliente[i], NULL);
        }

        pthread_join(barbeiro, NULL);
    }

}
