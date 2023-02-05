#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
/* Tested by LOH 2/1/21. */
/* compile with gcc -ofname thread-template.c -lpthread */
/* Solve with mutex locks */

/*Alec Braynen*/

struct rusage mytiming;
struct timeval beginTime, endTime;

struct shared_dat
{
  int value; /* shared variable to store result*/
};

struct shared_dat *counter;
int getpid();
bool worker = 1; // 0 means thread 1 is working. 1 means thread 2
pthread_mutex_t counterMutex;
pthread_cond_t wait_var;
int mytot = 0, jumps = 0;

/****************************************************************
* This function increases the value of shared variable "counter"
  by one 2750000 times
****************************************************************/
void *thread1(void *arg) //I tried cond wait and cond signal with worker but the behavior seemed the seem as just using an if statement on pthread mutex try lock
{
  int writes = 0;

  while (writes < 2750000)
  {
    if (pthread_mutex_trylock(&counterMutex) == 0)
    {
      if ((counter->value % 100) == 0)
      {
        counter->value = counter->value + 100;
        writes = writes + 100;
        jumps++;
        // worker = 1;
        pthread_mutex_unlock(&counterMutex);
      }
      else
      {
        counter->value = counter->value + 1;
        writes++;
        // worker = 1;
        // pthread_cond_signal(&wait_var);
        pthread_mutex_unlock(&counterMutex);
      }
    }
  }
  printf("from process1 counter %d, jumps %d \n", counter->value, jumps);
  return (NULL);
}

/****************************************************************
This function increases the value of shared variable "counter"
by one 275000 times
****************************************************************/
void *thread2(void *arg)
{
  int line = 0;
  int count = 0;

  while (line < 2750000)
  {
    /* Critical Section */
    if (pthread_mutex_trylock(&counterMutex) == 0)
    {
      counter->value = counter->value + 1;
      // worker = 0; //Thread is done with critcal variable
      // pthread_cond_signal(&wait_var);
      pthread_mutex_unlock(&counterMutex);
      line++;
      count++;
    }
  }

  printf("from process2 counter = %d\n", counter->value);
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

  pthread_mutex_init(&counterMutex, NULL);
  pthread_cond_init(&wait_var, NULL);
  counter = (struct shared_dat *)malloc(sizeof(struct shared_dat));

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

  pthread_create(&tid2[0], &attr[0], thread2, NULL);
  pthread_create(&tid1[0], &attr[0], thread1, NULL);

  /* Wait for the threads to finish */
  pthread_join(tid2[0], NULL);
  pthread_join(tid1[0], NULL);

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
