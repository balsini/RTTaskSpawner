#ifndef __RANDOM_TASK_GENERATOR_HPP__
#define __RANDOM_TASK_GENERATOR_HPP__

#include "../periodicity.h"

void random_task_generator(periodic_task_attr *p[],
                           unsigned int size,
                           float U_lb,
                           float U_tot,
                           unsigned int T_min,
                           unsigned int T_max,
                           unsigned int jobs);

#endif
