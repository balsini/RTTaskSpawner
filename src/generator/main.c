#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>

#include "../periodicity.h"
#include "random_task_generator.h"

int main(int argc, char *argv[])
{
  periodic_task_attr *p;
  unsigned int size = 2;
  unsigned int jobs = 300;
  unsigned long period_min = 1000 * 1000;
  unsigned long period_max = 10 * 1000 * 1000;
  float U_lb = 0.3;
  float U_tot = 0.8;
  int c;

  opterr = 0;
  while ((c = getopt (argc, argv, "t:U:u:j:p:P:")) != -1) {
    switch (c)
    {
      case 't':
        size = atoi(optarg);
        break;
      case 'j':
        jobs = atoi(optarg);
        break;
      case 'u':
        U_lb = atof(optarg);
        break;
      case 'U':
        U_tot = atof(optarg);
        break;
      case 'p':
        period_min = atof(optarg);
        break;
      case 'P':
        period_max = atof(optarg);
        break;
      case '?':
        if (optopt == 't')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
    }
  }

  //printf("Generating tasks configuration with:\nsize: %d\nU_tot: %f\nU_lb: %f\njobs: %d\n", size, U_tot, U_lb, jobs);

  if (U_lb * size > U_tot) {
	  printf("The system cannot generate the desired configuration:\n");
	  printf("For generating a taskset of [%d] tasks with [%f], you need a U_lb of at maximum [%f]\n", size, U_tot, U_tot / size);
	  return -1;
  }

  random_task_generator(&p,
                        size,
                        U_lb,
                        U_tot,
                        period_min,
                        period_max,
                        jobs);

  //printf("Creating %d threads\n", size);

  print_pta_json(p, size);

  return 0;
}
