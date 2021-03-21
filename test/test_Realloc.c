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

struct block_test_values ReallocTooBig_test_values[] =
{ {0, false, 1, 0, 2, 2}
  , {1, false, 2, 0, 0, 0}
  , {2, true,  UMM_LASTBLOCK, 1, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 2, 0, 0}};

void testReallocTooBig(void) {
    void *foo = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], foo);
    TEST_ASSERT_TRUE(check_blocks(ReallocTooBig_test_values, ARRAYELEMENTCOUNT(ReallocTooBig_test_values)));

    // Realloc with a request that is too big should return NULL and leave the original memory untouched.

    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_realloc(foo,UMM_BLOCK_BODY_SIZE * (SUPPORT_UMM_MALLOC_BLOCKS - 2))));
    TEST_ASSERT_TRUE(check_blocks(ReallocTooBig_test_values, ARRAYELEMENTCOUNT(ReallocTooBig_test_values)));
}

struct block_test_values ReallocSameSize_test_values[] =
{ {0, false, 1, 0, 2, 2}
  , {1, false, 2, 0, 0, 0}
  , {2, true,  UMM_LASTBLOCK, 1, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 2, 0, 0}};

void testReallocSameSize(void) {
    void *foo = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE / 2);

    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], foo);
    TEST_ASSERT_TRUE(check_blocks(ReallocSameSize_test_values, ARRAYELEMENTCOUNT(ReallocSameSize_test_values)));

    // Realloc with a request that is same size or block size should leave the original memory untouched.

    TEST_ASSERT_EQUAL_PTR((void *)foo, (umm_realloc(foo, UMM_FIRST_BLOCK_BODY_SIZE / 2)));
    TEST_ASSERT_TRUE(check_blocks(ReallocSameSize_test_values, ARRAYELEMENTCOUNT(ReallocSameSize_test_values)));

    // Realloc with a request that is same size or block size should leave the original memory untouched.

    TEST_ASSERT_EQUAL_PTR((void *)foo, (umm_realloc(foo, 1)));
    TEST_ASSERT_TRUE(check_blocks(ReallocSameSize_test_values, ARRAYELEMENTCOUNT(ReallocSameSize_test_values)));

    // Realloc with a request that is same size or block size should leave the original memory untouched.

    TEST_ASSERT_EQUAL_PTR((void *)foo, (umm_realloc(foo, UMM_FIRST_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(ReallocSameSize_test_values, ARRAYELEMENTCOUNT(ReallocSameSize_test_values)));
}

struct block_test_values ReallocFree_test_values[] =
{ {0, false, 1, 0, 1, 1}
  , {1, true,  UMM_LASTBLOCK, 0, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 1, 0, 0}};

void testReallocFree(void) {
    void *foo = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE / 2);

    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], foo);

    // Realloc with a request that is 0 size should free the block

    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_realloc(foo, 0)));
    TEST_ASSERT_TRUE(check_blocks(ReallocFree_test_values, ARRAYELEMENTCOUNT(ReallocFree_test_values)));
}

struct block_test_values ReallocFreeRealloc_test_values[] =
{ {0, false, 1, 0, 2, 2}
  , {1, false, 2, 0, 0, 0}
  , {2, true,  UMM_LASTBLOCK, 1, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 2, 0, 0}};

void testReallocFreeRealloc(void) {
    void *foo = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE / 2);

    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], foo);
    TEST_ASSERT_TRUE(check_blocks(ReallocFreeRealloc_test_values, ARRAYELEMENTCOUNT(ReallocFreeRealloc_test_values)));

    // Realloc with a request that is 0 size should free the block

    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_realloc(foo, 0)));

    // Realloc with a request that is same size or block size should leave the original memory untouched.

    TEST_ASSERT_EQUAL_PTR((void *)foo, (umm_realloc(NULL, UMM_FIRST_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(ReallocSameSize_test_values, ARRAYELEMENTCOUNT(ReallocSameSize_test_values)));
}

struct block_test_values ReallocAssimilateUpExact[] =
{ {0, false, 1, 0, 5, 5}
  , {1, false, 2, 0, 0, 0}
  , {2, false, 4, 1, 0, 0}
  , {4, false, 5, 2, 0, 0}
  , {5, true,  UMM_LASTBLOCK, 4, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 5, 0, 0}};

void testReallocAssimilateUpExact(void) {
    void *mem0 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem1 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem2 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem3 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], mem0);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   2][UMM_BLOCK_HEADER_SIZE], mem1);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   3][UMM_BLOCK_HEADER_SIZE], mem2);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   4][UMM_BLOCK_HEADER_SIZE], mem3);

    // Free the last block and then realloc the middle block to use it

    umm_free(mem2);

    TEST_ASSERT_EQUAL_PTR((void *)mem1, (umm_realloc(mem1, UMM_FIRST_BLOCK_BODY_SIZE + 1)));
    TEST_ASSERT_TRUE(check_blocks(ReallocAssimilateUpExact, ARRAYELEMENTCOUNT(ReallocAssimilateUpExact)));
}

