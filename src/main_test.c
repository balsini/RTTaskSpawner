#include <stdio.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <time.h>

#include "periodicity.h"

int init_suite_periodicity(void) { return 0; }
int clean_suite_periodicity(void) { return 0; }

void test_periodicity_time_add(void)
{
  struct timespec t1, t2, tr;

  t1.tv_sec = 1;
  t1.tv_nsec = 2;

  t2.tv_sec = 3;
  t2.tv_nsec = 4;

  tr.tv_sec = 4;
  tr.tv_nsec = 6;

  CU_ASSERT_EQUAL(time_add(&t1, &t2).tv_sec, tr.tv_sec);
  CU_ASSERT_EQUAL(time_add(&t1, &t2).tv_nsec, tr.tv_nsec);

  t1.tv_sec = 1;
  t1.tv_nsec = 600 * 1000 * 1000;

  t2.tv_sec = 3;
  t2.tv_nsec = 600 * 1000 * 1000;

  tr.tv_sec = 5;
  tr.tv_nsec = 200 * 1000 * 1000;

  CU_ASSERT_EQUAL(time_add(&t1, &t2).tv_sec, tr.tv_sec);
  CU_ASSERT_EQUAL(time_add(&t1, &t2).tv_nsec, tr.tv_nsec);
}

void test_periodicity_time_add_ns(void)
{
  struct timespec t1, t2, tr;
  long int ns;

  t1.tv_sec = t2.tv_sec = 1;
  t1.tv_nsec = t2.tv_nsec = 2;

  ns = 10;

  time_add_ns(&t1, ns);

  CU_ASSERT_EQUAL(t1.tv_sec, t2.tv_sec);
  CU_ASSERT_EQUAL(t1.tv_nsec, t2.tv_nsec + ns);

  t1.tv_sec = t2.tv_sec = 1;
  t1.tv_nsec = t2.tv_nsec = 600 * 1000 * 1000;

  ns = 600 * 1000 * 1000;

  time_add_ns(&t1, ns);

  CU_ASSERT_EQUAL(t1.tv_sec, t2.tv_sec + 1);
  CU_ASSERT_EQUAL(t1.tv_nsec, 200 * 1000 * 1000);
}

int main()
{
  printf("-------------------\n");
  printf("------TESTING------\n");
  printf("-------------------\n");

  CU_pSuite pSuite = NULL;

  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  /* add a suite to the registry */
  pSuite = CU_add_suite("Suite_Periodicity", init_suite_periodicity, clean_suite_periodicity);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "test_periodicity_time_add", test_periodicity_time_add)) ||
      (NULL == CU_add_test(pSuite, "test_periodicity_time_add_ns", test_periodicity_time_add_ns)))
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Run all tests using the basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  printf("\n");
  CU_basic_show_failures(CU_get_failure_list());
  printf("\n\n");

  /* Clean up registry and return */
  CU_cleanup_registry();
  return CU_get_error();

  return 0;
}
