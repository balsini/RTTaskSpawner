#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>

#include "task.h"


#define gettid() syscall(__NR_gettid)

unsigned int thread_count;
pthread_t thread_list[100];

void create_thread(struct task_parameters param)
{
  printf("Creating task\n");

  pthread_create(&thread_list[thread_count], NULL, task_main, (void *)&param);

  thread_count++;
}

int main()
{
  thread_count = 0;

  printf("Spawner started\n");

  struct task_parameters p;

  p.c0.tv_sec = 0;
  p.c0.tv_nsec = 10 * 1000;
  p.c1.tv_sec = 0;
  p.c1.tv_nsec = 10 * 1000;
  p.ss.tv_sec = 0;
  p.ss.tv_nsec = 10 * 1000;

  p.ss_every = 0;

  p.attr.size = sizeof(p.attr);
  p.attr.sched_deadline = p.attr.sched_period = 10 * 1000 * 1000;
  p.attr.sched_runtime = 5 * 1000 * 1000;

  create_thread(p);

  while (thread_count) {
    thread_count--;
    pthread_join(thread_list[thread_count], NULL);
  }

  printf("Spawner ended\n");
  return 0;
}
