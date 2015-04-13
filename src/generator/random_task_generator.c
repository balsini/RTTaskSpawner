#include "random_task_generator.h"

#include <time.h>
#include <stdlib.h>

#include <stdio.h>

/*
 * Generates the utilization factors
 */
void random_task_generator_U(float U[],
                             unsigned int size,
                             float U_lb,
                             float U_tot)
{
  int i, j;
  unsigned int U_big;
  float U_tot_d = 0;
  float app;
  float granularity = 10.0 * 1000.0;

  // Generating random values
  for (i=0; i<size; ++i) {
    do {
      U_big = rand() % ((int)(U_tot * granularity));
    } while (U_big < U_lb * granularity);

    U[i] = U_big / granularity;
    //printf("U[%d]: %f\n", i, U[i]);
  }

  // Reordering array
  for (i=0; i<size; ++i) {
    for (j=0; j<size - i - 1; ++j) {
      if (U[j] > U[j+1]) {
        app = U[j];
        U[j] = U[j+1];
        U[j+1] = app;
      }
    }
  }

  // Assigning correct utilization factors
  U[0] -= U_lb;
  U[size - 1] = U_tot - U[size - 2];
  for (i=size-2; i>0; i--)
    U[i] -= U[i - 1];

  for (i=0; i<size; ++i) {
    //printf("U[%d]:\t%f\n", i, U[i]);
    U_tot_d += U[i];
  }

  //printf("U_tot:\t%f\n", U_tot_d);
}

/*
 * Generates the periods
 */
void random_task_generator_T(unsigned int T[],
                             unsigned int size,
                             unsigned int T_min,
                             unsigned int T_max)
{
  int i;

  // Generating random values
  for (i=0; i<size; ++i) {
    do {
      T[i] = rand() % (T_max + 1);
    } while (T[i] < T_min);

    //printf("T[%d]: %d\n", i, T[i]);
  }
}

/*
 * Generates the computation times
 */
void random_task_generator_C(unsigned int C[],
                             unsigned int T[],
                             float U[],
                             unsigned int size)
{
  int i;

  for (i=0; i<size; ++i) {
    C[i] = U[i] * T[i];

    //printf("C[%d]: %d\n", i, C[i]);
  }
}

/*
 * Generates the periodic_task_attr
 */
void random_task_generator_PTA(periodic_task_attr p[],
                               unsigned int C[],
                               unsigned int T[],
                               unsigned int size,
                               unsigned int jobs)
{
  int i;
  unsigned int C_residual;

  for (i=0; i<size; ++i) {
    p[i].ss_every = 0;
    p[i].jobs = jobs;

    p[i].s_deadline = T[i];
    p[i].s_period = T[i];
    p[i].s_runtime = C[i];

    p[i].period = p[i].s_period;
    p[i].deadline = p[i].s_deadline;

    C_residual = C[i] * 80 / 100;

    p[i].c0 = C_residual * 10 / 60;
    p[i].ss = C_residual * 50 / 60;
    p[i].c1 = C_residual * 10 / 60;
  }

  /*
  printf("c0\tss\tc1\tdl\tperiod\n");

  for (i=0; i<size; ++i) {
    printf("%d\t%d\t%d\t%d\t%d\n",
           p[i].c0,
           p[i].ss,
           p[i].c1,
           p[i].deadline,
           p[i].period);
  }
  */
}

void random_task_generator(periodic_task_attr *p[],
                           unsigned int size,
                           float U_lb,
                           float U_tot,
                           unsigned int T_min,
                           unsigned int T_max,
                           unsigned int jobs)
{
  //unsigned int i;
  float *U;
  unsigned int *T;
  unsigned int *C;

  *p = (periodic_task_attr *)malloc(sizeof(periodic_task_attr) * size);
  U = (float *)malloc(sizeof(float) * size);
  T = (unsigned int *)malloc(sizeof(unsigned int) * size);
  C = (unsigned int *)malloc(sizeof(unsigned int) * size);
  
  srand(time(NULL));

  random_task_generator_U(U, size, U_lb, U_tot);
  //printf("\n");
  random_task_generator_T(T, size, T_min, T_max);
  //printf("\n");
  random_task_generator_C(C, T, U, size);
  //printf("\n");
  random_task_generator_PTA(*p, C, T, size, jobs);

  free(U);
  free(T);
  free(C);
}
