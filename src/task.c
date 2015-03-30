#include "task.h"

#include "periodicity.h"

pthread_mutex_t console_mux = PTHREAD_MUTEX_INITIALIZER;

void task_init(periodic_task_attr *pta)
{
  int r;
  struct sched_attr attr;

  attr.size = sizeof(attr);
  attr.sched_flags = 0;
  attr.sched_nice = 0;
  attr.sched_priority = 0;

  attr.sched_policy = SCHED_DEADLINE;
  attr.sched_runtime = pta->s_runtime * 1000 * 1000;
  attr.sched_period = pta->s_period * 1000 * 1000;
  attr.sched_deadline = pta->s_deadline * 1000 * 1000;

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
  unsigned int i, j;

  pthread_mutex_lock(&console_mux);
  printf("Into Task Body [ %ld ]\n", gettid());
  pthread_mutex_unlock(&console_mux);

  set_period(pta);

  for (i=0; i<pta->jobs; ++i) {

    static struct timespec t;

    clock_gettime(CLOCK_MONOTONIC, &t);

    // DO STUFF
    //for (j=0; j<10000; j++) ;

    printf("Do stuff %d [ %ld ]\n", i, gettid());

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
