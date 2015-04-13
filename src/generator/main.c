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
  unsigned int size = 10;
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
