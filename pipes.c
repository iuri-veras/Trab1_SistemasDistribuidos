#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

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

int main(){

    srand(time(0));
    int fd[2];
    pid_t pid;
    const int N = 100;

    if(pipe(fd) < 0){
        printf("Erro!\n");
        return -1;
    }

    pid = fork();
    if(pid < 0){
        printf("Erro!\n");
        return -1;
    }

    if(pid > 0){
        // Processo Produtor
        close(fd[0]);

        int value = 1;
        for(int i = 0; i < N; i++){
            value = value + (rand() % 100) + 1;
            printf("Produziu %d\n", value);
            int bytes = write(fd[1], &value, sizeof(value));
            if(bytes == -1){
                printf("Erro de escrita no pipe!\n");
            }
        }
        value = 0;
        write(fd[1], &value, sizeof(value));
        close(fd[1]);
        return 0;
    }else{
        // Processo Consumidor 
        close(fd[1]);

        int value = 0;
        int bytes;
        while((bytes = read(fd[0], &value, sizeof(value))) > 0){
            if(value == 0){
                break;
            }
            if(is_prime(value)){
                printf("%d é Primo\n", value);
            }else{
                printf("%d não é Primo\n", value);
            }
        }
        if(bytes == -1){
            perror("Erro de leitura");
        }
        close(fd[0]);
        return 0;
    }

}