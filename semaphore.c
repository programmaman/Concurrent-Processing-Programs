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
#include <unistd.h>


/*Alec Braynen*/

/*
MUTEX
1 - I am here 0 in pid 5737
from process1 counter 4264371, jumps 13689
from process2 counter = 5500000
from parent counter  =  5500000
Time used is sec: 0, usec 268227
System Time used is sec: 0, usec 4003
*/
/*
SEMAPHORE
1 - I am here 0 in pid 5712
from process1 counter 4301877, jumps 13739 writes2750000
from process2 counter = 5500000 count2750000
from parent counter  =  5500000
Time used is sec: 0, usec 801646
System Time used is sec: 0, usec 235309
*/


struct rusage mytiming;
struct timeval beginTime, endTime;

struct sharedData
{
  int value; /* shared variable to store result*/
};

struct sharedData *counter;
int getpid();
sem_t readSem, rwMutex;
int mytot = 0, jumps = 0;

/****************************************************************
* This function increases the value of shared variable "counter"
  by one 2750000 times
****************************************************************/
void *threadModulo100(void *arg)
{
  int writes = 0;
  int countValue = 0;

  while (writes < 2750000)
  {
    sem_wait(&readSem);
    countValue = counter->value;
    sem_post(&readSem);

    if ((countValue % 100) == 0 && writes < 2749900)
    {
      sem_wait(&readSem);
      sem_wait(&rwMutex);
      counter->value = counter->value + 101;
      writes += 101;
      jumps++;
      sem_post(&readSem);
      sem_post(&rwMutex);
    }
    else
    {
      sem_wait(&rwMutex);
      counter->value++;
      ++writes;
      sem_post(&rwMutex);
    }
  }
  sem_wait(&readSem);
  printf("from process1 counter %d, jumps %d writes%d\n", counter->value, jumps, writes);
  sem_post(&readSem);
  return (NULL);
}

/****************************************************************
This function increases the value of shared variable "counter"
by one 275000 times
****************************************************************/
void *countingThread(void *arg)
{
  int line = 0;
  int count = 0;

  while (line < 2750000)
  {
    /* Critical Section */
    sem_wait(&rwMutex);
    counter->value++;
    ++line;
    ++count;
    sem_post(&rwMutex);
  }

  sem_wait(&readSem);
  printf("from process2 counter = %d count%d\n", counter->value, count);
  sem_post(&readSem);
  return (NULL);
}

void *countingThread2(void *arg)
{
  int line = 0;
  int count = 0;

  while (line < 2750000)
  {
    /* Critical Section */
    sem_wait(&rwMutex);
    counter->value++;
    ++line;
    ++count;
    sem_post(&rwMutex);
  }

  sem_wait(&readSem);
  printf("from process3 counter = %d count%d\n", counter->value, count);
  sem_post(&readSem);
  return (NULL);
}

/****************************************************************
*                  Main Body                                    *
****************************************************************/
int main()
{
  getrusage(RUSAGE_SELF, &mytiming);
  int r = 0;
  int i;
  pthread_t tid1[1];      /* process id for thread 1 */
  pthread_t tid2[1];      /* process id for thread 2 */
  pthread_t tid3[1];      /* process id for thread 3 */
  pthread_attr_t attr[1]; /* attribute pointer array */

  sem_init(&readSem, 0, 1);
  sem_init(&rwMutex, 0, 1);

  counter = (struct sharedData *)malloc(sizeof(struct sharedData));

  /* initialize shared memory to 0 */
  counter->value = 0;
  printf("1 - I am here %d in pid %d\n", r, getpid());

  fflush(stdout);
  /* Required to schedule thread independently.
 Otherwise use NULL in place of attr. */
  pthread_attr_init(&attr[0]);
  pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM); /* system-wide contention */

  /* end to schedule thread independently */

  /* Create the threads */

  pthread_create(&tid1[0], &attr[0], threadModulo100, NULL);
  pthread_create(&tid2[0], &attr[0], countingThread, NULL);
  pthread_create(&tid3[0], &attr[0], countingThread2, NULL);

  /* Wait for the threads to finish */
  pthread_join(tid1[0], NULL);
  pthread_join(tid2[0], NULL);
  pthread_join(tid3[0], NULL);



  printf("from parent counter  =  %d\n", counter->value);
  getrusage(RUSAGE_SELF, &mytiming);
  printf("Time used is sec: %d, usec %d\n", mytiming.ru_utime.tv_sec,
         mytiming.ru_utime.tv_usec);
  printf("System Time used is sec: %d, usec %d\n", mytiming.ru_stime.tv_sec,
         mytiming.ru_stime.tv_usec);
  printf("---------------------------------------------------------------------------\n");
  printf("\t\t	End of simulation\n");

  exit(0);
}