struct block_test_values ReallocAssimilateDown[] =
{ {0, false, 1, 0, 4, 4}
  , {1, false, 3, 0, 0, 0}
  , {3, false, 4, 1, 0, 0}
  , {4, true,  UMM_LASTBLOCK, 3, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 4, 0, 0}};

void testReallocAssimilateDown(void) {
    void *mem0 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem1 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem2 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], mem0);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   2][UMM_BLOCK_HEADER_SIZE], mem1);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   3][UMM_BLOCK_HEADER_SIZE], mem2);

    // Free the first block and then realloc the middle block to use it

    umm_free(mem0);

    TEST_ASSERT_EQUAL_PTR((void *)mem0, (umm_realloc(mem1, UMM_FIRST_BLOCK_BODY_SIZE + 1)));
    TEST_ASSERT_TRUE(check_blocks(ReallocAssimilateDown, ARRAYELEMENTCOUNT(ReallocAssimilateDown)));
}

struct block_test_values ReallocAssimilateUpDown[] =
{ {0, false, 1, 0, 5, 5}
  , {1, false, 4, 0, 0, 0}
  , {4, false, 5, 1, 0, 0}
  , {5, true,  UMM_LASTBLOCK, 4, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 5, 0, 0}};

void testReallocAssimilateUpDown(void) {
    void *mem0 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem1 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem2 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem3 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], mem0);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   2][UMM_BLOCK_HEADER_SIZE], mem1);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   3][UMM_BLOCK_HEADER_SIZE], mem2);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   4][UMM_BLOCK_HEADER_SIZE], mem3);

    // Free the first and third block and then realloc the middle block to use both

    umm_free(mem0);
    umm_free(mem2);

    TEST_ASSERT_EQUAL_PTR((void *)mem0, (umm_realloc(mem1, UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE * 2)));
    TEST_ASSERT_TRUE(check_blocks(ReallocAssimilateUpDown, ARRAYELEMENTCOUNT(ReallocAssimilateUpDown)));
}

struct block_test_values ReallocAssimilateForceDown[] =
{ {0, false, 1, 0, 3, 7}
  , {1, false, 3, 0, 0, 0}
  , {3, true,  6, 1, 7, 0}
  , {6, false, 7, 3, 0, 0}
  , {7, true,  UMM_LASTBLOCK, 6, 0, 3}
  , {UMM_LASTBLOCK, false, 0, 7, 0, 0}};

void testReallocAssimilateForceDown(void) {
    void *mem0 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE);
    void *mem1 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem2 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE);
    void *mem3 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], mem0);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   3][UMM_BLOCK_HEADER_SIZE], mem1);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   4][UMM_BLOCK_HEADER_SIZE], mem2);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   6][UMM_BLOCK_HEADER_SIZE], mem3);

    // Free the first and third blocks and then realloc the
    // middle block to use the freed space - force down

    umm_free(mem0);
    umm_free(mem2);

    TEST_ASSERT_EQUAL_PTR((void *)mem0, (umm_realloc(mem1, UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(ReallocAssimilateForceDown, ARRAYELEMENTCOUNT(ReallocAssimilateForceDown)));
}

struct block_test_values ReallocNewBlock[] =
{ {0, false, 1, 0, 2, 6}
  , {1, false, 2, 0, 0, 0}
  , {2, true, 3, 1, 6, 0}
  , {3, false, 4, 2, 0, 0}
  , {4, false, 6, 3, 0, 0}
  , {6, true,  UMM_LASTBLOCK, 4, 0, 2}
  , {UMM_LASTBLOCK, false, 0, 6, 0, 0}};

void testReallocNewBlock(void) {
    void *mem0 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem1 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem2 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], mem0);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   2][UMM_BLOCK_HEADER_SIZE], mem1);
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   3][UMM_BLOCK_HEADER_SIZE], mem2);

    /* Realloc the middle block - should need a totally new block */

    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[   4][UMM_BLOCK_HEADER_SIZE], (umm_realloc(mem1, UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(ReallocNewBlock, ARRAYELEMENTCOUNT(ReallocNewBlock)));
}
