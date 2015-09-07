#define _GNU_SOURCE

#include <pthread.h>
#include <time.h>
#include <sched.h>

#include "task.h"
#include "periodicity.h"

pthread_barrier_t barrier;
struct timespec **finishing_time;
struct timespec **activation_time;
struct timespec first_activation_time;
int first_activation_time_set;

pthread_mutex_t console_mux = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t first_activation_mux = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ss0_mux = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ss1_mux = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ss2_mux = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ss3_mux = PTHREAD_MUTEX_INITIALIZER;

void print_time(const struct timespec *t)
{
  pthread_mutex_lock(&console_mux);
  printf("[ %ld ] Time: %lld.%ld\n", gettid(), (long long)t->tv_sec, t->tv_nsec);
  pthread_mutex_unlock(&console_mux);
}

void task_init(periodic_task_attr *pta)
{
  int r;
  struct sched_attr attr;

  attr.size = sizeof(attr);
  attr.sched_flags =    0;
  attr.sched_nice =     0;
  attr.sched_priority = 0;

  attr.sched_policy =   SCHED_DEADLINE;
  attr.sched_runtime =  pta->s_runtime;
  attr.sched_period =   pta->s_period;
  attr.sched_deadline = pta->s_deadline;

  r = sched_setattr(0, &attr, 0);
  if (r < 0) {
    pthread_mutex_lock(&console_mux);
    perror("ERROR: sched_setattr");
    printf("runtime: %lld\nperiod: %lld\ndeadline: %lld\n",
           attr.sched_runtime,
           attr.sched_period,
           attr.sched_deadline);
    pthread_mutex_unlock(&console_mux);
    pthread_exit(NULL);
  }
}

void task_body(periodic_task_attr *pta)
{
  unsigned int i;
  unsigned int every;
  //struct timespec now;

  every = 0;

  pthread_mutex_lock(&console_mux);
  printf("Into Task Body [ %ld ]\n", gettid());
  pthread_mutex_unlock(&console_mux);

  pthread_barrier_wait(&barrier);
  
  pthread_mutex_lock(&first_activation_mux);
  if (first_activation_time_set == 0) {
	clock_gettime(CLOCK_MONOTONIC, &(first_activation_time));
	first_activation_time_set = 1;
  }
  pthread_mutex_unlock(&first_activation_mux);
  
  //set_period(pta);
  
  for (i=0; i<pta->jobs; ++i) {
    //clock_gettime(CLOCK_MONOTONIC, &now);
    //print_time(&now);

    // Busy wait for c0
    busy_wait(pta->c0);

    //clock_gettime(CLOCK_MONOTONIC, &now);
    //print_time(&now);

    // Self suspension
    if (pta->ss_every > 0)
      every = (every + 1) % pta->ss_every;
    if (every == 0) {
/*
      if (gettid() % 2) {
        pthread_mutex_lock(&ss0_mux);
        busy_wait(pta->c0 * 1000);
        pthread_mutex_lock(&ss1_mux);
        busy_wait(pta->c0 * 1000);
        pthread_mutex_unlock(&ss1_mux);
        pthread_mutex_unlock(&ss0_mux);
      } else {
        pthread_mutex_lock(&ss2_mux);
        busy_wait(pta->c0 * 1000);
        pthread_mutex_lock(&ss3_mux);
        busy_wait(pta->c0 * 1000);
        pthread_mutex_unlock(&ss3_mux);
        pthread_mutex_unlock(&ss2_mux);
      }
*/
      susp_wait(pta->ss);
    }

    //clock_gettime(CLOCK_MONOTONIC, &now);
    //print_time(&now);

    // Wait for c1
    busy_wait(pta->c1);

    //clock_gettime(CLOCK_MONOTONIC, &now);
    //print_time(&now);

    //printf("\n");

    clock_gettime(CLOCK_MONOTONIC, &(finishing_time[pta->id][i]));

    // Check deadline miss
    // (finishing_time - activation_time)

    //wait_for_period(pta);

    // Next activation time = (finishing_time + period) % period
    clock_gettime(CLOCK_MONOTONIC, &(activation_time[pta->id][i]));


    sched_yield();
    

  }
}

void *task_main(void *arg)
{
  pthread_mutex_lock(&console_mux);
  printf("Thread started [ %ld ]\n", gettid());
  pthread_mutex_unlock(&console_mux);

  task_init((periodic_task_attr *)arg);
  task_body((periodic_task_attr *)arg);

  pthread_mutex_lock(&console_mux);
  printf("Thread completed [ %ld ]\n", gettid());
  pthread_mutex_unlock(&console_mux);

  pthread_exit(0);
}
