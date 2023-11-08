# Producer-Consumer-Problem

pcProblems.c is the one file implementation of the project - everything is working
  uses semaphores and mutex lock
  bounded buffer of 2
  uses threads

  generates random numbers 0-99 infinitely until the user stops the process, currently has a set wait time of 1 as to allow easier reading
  little to no issues on this part, fairly easy as no memory sharing was needed.
  
  can be compiled with
    gcc pcProblem.c -pthread -lrt -o pcProblem
    ./pcProblem

![image](https://github.com/Bluehippo321/Producer-Consumer-Problem/assets/149278207/fdb2e914-0b58-4b7e-ac8f-34e15e759e77)


################################

consumer.c and producer.c is the multiple file implementation of the project - currently NOT working
  uses semaphores
  bounded buffer of 2
  uses threads
  uses shm_open and mmap

  the files are the split version of pcProblems.c where each generate their own threads for the designated process
  currently NOT working
  main problem coming from implementation of memory sharing
  currently using mmap method for mem sharing
  
![image](https://github.com/Bluehippo321/Producer-Consumer-Problem/assets/149278207/318b41da-26ae-43d4-80a8-626a91d3d045)
  
  4 items are in shared memory: buffer, count, and full and empty semaphores

![image](https://github.com/Bluehippo321/Producer-Consumer-Problem/assets/149278207/58b8be07-0faa-4125-b894-200b16189a1b)

  I believe everything is linked and working properly in the shared memory, but knowing how it works is the issue
  I think the buffer and count are the issue and the semaphores should be working
  becuase the buffer's and counter's location does matter, I think that is why they aren't working
  semaphores should be working because their location doesn't
  
  had troubles implementing shared memory through given documentation
  currently programs will compile, but nothing will be printed to terminal, implying there is an issue with the buffer

  can be complied with
    gcc producer.c -pthread -lrt -o producer
    gcc consumer.c -pthread -lrt -o consumer
    ./producer ./consumer


