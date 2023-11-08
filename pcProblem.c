#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

// edit num to increase thread count, distributed evenly
#define THREAD_NUM 2

// semaphores to determine empty and full
sem_t semEmpty;
sem_t semFull;

// redundant mutex for increased data safety
pthread_mutex_t mutexBuffer;

int buffer[2];
int count = 0;

// produces ran number 0-99 and adds to buffer
void* producer(void* args){
    while(1){
        // creates num
        int x = rand() % 100;
        // waits one second for easier code reading, not necessary
        sleep(1);

        // CRITICAL SECTION
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        buffer[count] = x;
        count++;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
        // CRITICAL SECTION
    }
}

// takes a num from the buffer and outputs it
void* consumer(void* args){
    while(1){
        // defaults to 1 to show idle time for consumer
        int y = -1;

        // CRITICAL SECTION
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        y = buffer[count-1];
        count--;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);
        // CRITICAL SECTION

        printf("Got %d\n", y);
        // waits one second for easier code reading, not necessary
        sleep(1);
    } 
}

int main(int argc, char* argv[]){
    //sets seed for random num
    srand(time(NULL));

    // initilization of threads, mutex, and semaphores
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEmpty, 0, 2);
    sem_init(&semFull, 0, 0);

    // creates and evenly distributes allocation of threads
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (i%2 == 0){    
            if (pthread_create(&th[i], NULL, &producer, NULL)){
                perror("Failed to create thread");
            }
        } else {
            if (pthread_create(&th[i], NULL, &consumer, NULL)){
                perror("Failed to create thread");
            }    
        }
    }

    // starts the threads
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

