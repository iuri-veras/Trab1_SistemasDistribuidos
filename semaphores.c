#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define M 100000
#define N 1000

int total_produced = 0;
int total_consumed = 0;

sem_t mutex_total_produced;
sem_t mutex_total_consumed;

sem_t mutex;
sem_t empty;
sem_t full;

unsigned int seed = 1976456;

int is_prime(int x){

    if(x == 1){return 0;}
    if(x == 2){return 1;}

    for(int i = 2; i * i <= x; i++){
        if(x % i == 0){
            return 0; 
        }
    }

    return 1;
}

int config = 0;
int data[N];
int n = 0;

void *producer_function(){
    while(1){
        sem_wait(&mutex_total_produced);
        if(total_produced == M){
            sem_post(&mutex_total_produced);
            break;
        }

        sem_wait(&empty);

        sem_wait(&mutex);
        int value = 1 + (rand_r(&seed) % 10000000);
        data[n] = value;
        n += 1;
        total_produced += 1;
        sem_post(&mutex);

        sem_post(&full);
        sem_post(&mutex_total_produced);
    }
}

void *consumer_function(){
    while(1){
        sem_wait(&mutex_total_consumed);
        if(total_consumed == M){
            sem_post(&mutex_total_consumed);
            break;
        }

        sem_wait(&full);

        sem_wait(&mutex);
        n -= 1;
        int value = data[n];
        if(is_prime(value)){
            printf("%d é Primo\n", value);
        }else{
            printf("%d não é Primo\n", value);
        }
        total_consumed += 1;
        sem_post(&mutex);

        sem_post(&empty);
        sem_post(&mutex_total_consumed);
    }
}

int main(int argc, char *argv[]){

    pthread_t producers[20];
    pthread_t consumers[20];

    int Np_values[7] = {1, 1, 1, 1, 2, 4, 8};
    int Nc_values[7] = {1, 2, 4, 8, 1, 1, 1};

    // gcc semaphores.c -o semaphores.out
    // ./semaphores.out 0
    int index = atoi(argv[1]); // Muda configuração por parâmetro passado

    int Np = Np_values[index];
    int Nc = Nc_values[index];

    double average = 0;

    for(int j = 0; j < 10; j++){
        clock_t inicio, fim;
        double tempo_execucao;
        inicio = clock();

        sem_init(&mutex_total_produced, 0, 1);
        sem_init(&mutex_total_consumed, 0, 1);
        sem_init(&mutex, 0, 1);
        sem_init(&empty, 0, N);
        sem_init(&full, 0, 0);

        for(int i = 0; i < Np; i++){
            if(pthread_create(&producers[i], NULL, producer_function, NULL) != 0){
                perror("Falha ao criar thread");
            }
        }

        for(int i = 0; i < Nc; i++){
            if(pthread_create(&consumers[i], NULL, consumer_function, NULL) != 0){
                perror("Falha ao criar thread");
            }
        }

        for(int i = 0; i < Np; i++){
            pthread_join(producers[i], NULL);
        }
        for(int i = 0; i < Nc; i++){
            pthread_join(consumers[i], NULL);
        }

        fim = clock();
        tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
        average += tempo_execucao;
    }

    average /= 10;
    printf("%lf\n", average * 1000);
}
