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
void time_add_ms(struct timespec *dst, long int ms)
{
  dst->tv_sec += ms/1000;
  dst->tv_nsec += (ms % 1000) * 1e6;
  if (dst->tv_nsec > 1e9) {
    dst->tv_nsec -= 1e9;
    dst->tv_sec++;
  }
}

/*
 * Adds nanoseconds to the given time
 */
void time_add_ns(struct timespec *dst, long int ns)
{
  dst->tv_nsec += ns % (1000 * 1000);
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
  struct timespec now;

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
  struct timespec t;

  clock_gettime(CLOCK_MONOTONIC, &t);
  time_copy(&(ta->at), &t);
  time_copy(&(ta->dl), &t);
  time_add_ns(&(ta->at), ta->period);
  time_add_ns(&(ta->dl), ta->deadline);
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
  time_add_ns(&(ta->at), ta->period);
  time_add_ns(&(ta->dl), ta->period);
  //printf("Woken up\n");
}

/*
 * Suspends the calling thread for a certain number of ns in a
 * busy wait fashion.
 */
void busy_wait(int ns)
{
  struct timespec t, now;

  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
  time_add_ns(&t, ns);
  do {
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);
  } while (time_cmp(&now, &t) < 0) ;
}

/*
 * Suspends the calling thread for a certain number of ms in a
 * suspending wait fashion.
 */
void susp_wait(int ns)
{
  struct timespec t;

  clock_gettime(CLOCK_MONOTONIC, &t);
  time_add_ns(&t, ns);
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
}

void print_pta(periodic_task_attr *p)
{
  printf("jobs:\t\t%d\n", p->jobs);
  printf("ss_every:\t%d\n", p->ss_every);
  printf("ss:\t\t%d\n", p->ss);
  printf("c0:\t\t%d\n", p->c0);
  printf("c1:\t\t%d\n", p->c1);
  printf("period:\t\t%d\n", p->period);
  printf("deadline:\t%d\n", p->deadline);
  printf("s_period:\t%d\n", p->s_period);
  printf("s_deadline:\t%d\n", p->s_deadline);
  printf("s_runtime:\t%d\n", p->s_runtime);
}

void print_pta_json(periodic_task_attr p[], unsigned int size)
{
  unsigned int i;

  printf("{\n");
  printf("\t\"tasks\" : {\n");

  for (i=0; i<size; ++i) {
    printf("\t\t\"thread%d\" : {\n", i);

    printf("\t\t\t\"jobs\":\t\t%ld.0,\n", p[i].jobs);
    printf("\t\t\t\"ss_every\":\t%ld.0,\n", p[i].ss_every);
    printf("\t\t\t\"ss\":\t\t%ld.0,\n", p[i].ss);
    printf("\t\t\t\"c0\":\t\t%ld.0,\n", p[i].c0);
    printf("\t\t\t\"c1\":\t\t%ld.0,\n", p[i].c1);
    printf("\t\t\t\"period\":\t%ld.0,\n", p[i].period);
    printf("\t\t\t\"deadline\":\t%ld.0,\n", p[i].deadline);
    printf("\t\t\t\"s_period\":\t%ld.0,\n", p[i].s_period);
    printf("\t\t\t\"s_deadline\":\t%ld.0,\n", p[i].s_deadline);
    printf("\t\t\t\"s_runtime\":\t%ld.0\n", p[i].s_runtime);

    if (i == size-1)
      printf("\t\t}\n");
    else
      printf("\t\t},\n");
  }
  printf("\t}\n}\n");
}
