#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>

#define THREAD_NUM 1

sem_t semEmpty;
sem_t semFull;

pthread_mutex_t mutexBuffer;

const int SIZE = 4096;
const char *name = "data";

int* buffer;
int* tmpCount = 0;
int count = 0;

void* producer(void* args){
    while(1){
        int x = rand() % 100;
        sleep(1);

        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        count = *tmpCount;
        buffer[count] = x;
        tmpCount = count++;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
    }
}

int main(int argc, char* argv[]){
    srand(time(NULL));

    
    int fd = shm_open(name, O_CREAT|O_RDWR, 0666);
    ftruncate(fd, SIZE);
    buffer = (int*)mmap(0, 2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    tmpCount = (int*)mmap(0, 2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    

    pthread_t th[1];
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEmpty, 0, 2);
    sem_init(&semFull, 0, 0);

    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &producer, NULL)){
            perror("Failed to create thread");
        }
    }
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0){
            perror("Failed to join thread");
        }
    }
    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexBuffer);
    return 0;
}