#include <time.h>

#include "task.h"
#include "periodicity.h"

pthread_mutex_t console_mux = PTHREAD_MUTEX_INITIALIZER;

void print_time(const struct timespec *t)
{
  pthread_mutex_lock(&console_mux);
  printf("Time: %lld.%ld\n", (long long)t->tv_sec, t->tv_nsec);
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
  attr.sched_runtime =  1000 * 1000 * pta->s_runtime;
  attr.sched_period =   1000 * 1000 * pta->s_period;
  attr.sched_deadline = 1000 * 1000 * pta->s_deadline;

  r = sched_setattr(0, &attr, 0);
  if (r < 0) {
    pthread_mutex_lock(&console_mux);
    perror("ERROR: sched_setattr");
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

  //pthread_mutex_lock(&console_mux);
  //printf("Into Task Body [ %ld ]\n", gettid());
  //pthread_mutex_unlock(&console_mux);

  set_period(pta);

  for (i=0; i<pta->jobs; ++i) {
    //clock_gettime(CLOCK_MONOTONIC, &now);
    //print_time(&now);

    // Busy wait for c0
    busy_wait(pta->c0);

    //clock_gettime(CLOCK_MONOTONIC, &now);
    //print_time(&now);

    // Self suspension
    every = (every + 1) % pta->ss_every;
    if (every == 0)
      susp_wait(pta->ss);

    //clock_gettime(CLOCK_MONOTONIC, &now);
    //print_time(&now);

    // Wait for c1
    busy_wait(pta->c1);

    //clock_gettime(CLOCK_MONOTONIC, &now);
    //print_time(&now);

    //printf("\n");

    wait_for_period(pta);
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

  return NULL;
}
