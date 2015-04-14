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
  unsigned int size = 64;
  float U_lb = 0.005;
  float U_tot = 0.8;
  int c;
  char *tvalue = NULL;

  opterr = 0;
  while ((c = getopt (argc, argv, "t:")) != -1) {
    switch (c)
    {
      case 't':
        tvalue = optarg;
        size = atoi(tvalue);
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

  if (U_lb * size > U_tot) {
	  printf("The system cannot generate the desired configuration:\n");
	  printf("For generating a taskset of [%d] tasks with [%f], you need a U_lb of at maximum [%f]\n", size, U_tot, U_tot / size);
	  return -1;
  }

  random_task_generator(&p,
                        size,
                        U_lb,
                        U_tot,
                        1000 * 1000,
                        10 * 1000 * 1000,
                        100);

  //printf("Creating %d threads\n", size);

  print_pta_json(p, size);

  return 0;
}
