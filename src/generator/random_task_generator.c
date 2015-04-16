#include "random_task_generator.h"

#include <time.h>
#include <stdlib.h>

#include <stdio.h>


void print_pta_json(periodic_task_attr p[], unsigned int size)
{
  unsigned int i;

  printf("{\n");
  printf("\t\"tasks\" : {\n");

  for (i=0; i<size; ++i) {
      printf("\t\t\"thread%d\" : {\n", i);

      printf("\t\t\t\"jobs\":\t\t%d.0,\n", p[i].jobs);
      printf("\t\t\t\"ss_every\":\t%d.0,\n", p[i].ss_every);
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


/*
 * Generates the utilization factors
 */
void random_task_generator_U(float U[],
                             unsigned int size,
                             float U_lb,
                             float U_tot)
{
  int i, j;
  float U_sum;
  float app;
  float *U_tmp;
  double m, q;

  U_tmp = (float *)malloc(sizeof(float) * size);

  U_sum = 0;
  for (i=0; i<size; ++i) {
      U_tmp[i] = (double)rand() / RAND_MAX;
      U_sum += U_tmp[i];
  }

  // Reordering array
  for (i=0; i<size; ++i) {
      for (j=0; j<size - i - 1; ++j) {
          if (U_tmp[j] > U_tmp[j+1]) {
              app = U_tmp[j];
              U_tmp[j] = U_tmp[j+1];
              U_tmp[j+1] = app;
          }
      }
  }

  m = (U_tot - size * U_lb) / (U_sum - size * U_tmp[0]);
  q = U_lb / m - U_tmp[0];

  U_sum = 0;
  for (i=0; i<size; ++i) {
      U[i] = (U_tmp[i] + q) * m;
      U_sum += U[i];
      //printf("U[%d]:\t%f\n", i, U[i]);
  }

  //printf("U_sum:\t%f\n", U_sum);

  free(U_tmp);
}

/*
 * Generates the periods and runtimes
 */
void random_task_generator_TC(float U[],
                             unsigned int T[],
                             unsigned int C[],
                             unsigned int size,
                             unsigned int T_min,
                             unsigned int T_max)
{
  int i;

  // Generating random values
  for (i=0; i<size; ++i) {
      do {
          T[i] = rand() % (T_max + 1);
          C[i] = U[i] * T[i];
      } while (T[i] < T_min || C[i] < 1024);

      //printf("T[%d]: %d\n", i, T[i]);
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
  random_task_generator_TC(U, T, C, size, T_min, T_max);
  //printf("\n");
  random_task_generator_PTA(*p, C, T, size, jobs);

  free(U);
  free(T);
  free(C);
}
