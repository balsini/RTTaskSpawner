#include "task.hpp"
#include "sched_new.h"

#include <pthread.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdio.h>

using namespace std;

pthread_mutex_t console_mux = PTHREAD_MUTEX_INITIALIZER;

pid_t gettid() {
  return (pid_t) syscall (SYS_gettid);
}

void task_init()
{
  struct sched_attr a;
  int r;

  a.size =            sizeof(a);
  a.sched_policy =    SCHED_DEADLINE;

  /* This creates a 10ms/30ms reservation */
  a.sched_runtime =   10 * 1000 * 1000;
  a.sched_period =    30 * 1000 * 1000;
  a.sched_deadline =  30 * 1000 * 1000;

  r = sched_setattr(0, &a, 0);
  if (r < 0) {
    pthread_mutex_lock(&console_mux);
    perror("ERROR: sched_setattr");
    pthread_mutex_unlock(&console_mux);
    pthread_exit(NULL);
  }
}

void task_body()
{
  for (unsigned int i=0; i<10000; i++) ;
}

void *task_main(void *arg)
{
  pthread_mutex_lock(&console_mux);
  cout << "Thread started [ " << gettid() << " ]" << endl;
  pthread_mutex_unlock(&console_mux);

  task_init();
  task_body();
}
