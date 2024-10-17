// growth_simulation_prevention.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

// Estrutura para os dados de cada colônia
typedef struct {
    int id;       // ID da colônia
    double P0;    // População inicial
    double r;     // Taxa de crescimento
    double T;     // Tempo total
} ColoniaArgs;

// Variáveis globais
pthread_mutex_t *recursosA;   // Array de mutexes para recursos A
pthread_mutex_t *recursosB;   // Array de mutexes para recursos B
int num_colonias;             // Número de colônias
int num_recursos;             // Número de recursos
ColoniaArgs *colonias;        // Array com os dados das colônias

// Função da thread de cada colônia
void *colonia_func(void *args) {
    ColoniaArgs *colonia = (ColoniaArgs *)args;
    int id = colonia->id;
    double P0 = colonia->P0;
    double r = colonia->r;
    double T = colonia->T;

    // Todas as colônias tentam pegar o recurso 0 de cada tipo
    int recursoA_id = 0;
    int recursoB_id = 0;

    // Loop para simular o crescimento ao longo do tempo
    for (int t = 0; t <= T; t++) {
        // Ordem consistente: sempre pega o Recurso A antes do Recurso B
        printf("Colônia %d tentando obter Recurso A %d\n", id, recursoA_id);
        pthread_mutex_lock(&recursosA[recursoA_id]); // Pega o Recurso A
        printf("Colônia %d obteve Recurso A %d\n", id, recursoA_id);

        sleep(1); // Simula um processamento

        printf("Colônia %d tentando obter Recurso B %d\n", id, recursoB_id);
        pthread_mutex_lock(&recursosB[recursoB_id]); // Pega o Recurso B
        printf("Colônia %d obteve Recurso B %d\n", id, recursoB_id);

        // Calcula o crescimento populacional
        double Pt = P0 * exp(r * t);
        printf("Colônia %d: Tempo %d, População %.2f\n", id, t, Pt);

        // Libera os recursos na ordem inversa
        pthread_mutex_unlock(&recursosB[recursoB_id]);
        pthread_mutex_unlock(&recursosA[recursoA_id]);

        sleep(1); // Espera antes da próxima etapa
    }

    pthread_exit(NULL);
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

        pthread_create(&threads[i], NULL, colonia_func, (void *)&colonias[i]);
    }

    // Espera as threads terminarem
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
    free(colonias);

    return 0;
}
