#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>

#include "../periodicity.h"
#include "random_task_generator.h"

int main()
{
  periodic_task_attr *p;
  unsigned int size = 50;

  random_task_generator(&p,
                        size,
                        0.01,
                        0.8,
                        100 * 1000,
                        1 * 1000 * 1000,
                        500);

  //printf("Creating %d threads\n", size);

  print_pta_json(p, size);

  return 0;
}
