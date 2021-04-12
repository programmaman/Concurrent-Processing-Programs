//Alec Braynen
#define _REENTRANT
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <unistd.h>

struct sharedData
{
  int value; /* shared variable to store result*/
};
struct sharedData *counter;

atomic_bool writerFlag = 0; /*Atomic Boolean Flag for debugging and confirming proper critical section behavior*/

sem_t mutex, synch, writeSem; /* Mutual Exclusion semaphore for modifying the shared readerCounter variable, synch semaphore for process queue synchronization, writeSem for writing synchronization*/
int readerCounter = 0; /*Number of Active Readers, may remove and try to do an implementation with counting semaphores if I get the time and figure out that solution */

void readCounter(); /*Function that reads*/

void *writerThread(void *arg)
{
  sem_wait(&synch); //Queue synchronization, blocking wait
  sem_wait(&writeSem); //Critical section blocking wait
  while ((counter->value) < 25000)
  {
    /* Critical Section */
    writerFlag = 1;
    counter->value += 1;
    writerFlag = 0;
  }
  sem_post(&writeSem);
  sem_post(&synch);
  printf("Writer done. Wrote critical section: %d times.\n", counter->value);
  return (NULL);
}

void *readerThread(void *arg)
{
  int readCount = 0;
  int i = (intptr_t)arg;

  sem_wait(&synch);
  sem_wait(&mutex); //Mutual Exclusion for updating # of counters//
  readerCounter++;
  if (readerCounter == 1)
    sem_wait(&writeSem); // Writer won't preempt readers
  sem_post(&mutex);
  sem_post(&synch);
  while (readCount < 250000000)
  {
    if (writerFlag == 1)
    {
      fprintf(stderr, "%s", "Writer accessed critical section while reading.\n");
    }
    readCount++;
  }
  sem_wait(&mutex);
  readerCounter--;
  if (readerCounter == 0)
    sem_post(&writeSem); //Writer allowed to write. There are no active readers
  sem_post(&mutex);

  printf("Reader #%d done. Read critical section: %d times.\n", i, readCount);
  return (NULL);
}

int main(int argc, char **argv)
{

  int numOfReaders = atoi(argv[1]);

  pthread_t readers[numOfReaders];
  pthread_t writer;
  pthread_attr_t attr[1];

  sem_init(&mutex, 0, 1);
  sem_init(&synch, 0, 1);
  sem_init(&writeSem, 0, 1);

  counter = (struct sharedData *)malloc(sizeof(struct sharedData));
  counter->value = 0;

  int k = numOfReaders / 2;

  pthread_attr_init(&attr[0]);
  pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM); /* system-wide contention */ /*Using the OS's built in scheduler for this project. All threads get an equal chance in this code but OS Scheduler might have a nonintuitive scheduling algorithm*/

  /*Create first half of reader threads*/
  for (int i = 0; i < k; i++)
  {
    printf("Reader thread #%d created. \n", i);
    pthread_create(&readers[i], &attr[0], readerThread, (void *)(intptr_t)i);
  }

  /*Create Writer Thread*/
  printf("Writer thread created.\n");
  pthread_create(&writer, &attr[0], writerThread, NULL);

  /*Create second half of reader threads*/
  for (int i = k; i < numOfReaders; i++)
  {
    printf("Reader thread #%d created. \n", i);
    pthread_create(&readers[i], &attr[0], readerThread, (void *)(intptr_t)i);
  }

  for (int i = 0; i < numOfReaders; i++)
  {
    pthread_join(readers[i], NULL);
  }

  pthread_join(writer, NULL);

  return 0;
}

void readCounter()
{
  if ((counter->value))
  {
    //Read counter value
  }
}
