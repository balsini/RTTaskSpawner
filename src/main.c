#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>

#include "task.h"
#include "periodicity.h"

#define gettid() syscall(__NR_gettid)

unsigned int thread_count;
pthread_t thread_list[100];

void create_task(periodic_task_attr *param)
{
  printf("Creating task\n");

  pthread_create(&thread_list[thread_count], NULL, task_main, (void *)param);

  thread_count++;
}

int main()
{
  periodic_task_attr p0, p1;

  thread_count = 0;

  printf("Spawner started\n");

  p0.c0 = 10;
  p0.ss = 50;
  p0.c1 = 10;
  p0.ss_every = 0;
  p0.jobs = 50;

  p0.period = 100;
  p0.deadline = 100;

  p0.s_deadline = 100;
  p0.s_period = 100;
  p0.s_runtime = 40;


  p1.c0 = 100;
  p1.ss = 0;
  p1.c1 = 0;
  p1.ss_every = 999999;
  p1.jobs = 50;

  p1.period = 200;
  p1.deadline = 200;

  p1.s_deadline = 200;
  p1.s_period = 200;
  p1.s_runtime = 100;

  create_task(&p0);
  create_task(&p1);

  while (thread_count) {
    thread_count--;
    pthread_join(thread_list[thread_count], NULL);
  }

  printf("Spawner ended\n");
  return 0;
}
