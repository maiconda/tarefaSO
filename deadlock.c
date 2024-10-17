// growth_simulation_deadlock.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define MAX_THREADS 100
#define MAX_RECURSOS 100

// Estrutura para os dados de cada colônia
typedef struct {
    int id;            // ID da colônia
    double P0;         // População inicial
    double r;          // Taxa de crescimento
    double T;          // Tempo total
    int ordem;         // Ordem de aquisição dos recursos (0: A->B, 1: B->A)
} ColoniaArgs;

// Variáveis globais
pthread_mutex_t *recursosA;            // Array de mutexes para recursos A
pthread_mutex_t *recursosB;            // Array de mutexes para recursos B
int num_colonias;                      // Número de colônias
int num_recursos;                      // Número de recursos
ColoniaArgs *colonias;                 // Array com os dados das colônias

// Variáveis para detecção de deadlock
pthread_mutex_t mutex_estado;          // Mutex para proteger o estado
int esperando_por[MAX_THREADS];        // Qual recurso cada thread está esperando (-1 se não estiver esperando)
int dono_do_recurso[MAX_RECURSOS * 2]; // Qual thread possui cada recurso (-1 se livre)

// Função para atualizar o estado dos recursos e threads
void atualiza_estado(int thread_id, int recurso_id, bool adquirindo, bool tipoA) {
    pthread_mutex_lock(&mutex_estado);
    if (adquirindo) {
        // Se estiver adquirindo o recurso
        if (tipoA) {
            dono_do_recurso[recurso_id] = thread_id;
        } else {
            dono_do_recurso[num_recursos + recurso_id] = thread_id;
        }
        esperando_por[thread_id] = -1; // Não está mais esperando
    } else {
        // Se estiver liberando o recurso
        if (tipoA) {
            dono_do_recurso[recurso_id] = -1;
        } else {
            dono_do_recurso[num_recursos + recurso_id] = -1;
        }
    }
    pthread_mutex_unlock(&mutex_estado);
}

// Função auxiliar para detectar ciclos no grafo de espera
bool verifica_deadlock_util(int thread_id, bool visitados[], bool recursao_stack[]) {
    if (!visitados[thread_id]) {
        visitados[thread_id] = true;
        recursao_stack[thread_id] = true;

        int recurso_id = esperando_por[thread_id];
        if (recurso_id != -1) {
            int dono = dono_do_recurso[recurso_id];
            if (dono != -1) {
                if (!visitados[dono] && verifica_deadlock_util(dono, visitados, recursao_stack)) {
                    return true;
                } else if (recursao_stack[dono]) {
                    return true;
                }
            }
        }
    }
    recursao_stack[thread_id] = false;
    return false;
}

// Função que verifica se há deadlock
bool verifica_deadlock() {
    bool visitados[MAX_THREADS];
    bool recursao_stack[MAX_THREADS];
    for (int i = 0; i < num_colonias; i++) {
        visitados[i] = false;
        recursao_stack[i] = false;
    }

    for (int i = 0; i < num_colonias; i++) {
        if (verifica_deadlock_util(i, visitados, recursao_stack)) {
            return true;
        }
    }
    return false;
}

// Função da thread de cada colônia
void *colonia_func(void *args) {
    ColoniaArgs *colonia = (ColoniaArgs *)args;
    int id = colonia->id;
    double P0 = colonia->P0;
    double r = colonia->r;
    double T = colonia->T;
    int ordem = colonia->ordem;

    // Todas as colônias tentam pegar o recurso 0 de cada tipo
    int recursoA_id = 0;
    int recursoB_id = 0;

    // Loop para simular o crescimento ao longo do tempo
    for (int t = 0; t <= T; t++) {
        if (ordem == 0) {
            // Tenta pegar o Recurso A primeiro
            printf("Colônia %d tentando obter Recurso A %d\n", id, recursoA_id);

            // Atualiza que está esperando pelo Recurso A
            pthread_mutex_lock(&mutex_estado);
            esperando_por[id] = recursoA_id;
            pthread_mutex_unlock(&mutex_estado);

            pthread_mutex_lock(&recursosA[recursoA_id]); // Tenta pegar o Recurso A
            atualiza_estado(id, recursoA_id, true, true); // Atualiza que possui o Recurso A
            printf("Colônia %d obteve Recurso A %d\n", id, recursoA_id);

            sleep(1); // Simula um processamento

            // Tenta pegar o Recurso B
            printf("Colônia %d tentando obter Recurso B %d\n", id, recursoB_id);

            // Atualiza que está esperando pelo Recurso B
            pthread_mutex_lock(&mutex_estado);
            esperando_por[id] = num_recursos + recursoB_id;
            pthread_mutex_unlock(&mutex_estado);

            pthread_mutex_lock(&recursosB[recursoB_id]); // Tenta pegar o Recurso B
            atualiza_estado(id, recursoB_id, true, false); // Atualiza que possui o Recurso B
            printf("Colônia %d obteve Recurso B %d\n", id, recursoB_id);
        } else {
            // Tenta pegar o Recurso B primeiro
            printf("Colônia %d tentando obter Recurso B %d\n", id, recursoB_id);

            // Atualiza que está esperando pelo Recurso B
            pthread_mutex_lock(&mutex_estado);
            esperando_por[id] = num_recursos + recursoB_id;
            pthread_mutex_unlock(&mutex_estado);

            pthread_mutex_lock(&recursosB[recursoB_id]); // Tenta pegar o Recurso B
            atualiza_estado(id, recursoB_id, true, false); // Atualiza que possui o Recurso B
            printf("Colônia %d obteve Recurso B %d\n", id, recursoB_id);

            sleep(1); // Simula um processamento

            // Tenta pegar o Recurso A
            printf("Colônia %d tentando obter Recurso A %d\n", id, recursoA_id);

            // Atualiza que está esperando pelo Recurso A
            pthread_mutex_lock(&mutex_estado);
            esperando_por[id] = recursoA_id;
            pthread_mutex_unlock(&mutex_estado);

            pthread_mutex_lock(&recursosA[recursoA_id]); // Tenta pegar o Recurso A
            atualiza_estado(id, recursoA_id, true, true); // Atualiza que possui o Recurso A
            printf("Colônia %d obteve Recurso A %d\n", id, recursoA_id);
        }

        // Atualiza que não está esperando por nenhum recurso
        pthread_mutex_lock(&mutex_estado);
        esperando_por[id] = -1;
        pthread_mutex_unlock(&mutex_estado);

        // Calcula o crescimento populacional
        double Pt = P0 * exp(r * t);
        printf("Colônia %d: Tempo %d, População %.2f\n", id, t, Pt);

        // Libera os recursos
        if (ordem == 0) {
            atualiza_estado(id, recursoB_id, false, false); // Libera Recurso B
            pthread_mutex_unlock(&recursosB[recursoB_id]);

            atualiza_estado(id, recursoA_id, false, true); // Libera Recurso A
            pthread_mutex_unlock(&recursosA[recursoA_id]);
        } else {
            atualiza_estado(id, recursoA_id, false, true); // Libera Recurso A
            pthread_mutex_unlock(&recursosA[recursoA_id]);

            atualiza_estado(id, recursoB_id, false, false); // Libera Recurso B
            pthread_mutex_unlock(&recursosB[recursoB_id]);
        }

        sleep(1); // Espera antes da próxima etapa
    }

    pthread_exit(NULL);
}

