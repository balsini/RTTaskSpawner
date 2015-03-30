/*
 * Periodicity.c
 *
 *  Created on: Nov 24, 2012
 *      Author: Alessio Balsini
 */

#include <stdio.h>
#include "periodicity.h"

/*
 * Copies the second passed time to the first one
 */

inline void time_copy(struct timespec *dst, const struct timespec *src)
{
	dst->tv_sec = src->tv_sec;
	dst->tv_nsec = src->tv_nsec;
}

/*
 * Adds milliseconds to the given time
 */

inline void time_add_ms(struct timespec *dst, long int ms)
{
	dst->tv_sec += ms/1000;
	dst->tv_nsec += (ms % 1000) * 1e6;
	if (dst->tv_nsec > 1e9) {
		dst->tv_nsec -= 1e9;
		dst->tv_sec++;
	}
}

/* Compares two times
 *
 * returns:
 * 		1 if the first time passed is greater than the second one
 * 		0 if times are equal
 * 		-1 otherwise
 */

inline int time_cmp(struct timespec *t1, struct timespec *t2)
{
	if (t1->tv_sec > t2->tv_sec) return 1;
	if (t1->tv_sec < t2->tv_sec) return -1;
	if (t1->tv_nsec > t2->tv_nsec) return 1;
	if (t1->tv_nsec < t2->tv_nsec) return -1;
	return 0;
}

/*
 * Checks if thread missed it's deadline and updates thread's
 * attributes
 *
 * returns:
 * 		0 if no deadline miss occurred.
 * 		1 otherwise.
 */

inline int deadline_miss(periodic_task_attr *ta)
{
	static struct timespec now;

	clock_gettime(CLOCK_MONOTONIC, &now);
	if (time_cmp(&now, &(ta->dl)) > 0) {
		ta->dmiss++;
		ta->period++;
		ta->deadline++;
		return 1;
	}
	return 0;
}

/*
 * Computes the next activation point of the task
 */

inline void set_period(periodic_task_attr *ta)
{
	static struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	time_copy(&(ta->at), &t);
	time_copy(&(ta->dl), &t);
	time_add_ms(&(ta->at), ta->period);
	time_add_ms(&(ta->dl), ta->deadline);
}

/*
 * Suspends the calling thread until next activation and, when
 * awakened, updates it's activation time and deadline.
 */

inline void wait_for_period(periodic_task_attr *ta)
{
	deadline_miss(ta);
	//printf("Waiting for period\n");
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &(ta->at), NULL);
	time_add_ms(&(ta->at), ta->period);
	time_add_ms(&(ta->dl), ta->period);
	//printf("Woken up\n");
}
 
