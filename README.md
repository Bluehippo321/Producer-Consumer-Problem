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

################################

consumer.c and producer.c is the multiple file implementation of the project - currently NOT working
  uses semaphores and mutex lock
  bounded buffer of 2
  uses threads
  uses mmap

  the files are the split version of pcProblems.c where each generate their own threads for the designated process
  currently NOT working
  main problem coming from implementation of memory sharing
  currently using mmap method, don't believe to be the right one, but it was the only one I could figure out
  assumedly buffer and count is working, implementation of sephamores is needed
  having troubles understanding how to implement posix shared memory through documentation

  can be complied with
    gcc producer.c -pthread -lrt -o producer
    gcc consumer.c -pthread -lrt -o consumer
    ./producer ./consumer
