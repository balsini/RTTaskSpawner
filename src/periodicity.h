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

typedef struct periodic_task_attr_ {
	void * arg;			// task argument
	int period;			// in milliseconds
	int deadline;		// relative (ms)
	int priority;		// in [0,99]
	int dmiss;			// # of deadline misses
	int running;		// 1 if running, 0 if dead
	struct timespec at;	// next activation time
	struct timespec dl;	// abs. deadline
	pthread_mutex_t mux;// mutex for this data struct.
} periodic_task_attr;

void set_period(periodic_task_attr * periodic_task_attribute);
void wait_for_period(periodic_task_attr * periodic_task_attribute);

#endif /* __PERIODICITY_H__ */
