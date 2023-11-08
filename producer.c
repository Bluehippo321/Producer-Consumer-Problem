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

// amount of producer threads
#define THREAD_NUM 1

// semaphores to determine empty and full
sem_t semEmpty;
sem_t semFull;

// redundant mutex for increased data safety
pthread_mutex_t mutexBuffer;

// variables for buffer and shared memory
const int SIZE = 4096;
const char *name = "data";
int* buffer;
int* tmpCount = 0;
int count = 0;

// produces ran number 0-99 and adds to buffer
void* producer(void* args){
    while(1){
        //creates num
        int x = rand() % 100;
        // waits one second for easier code reading, not necessary
        sleep(1);

        // CRITICAL SECTION
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        // should work with mem sharing by using pointers that get updated by producer and consumer
        count = *tmpCount;
        buffer[count] = x;
        tmpCount = count++;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
        // CRITICAL SECTION
    }
}

int main(int argc, char* argv[]){
    //sets seed for random num
    srand(time(NULL));

    // creation of shared memory via mmaping, currently only buffer and counter are implemented
    int fd = shm_open(name, O_CREAT|O_RDWR, 0666);
    ftruncate(fd, SIZE);
    buffer = (int*)mmap(0, 2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    tmpCount = (int*)mmap(0, 2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    
    // initilization of threads, mutex, and semaphores
    pthread_t th[1];
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEmpty, 0, 2);
    sem_init(&semFull, 0, 0);

    // creates threads for the producer
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &producer, NULL)){
            perror("Failed to create thread");
        }
    }

    // starts threads for the producer
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0){
            perror("Failed to join thread");
        }
    }

    // destroys all mem no longer needed
    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexBuffer);
    return 0;
}


