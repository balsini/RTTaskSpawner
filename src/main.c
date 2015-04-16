#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>

#include "task.h"
#include "periodicity.h"
#include "json_inout.h"

#define gettid() syscall(__NR_gettid)

#define TASK_NUM_MAX 50

unsigned int thread_count;
pthread_t *thread_list;

void create_task(periodic_task_attr *param)
{
  //printf("Creating task\n");

  pthread_create(&thread_list[thread_count], NULL, task_main, (void *)param);

  thread_count++;
}

int main()
{
  periodic_task_attr *p;
  unsigned int i;
  unsigned int size = 50;

  thread_count = 0;
  
  //printf("Spawner started\n");

  parse_config_stdin(&p, &size);

  thread_list = (pthread_t *)malloc(sizeof(pthread_t) * size);

  print_pta_json(p, size);

  printf("Creating %d threads\n", size);

  for (i=0; i<size; ++i)
    create_task(&p[i]);

  while (thread_count) {
    thread_count--;
    pthread_join(thread_list[thread_count], NULL);
    //printf("[%d/%d]\n", thread_count, size);
  }

  free(thread_list);
  free(p);

  //printf("Spawner ended\n");
  return 0;
}