// Função do monitor para detectar deadlock
void *monitor_func(void *args) {
    while (1) {
        sleep(5); // Intervalo de verificação

        pthread_mutex_lock(&mutex_estado);
        if (verifica_deadlock()) {
            printf("\nDeadlock detectado!\nThreads envolvidas:\n");
            for (int i = 0; i < num_colonias; i++) {
                if (esperando_por[i] != -1) {
                    printf("Colônia %d está esperando pelo recurso %d\n", i, esperando_por[i]);
                }
            }
            pthread_mutex_unlock(&mutex_estado);
            exit(1); // Encerra o programa
        }
        pthread_mutex_unlock(&mutex_estado);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Uso: %s <P0> <r> <T> <num_colonias> <num_recursos>\n", argv[0]);
        exit(1);
    }

    double P0 = atof(argv[1]);            // População inicial
    double r = atof(argv[2]);             // Taxa de crescimento
    double T = atof(argv[3]);             // Tempo total
    num_colonias = atoi(argv[4]);         // Número de colônias
    num_recursos = atoi(argv[5]);         // Número de recursos

    if (num_colonias > MAX_THREADS || num_recursos > MAX_RECURSOS) {
        printf("Número de colônias ou recursos excede o máximo permitido.\n");
        exit(1);
    }

    // Inicializa as estruturas para detecção de deadlock
    pthread_mutex_init(&mutex_estado, NULL);
    for (int i = 0; i < num_colonias; i++) {
        esperando_por[i] = -1;
    }
    for (int i = 0; i < num_recursos * 2; i++) {
        dono_do_recurso[i] = -1;
    }

    // Aloca os arrays de recursos
    recursosA = malloc(sizeof(pthread_mutex_t) * num_recursos);
    recursosB = malloc(sizeof(pthread_mutex_t) * num_recursos);

    // Inicializa os mutexes dos recursos
    for (int i = 0; i < num_recursos; i++) {
        pthread_mutex_init(&recursosA[i], NULL);
        pthread_mutex_init(&recursosB[i], NULL);
    }

    // Aloca o array das colônias
    colonias = malloc(sizeof(ColoniaArgs) * num_colonias);

    // Cria as threads das colônias
    pthread_t threads[num_colonias];
    for (int i = 0; i < num_colonias; i++) {
        colonias[i].id = i;
        colonias[i].P0 = P0;
        colonias[i].r = r;
        colonias[i].T = T;
        colonias[i].ordem = i % 2; // Alterna a ordem para causar deadlock

        pthread_create(&threads[i], NULL, colonia_func, (void *)&colonias[i]);
    }

    // Cria a thread monitor
    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, monitor_func, NULL);

    // Espera as threads terminarem
    for (int i = 0; i < num_colonias; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cancela a thread monitor
    pthread_cancel(monitor_thread);
    pthread_join(monitor_thread, NULL);

    // Destroi os mutexes
    for (int i = 0; i < num_recursos; i++) {
        pthread_mutex_destroy(&recursosA[i]);
        pthread_mutex_destroy(&recursosB[i]);
    }

    pthread_mutex_destroy(&mutex_estado);

    // Libera a memória alocada
    free(recursosA);
    free(recursosB);
    free(colonias);

    return 0;
}
