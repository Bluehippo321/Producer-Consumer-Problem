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
sem_t* semEmpty;
sem_t* semFull;

// variables for buffer and linking of shared memory
const int SIZE = 4096;
const char *name = "data";
int buffer[2];
int count = 0;

// shared memeory variables
int* tmpBuffer1 = -1;
int* tmpBuffer2 = -1;
int* tmpCount;

// produces ran number 0-99 and adds to buffer
void* producer(void* args){
    while(1){
        //creates num
        int x = rand() % 100;
        // waits one second for easier code reading, not necessary
        sleep(1);

        // CRITICAL SECTION
        sem_wait(semEmpty);
        // updates buffer in local to buffer in shared memory
        buffer[0] = tmpBuffer1;
        buffer[1] = tmpBuffer2;

        // updates count in local to count in shared memory
        count = tmpCount;

        // adds num to buffer and increases count
        buffer[count] = x;
        count++;

        // updates count in shared memory to count in local
        tmpCount = count;

        // updates buffer in shared memory to buffer in local
        tmpBuffer1 = buffer[0];
        tmpBuffer2 = buffer[1];
        sem_post(semFull);
        // CRITICAL SECTION
    }
}

int main(int argc, char* argv[]){
    //sets seed for random num
    srand(time(NULL));

    // creation of shared memory via mmaping
    int fd = shm_open(name, O_CREAT|O_RDWR, 0666);
    ftruncate(fd, SIZE);

    // implementation of buffer and counter into shared memory
    tmpBuffer1 = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    tmpBuffer2 = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    tmpCount = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    // implemetation of empty and full semaphores into shared memory
    semEmpty = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    semFull = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    // initilization of threads and semaphores
    pthread_t th[1];
    sem_init(semEmpty, 1, 2);
    sem_init(semFull, 1, 0);

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
    sem_destroy(semEmpty);
    sem_destroy(semFull);
    shm_unlink(name);
    return 0;
}
