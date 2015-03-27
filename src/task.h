#ifndef __TASK_HPP__
#define __TASK_HPP__

#include <time.h>

#include "sched_new.h"

// The task acts as follows:
//
//    C0     SS   C1     Runtime     Deadline
// |#######------####_______)___________| . . .

struct task_parameters {
  struct sched_attr attr;
  unsigned int ss_every;        // The task Self-Suspends after ss_every activations
  struct timespec ss;           // The task Self-Suspends after ss_every activations
  struct timespec c0;           // Duration of C0
  struct timespec c1;           // Duration of C1
};

void *task_main(void *arg);

#endif
