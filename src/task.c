#include "task.h"

#include "periodicity.h"

pthread_mutex_t console_mux = PTHREAD_MUTEX_INITIALIZER;

void task_init(struct task_parameters *p)
{
  struct sched_attr attr __attribute__ ((aligned (4)));// = p->attr;
  int r;

  attr.size = sizeof(attr);
  attr.sched_flags = 0;
  attr.sched_nice = 0;
  attr.sched_priority = 0;

  /* This creates a 10ms/30ms reservation */
  attr.sched_policy = SCHED_DEADLINE;
  attr.sched_runtime = 10 * 1000 * 1000;
  attr.sched_period = attr.sched_deadline = 30 * 1000 * 1000;

  r = sched_setattr(0, &attr, 0);
  if (r < 0) {
    pthread_mutex_lock(&console_mux);
    perror("ERROR: sched_setattr");
    pthread_mutex_unlock(&console_mux);
    pthread_exit(NULL);
  }
}

void task_body()
{
  unsigned int i, j;

  pthread_mutex_lock(&console_mux);
  printf("Into Task Body [ %ld ]\n", gettid());
  pthread_mutex_unlock(&console_mux);
  //set_period(attr);

  for (i=0; i<1000; i++) {

    // DO STUFF
    for (j=0; j<10000; j++) ;

    //wait_for_period(attr);
  }
}

void *task_main(void *arg)
{
  pthread_mutex_lock(&console_mux);
  printf("Thread started [ %ld ]", gettid());
  pthread_mutex_unlock(&console_mux);

  task_init((struct task_parameters *)arg);
  task_body();

  pthread_mutex_lock(&console_mux);
  printf("Thread completed [ %ld ]", gettid());
  pthread_mutex_unlock(&console_mux);

  return NULL;
}
