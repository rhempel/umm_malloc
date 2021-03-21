#include "unity.h"

#include <umm_malloc_cfg.h>
#include <umm_malloc.h>

#include <support_umm_malloc.h>

/* Use the default DBGLOG_LEVEL and DBGLOG_FUNCTION */

#define DBGLOG_LEVEL 0

#ifdef DBGLOG_ENABLE
    #include "dbglog/dbglog.h"
#endif

void setUp(void) {
    umm_init();
    umm_critical_depth = 0;
    umm_max_critical_depth = 0;
}

void tearDown(void) {
    TEST_ASSERT_LESS_OR_EQUAL(1, umm_max_critical_depth);
}

struct block_test_values MultiMallocManySmall_test_values[] =
{ {0, false, 1, 0,  6,  6}
  , {1, false, 2, 0,  0,  0}
  , {2, false, 3, 1,  0,  0}
  , {3, false, 4, 2,  0,  0}
  , {4, false, 5, 3,  0,  0}
  , {5, false, 6, 4,  0,  0}
  , {6, true,  UMM_LASTBLOCK, 5,  0,  0}
  , {UMM_LASTBLOCK, false, 0, 6,  0,  0}};

void testMultiMallocManySmall(void) {
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE));
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[2][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE));
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[3][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE));
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[4][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE));
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[5][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE));

    TEST_ASSERT_TRUE(check_blocks(MultiMallocManySmall_test_values, ARRAYELEMENTCOUNT(MultiMallocManySmall_test_values)));
}

struct block_test_values MultiMallocManyMed_test_values[] =
{ {   0, false,    1,    0,  2501,  2501}
  , {   1, false,  501,    0,  0,  0}
  , { 501, false, 1001,    1,  0,  0}
  , {1001, false, 1501,  501,  0,  0}
  , {1501, false, 2001, 1001,  0,  0}
  , {2001, false, 2501, 1501,  0,  0}
  , {2501, true,  UMM_LASTBLOCK, 2001,  0,  0}
  , {UMM_LASTBLOCK, false, 0, 2501,  0,  0}};

void testMultiMallocManyMed(void) {
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_BLOCK_BODY_SIZE * 500 - UMM_FIRST_BLOCK_BODY_SIZE));
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[ 501][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_BLOCK_BODY_SIZE * 500 - UMM_FIRST_BLOCK_BODY_SIZE));
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1001][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_BLOCK_BODY_SIZE * 500 - UMM_FIRST_BLOCK_BODY_SIZE));
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1501][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_BLOCK_BODY_SIZE * 500 - UMM_FIRST_BLOCK_BODY_SIZE));
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[2001][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_BLOCK_BODY_SIZE * 500 - UMM_FIRST_BLOCK_BODY_SIZE));

    TEST_ASSERT_TRUE(check_blocks(MultiMallocManyMed_test_values, ARRAYELEMENTCOUNT(MultiMallocManyMed_test_values)));
}

struct block_test_values MultiMallocManyLarge_test_values[] =
{ {    0, false, 1,    0,  7501,  7501}
  , {    1, false, 2501,    0,     0,     0}
  , { 2501, false, 5001,    1,     0,     0}
  , { 5001, false, 7501, 2501,     0,     0}
  , { 7501, true,  UMM_LASTBLOCK, 5001,     0,     0}
  , {UMM_LASTBLOCK, false, 0, 7501,     0,     0}};

void testMultiMallocManyLarge(void) {
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_BLOCK_BODY_SIZE * 2500 - UMM_FIRST_BLOCK_BODY_SIZE));
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[2501][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_BLOCK_BODY_SIZE * 2500 - UMM_FIRST_BLOCK_BODY_SIZE));
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[5001][UMM_BLOCK_HEADER_SIZE], umm_malloc(UMM_BLOCK_BODY_SIZE * 2500 - UMM_FIRST_BLOCK_BODY_SIZE));
    TEST_ASSERT_EQUAL_PTR((void *)NULL, umm_malloc(UMM_BLOCK_BODY_SIZE * 2500 - UMM_FIRST_BLOCK_BODY_SIZE));

    TEST_ASSERT_TRUE(check_blocks(MultiMallocManyLarge_test_values, ARRAYELEMENTCOUNT(MultiMallocManyLarge_test_values)));
}


