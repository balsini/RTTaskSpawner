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
  thread_count = 0;

  printf("Spawner started\n");

  periodic_task_attr p;

  p.c0 = 10;
  p.ss = 5;
  p.c1 = 5;
  p.ss_every = 3;
  p.jobs = 50;

  p.period = 40;
  p.deadline = 40;

  p.s_deadline = 40;
  p.s_period = 40;
  p.s_runtime = 30;

  create_task(&p);

  while (thread_count) {
    thread_count--;
    pthread_join(thread_list[thread_count], NULL);
  }

  printf("Spawner ended\n");
  return 0;
}
