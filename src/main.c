#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <time.h>
#include <limits.h>

#include "task.h"
#include "periodicity.h"
#include "json_inout.h"

#define gettid() syscall(__NR_gettid)

#define TASK_NUM_MAX 50

unsigned int thread_count;
pthread_t *thread_list;
extern pthread_barrier_t barrier;

extern struct timespec **finishing_time;
extern struct timespec **activation_time;
extern struct timespec first_activation_time;
extern int first_activation_time_set;

void create_task(periodic_task_attr *param)
{
  //printf("Creating task\n");
  finishing_time[param->id] = malloc(sizeof(struct timespec) * param->jobs);
  activation_time[param->id] = malloc(sizeof(struct timespec) * param->jobs);
  pthread_create(&thread_list[thread_count], NULL, task_main, (void *)param);

  thread_count++;
}

long double timespec2double(struct timespec t)
{
    long double time;
    
    time = t.tv_sec;
    time = time + t.tv_nsec / (long double)1e9;
    
    //printf("%lf\n", (double)time);

    return time;
}

int main()
{
  periodic_task_attr *p;
  unsigned int i;
  unsigned int size = 50;
  long double *response_time;
  long double a, f, r;

  thread_count = 0;
  first_activation_time_set = 0;

  //printf("Spawner started\n");

  parse_config_stdin(&p, &size);

  pthread_barrier_init(&barrier, NULL, size);

  thread_list = (pthread_t *)malloc(sizeof(pthread_t) * size);

  print_pta_json(p, size);

  printf("Creating %d threads\n", size);

  finishing_time = malloc(sizeof(struct timespec *) * size);
  activation_time = malloc(sizeof(struct timespec *) * size);
  for (i=0; i<size; ++i) {
      p[i].id = i;
    create_task(&p[i]);
  }

  while (thread_count) {
    thread_count--;
    pthread_join(thread_list[thread_count], NULL);
    //printf("[%d/%d]\n", thread_count, size);
  }

  // Gets response time statistics
  // Takes the smaller activation time among all, this will 
  //   be the actiovation time of each task
  
  response_time = malloc(sizeof(double) * size);
  
/*
  a = (long double)LONG_MAX;
  for (i=0; i<size; ++i) {
      if (timespec2double(activation_time[i]) < a)
          a = timespec2double(activation_time[i]);
  }
  */
  
  printf("Task\tRT\n");
  
  for (i=0; i<size; ++i) {
    int j;
    
    response_time[0] = timespec2double(finishing_time[i][0]) - timespec2double(first_activation_time);
    printf("%llf = %llf - %llf\n",
	   response_time[0],
	   timespec2double(finishing_time[i][0]),
	   timespec2double(first_activation_time));
    
    for (j=1; j<p[i].jobs; ++j) {
      /*printf("%d:\t%ld.%ld  ---  %ld.%ld\n", i,
             finishing_time[i][j].tv_sec, finishing_time[i][j].tv_nsec,
             activation_time[i].tv_sec, activation_time[i].tv_nsec
            );
      */
            
      /*
      a = a + p[i].period * j / (long double)1e9;
      f = timespec2double(finishing_time[i][j]);
      r = f - a;
      
      response_time[i] = response_time[i] + timespec2double(finishing_time[i][j]) - a;
      */
      
      response_time[i] = timespec2double(finishing_time[i][j]) - timespec2double(activation_time[i][j-1]);
      
      printf("%llf = %llf - %llf\n", response_time[i], timespec2double(finishing_time[i][j]), timespec2double(activation_time[i][j-1]));
      
      //printf("Activation time: %lf\n", timespec2double(activation_time[i]) * (j + 1.0));
      //printf("%d\t%lf\n", i, (double)response_time[i]);
      //printf("%d\t%lf - %lf = %lf\n", i, (double)f, (double)a, (double)r);
      
    }
    
    //printf("%d\t%lf\n", i, response_time[i]);
    
    response_time[i] = response_time[i] / (long double)p[i].jobs;
    
    //printf("%d\t%lf\n", i, (double)response_time[i]);
  }
  
  /*
  free(response_time);
  for (i=0; i<size; ++i) {
	  int j;
	  for (j=0; j<p[i].jobs; ++j) {
		  free(finishing_time[i][j]);
		  free(activation_time[i][j]);
	}
	free(finishing_time[i]);
	free(activation_time[i]);
  }
  
  free(thread_list);
  free(p);
  */
  
  //printf("Spawner ended\n");
  return 0;
}
