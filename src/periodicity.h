/*
 * Periodicity.h
 *
 *  Created on: Nov 24, 2012
 *      Author: Alessio Balsini
 */

#ifndef __PERIODICITY_H__
#define __PERIODICITY_H__

#include <time.h>
#include <pthread.h>

/*
 * All the values are in ms
 */
typedef struct periodic_task_attr_ {
  void * arg;		// task argument
  int dmiss;		// number of deadline misses

  // The task acts as follows:
  //
  //    c0     ss   c1    s_runtime    deadline
  // |#######======####_______)___________| . . .

  int jobs;		// Number of jobs for each task
  int ss_every;		// The task Self-Suspends after every ss_every
			//   activations
  long int ss;		// The task Self-Suspends after ss_every activations
  long int c0;		// Duration of C0
  long int c1;		// Duration of C1

  long int period;		// task period
  long int deadline;		// task deadline

  long int s_period;		// server period
  long int s_deadline;	// server deadline
  long int s_runtime;	// server runtime that will be used
                        //   by CBS for bandwidth management

  struct timespec at;	// next activation time
  struct timespec dl;	// abs. deadline
} periodic_task_attr;

void periodicity_init();
void set_period(periodic_task_attr * periodic_task_attribute);
void wait_for_period(periodic_task_attr * periodic_task_attribute);
void busy_wait(int ns);
void susp_wait(int ns);
void time_add_ms(struct timespec *dst, long int ms);
void time_add_ns(struct timespec *dst, long int ms);
void print_pta(periodic_task_attr *p);
void print_pta_json(periodic_task_attr p[], unsigned int size);

#endif /* __PERIODICITY_H__ */
