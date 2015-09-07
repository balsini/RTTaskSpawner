#ifndef __TASK_HPP__
#define __TASK_HPP__

#include "sched_new.h"

extern pthread_barrier_t barrier;
extern struct timespec **finishing_time;
extern struct timespec **activation_time;
extern struct timespec first_activation_time;
extern int first_activation_time_set;

void *task_main(void *arg);

#endif
