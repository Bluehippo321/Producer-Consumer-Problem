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

// amount of consumer threads
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

// takes a num from the buffer and outputs it
void* consumer(void* args){
    while(1){
        // defaults to 1 to show idle time for consumer
        int y = -1;

        // CRITICAL SECTION
        sem_wait(semFull);
        // updates buffer in local to buffer in shared memory
        buffer[0] = tmpBuffer1;
        buffer[1] = tmpBuffer2;

        // updates count in local to count in shared memory
        count = tmpCount;

        // takes a number out of the buffer and decrements the count
        y = buffer[count-1];
        count--;

        // updates count in shared memory to count in local
        tmpCount = count;

        // updates buffer in shared memory to buffer in local
        tmpBuffer1 = buffer[0];
        tmpBuffer2 = buffer[1];
        sem_post(semEmpty);
        // CRITICAL SECTION

        // prints number got in critical section to terminal
        printf("Got %d\n", y);

        // waits one second for easier code reading, not necessary
        sleep(1);
    } 
}

int main(int argc, char* argv[]){
    //sets seed for random num
    srand(time(NULL));

    // linking of shared memory via mmaping
    char* ptr;
    int fd = shm_open(name, O_RDWR, 0666);

    // implementation of buffer and counter through shared memory
    tmpBuffer1 = (int*)mmap(NULL, 1, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    tmpBuffer2 = (int*)mmap(NULL, 1, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    tmpCount = (int*)mmap(NULL, 1, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    // implementation of full and empty semaphore into shared memory
    semEmpty = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    semFull = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // initilization of threads and semaphores
    pthread_t th[1];
    sem_init(semEmpty, 1, 2);
    sem_init(semFull, 1, 0);

    // creates threads for the consumer
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &consumer, NULL)){
            perror("Failed to create thread");
        }    
    }

    // starts threads for consumer
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


