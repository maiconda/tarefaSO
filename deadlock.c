// growth_simulation_deadlock.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

// Estrutura para passar parâmetros para as threads
typedef struct {
    int id;       // Identificador da colônia
    double P0;    // População inicial
    double r;     // Taxa de crescimento
    double T;     // Tempo total de simulação
    int ordem;    // Ordem de aquisição dos recursos (0: A->B, 1: B->A)
} ColoniaArgs;

// Variáveis globais
pthread_mutex_t *recursosA;
pthread_mutex_t *recursosB;
int num_colonias;
int num_recursos;

// Função da thread da colônia
void *colonia_func(void *args) {
    ColoniaArgs *colonia = (ColoniaArgs *)args;
    int id = colonia->id;
    double P0 = colonia->P0;
    double r = colonia->r;
    double T = colonia->T;
    int ordem = colonia->ordem;

    // Todas as colônias tentam obter o recurso 0 de cada tipo
    int recursoA_id = 0;
    int recursoB_id = 0;

    // Crescimento em etapas
    for (int t = 0; t <= T; t++) {
        if (ordem == 0) {
            // Tenta obter Recurso A primeiro
            printf("Colônia %d tentando obter Recurso A %d\n", id, recursoA_id);
            pthread_mutex_lock(&recursosA[recursoA_id]);
            printf("Colônia %d obteve Recurso A %d\n", id, recursoA_id);

            sleep(1); // Aumenta o tempo de retenção do recurso

            // Tenta obter Recurso B
            printf("Colônia %d tentando obter Recurso B %d\n", id, recursoB_id);
            pthread_mutex_lock(&recursosB[recursoB_id]);
            printf("Colônia %d obteve Recurso B %d\n", id, recursoB_id);
        } else {
            // Tenta obter Recurso B primeiro
            printf("Colônia %d tentando obter Recurso B %d\n", id, recursoB_id);
            pthread_mutex_lock(&recursosB[recursoB_id]);
            printf("Colônia %d obteve Recurso B %d\n", id, recursoB_id);

            sleep(1); // Aumenta o tempo de retenção do recurso

            // Tenta obter Recurso A
            printf("Colônia %d tentando obter Recurso A %d\n", id, recursoA_id);
            pthread_mutex_lock(&recursosA[recursoA_id]);
            printf("Colônia %d obteve Recurso A %d\n", id, recursoA_id);
        }

        // Calcula o crescimento populacional
        double Pt = P0 * exp(r * t);
        printf("Colônia %d: Tempo %d, População %.2f\n", id, t, Pt);

        // Libera os recursos
        pthread_mutex_unlock(&recursosB[recursoB_id]);
        pthread_mutex_unlock(&recursosA[recursoA_id]);

        // Espera antes da próxima etapa
        sleep(1);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Uso: %s <P0> <r> <T> <num_colonias> <num_recursos>\n", argv[0]);
        exit(1);
    }

    double P0 = atof(argv[1]);
    double r = atof(argv[2]);
    double T = atof(argv[3]);
    num_colonias = atoi(argv[4]);
    num_recursos = atoi(argv[5]);

    // Aloca os arrays de recursos
    recursosA = malloc(sizeof(pthread_mutex_t) * num_recursos);
    recursosB = malloc(sizeof(pthread_mutex_t) * num_recursos);

    // Inicializa os mutexes
    for (int i = 0; i < num_recursos; i++) {
        pthread_mutex_init(&recursosA[i], NULL);
        pthread_mutex_init(&recursosB[i], NULL);
    }

    // Cria as threads das colônias
    pthread_t threads[num_colonias];
    ColoniaArgs colonias[num_colonias];

    for (int i = 0; i < num_colonias; i++) {
        colonias[i].id = i;
        colonias[i].P0 = P0;
        colonias[i].r = r;
        colonias[i].T = T;
        colonias[i].ordem = i % 2; // Alterna a ordem para criar possibilidade de deadlock

        pthread_create(&threads[i], NULL, colonia_func, (void *)&colonias[i]);
    }

    // Aguarda as threads terminarem
    for (int i = 0; i < num_colonias; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroi os mutexes
    for (int i = 0; i < num_recursos; i++) {
        pthread_mutex_destroy(&recursosA[i]);
        pthread_mutex_destroy(&recursosB[i]);
    }

    // Libera a memória alocada
    free(recursosA);
    free(recursosB);

    return 0;
}
