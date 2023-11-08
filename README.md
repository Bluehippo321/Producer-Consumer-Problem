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
  had troubles implementing shared memory through given documentation
  
![image](https://github.com/Bluehippo321/Producer-Consumer-Problem/assets/149278207/318b41da-26ae-43d4-80a8-626a91d3d045)
  
  5 items are in shared memory: buffer(broken into two tmp variables), count, and full and empty semaphores

![image](https://github.com/Bluehippo321/Producer-Consumer-Problem/assets/149278207/c70548db-c880-4c6c-a9ca-f9d48dcd8c41)

  I believe everything is linked and working properly in the shared memory, but knowing how it works is the issue
  I think the buffer and count are the issue and the semaphores are working
  buffer and count are being updated manually by using a tmp variable to transfer data between shared and local memory
  process is inefficient, but should work on paper
  shared memory will update local to shared memory, then critial process will proceed, then local will update shared memory to local memory

  programs will compile, but nothing will be printed to terminal, implying there is an issue with the buffer

  can be complied with
    gcc producer.c -pthread -lrt -o producer
    gcc consumer.c -pthread -lrt -o consumer
    ./producer ./consumer


