#include <umm_malloc.h>
#include <support_umm_malloc.h>

///* Use the default DBGLOG_LEVEL and DBGLOG_FUNCTION */
//
//#define DBGLOG_LEVEL 0
//
//#ifdef DBGLOG_ENABLE
//    #include "dbglog/dbglog.h"
//#endif

#include "CppUTest/TestHarness.h"

TEST_GROUP(Realloc)
{
    void setup(void) {
        umm_init();
        umm_critical_depth = 0;
        umm_max_critical_depth = 0;
    }

    void teardown(void) {
        CHECK_COMPARE(1, >= ,umm_max_critical_depth);
    }
};

struct block_test_values ReallocTooBig_test_values[] = {
  {0, false, 1, 0, 2, 2}
, {1, false, 2, 0, 0, 0}
, {2, true,  UMM_LASTBLOCK, 1, 0, 0}
, {UMM_LASTBLOCK, false, 0, 2, 0, 0}
};

TEST(Realloc, testReallocTooBig)
{
    void *foo = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], foo);
    CHECK_TRUE(check_blocks(ReallocTooBig_test_values, ARRAYELEMENTCOUNT(ReallocTooBig_test_values)));

    // Realloc with a request that is too big should return NULL and leave the original memory untouched.

    POINTERS_EQUAL((void *)NULL, (umm_realloc(foo,UMM_BLOCK_BODY_SIZE * (SUPPORT_UMM_MALLOC_BLOCKS - 2))));
    CHECK_TRUE(check_blocks(ReallocTooBig_test_values, ARRAYELEMENTCOUNT(ReallocTooBig_test_values)));
}

struct block_test_values ReallocSameSize_test_values[] = {
  {0, false, 1, 0, 2, 2}
, {1, false, 2, 0, 0, 0}
, {2, true,  UMM_LASTBLOCK, 1, 0, 0}
, {UMM_LASTBLOCK, false, 0, 2, 0, 0}
};

TEST(Realloc, testReallocSameSize)
{
    void *foo = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE / 2);

    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], foo);
    CHECK_TRUE(check_blocks(ReallocSameSize_test_values, ARRAYELEMENTCOUNT(ReallocSameSize_test_values)));

    // Realloc with a request that is same size or block size should leave the original memory untouched.

    POINTERS_EQUAL((void *)foo, (umm_realloc(foo, UMM_FIRST_BLOCK_BODY_SIZE / 2)));
    CHECK_TRUE(check_blocks(ReallocSameSize_test_values, ARRAYELEMENTCOUNT(ReallocSameSize_test_values)));

    // Realloc with a request that is same size or block size should leave the original memory untouched.

    POINTERS_EQUAL((void *)foo, (umm_realloc(foo, 1)));
    CHECK_TRUE(check_blocks(ReallocSameSize_test_values, ARRAYELEMENTCOUNT(ReallocSameSize_test_values)));

    // Realloc with a request that is same size or block size should leave the original memory untouched.

    POINTERS_EQUAL((void *)foo, (umm_realloc(foo, UMM_FIRST_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(ReallocSameSize_test_values, ARRAYELEMENTCOUNT(ReallocSameSize_test_values)));
}

struct block_test_values ReallocFree_test_values[] = {
  {0, false, 1, 0, 1, 1}
, {1, true,  UMM_LASTBLOCK, 0, 0, 0}
, {UMM_LASTBLOCK, false, 0, 1, 0, 0}
};

TEST(Realloc, testReallocFree)
{
    void *foo = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE / 2);

    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], foo);

    // Realloc with a request that is 0 size should free the block

    POINTERS_EQUAL((void *)NULL, (umm_realloc(foo, 0)));
    CHECK_TRUE(check_blocks(ReallocFree_test_values, ARRAYELEMENTCOUNT(ReallocFree_test_values)));
}

struct block_test_values ReallocFreeRealloc_test_values[] = {
  {0, false, 1, 0, 2, 2}
, {1, false, 2, 0, 0, 0}
, {2, true,  UMM_LASTBLOCK, 1, 0, 0}
, {UMM_LASTBLOCK, false, 0, 2, 0, 0}
};

TEST(Realloc, testReallocFreeRealloc)
{
    void *foo = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE / 2);

    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], foo);
    CHECK_TRUE(check_blocks(ReallocFreeRealloc_test_values, ARRAYELEMENTCOUNT(ReallocFreeRealloc_test_values)));

    // Realloc with a request that is 0 size should free the block

    POINTERS_EQUAL((void *)NULL, (umm_realloc(foo, 0)));

    // Realloc with a request that is same size or block size should leave the original memory untouched.

    POINTERS_EQUAL((void *)foo, (umm_realloc(NULL, UMM_FIRST_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(ReallocSameSize_test_values, ARRAYELEMENTCOUNT(ReallocSameSize_test_values)));
}

struct block_test_values ReallocAssimilateUpExact[] = {
  {0, false, 1, 0, 5, 5}
, {1, false, 2, 0, 0, 0}
, {2, false, 4, 1, 0, 0}
, {4, false, 5, 2, 0, 0}
, {5, true,  UMM_LASTBLOCK, 4, 0, 0}
, {UMM_LASTBLOCK, false, 0, 5, 0, 0}
};

TEST(Realloc, testReallocAssimilateUpExact)
{
    void *mem0 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem1 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem2 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem3 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    POINTERS_EQUAL((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], mem0);
    POINTERS_EQUAL((void *)&test_umm_heap[   2][UMM_BLOCK_HEADER_SIZE], mem1);
    POINTERS_EQUAL((void *)&test_umm_heap[   3][UMM_BLOCK_HEADER_SIZE], mem2);
    POINTERS_EQUAL((void *)&test_umm_heap[   4][UMM_BLOCK_HEADER_SIZE], mem3);

    // Free a middle block and then realloc the first block to use it

    umm_free(mem2);

    POINTERS_EQUAL((void *)mem1, (umm_realloc(mem1, UMM_FIRST_BLOCK_BODY_SIZE + 1)));
    CHECK_TRUE(check_blocks(ReallocAssimilateUpExact, ARRAYELEMENTCOUNT(ReallocAssimilateUpExact)));
}

struct block_test_values ReallocAssimilateUp[] = {
  {0, false, 1, 0, 4, 6}
, {1, false, 2, 0, 0, 0}
, {2, false, 4, 1, 0, 0}
, {4, true,  5, 2, 6, 0}
, {5, false, 6, 4, 0, 0}
, {6, true,  UMM_LASTBLOCK, 5, 0, 4}
, {UMM_LASTBLOCK, false, 0, 6, 0, 0}
};

TEST(Realloc, testReallocAssimilateUp)
{
    void *mem0 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem1 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem2 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem3 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem4 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    POINTERS_EQUAL((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], mem0);
    POINTERS_EQUAL((void *)&test_umm_heap[   2][UMM_BLOCK_HEADER_SIZE], mem1);
    POINTERS_EQUAL((void *)&test_umm_heap[   3][UMM_BLOCK_HEADER_SIZE], mem2);
    POINTERS_EQUAL((void *)&test_umm_heap[   4][UMM_BLOCK_HEADER_SIZE], mem3);
    POINTERS_EQUAL((void *)&test_umm_heap[   5][UMM_BLOCK_HEADER_SIZE], mem4);

    // Free two middle block and then realloc the first block to use one of them

    umm_free(mem2);
    umm_free(mem3);

    POINTERS_EQUAL((void *)mem1, (umm_realloc(mem1, UMM_FIRST_BLOCK_BODY_SIZE + 8)));
    CHECK_TRUE(check_blocks(ReallocAssimilateUp, ARRAYELEMENTCOUNT(ReallocAssimilateUp)));
}

struct block_test_values ReallocAssimilateDown[] = {
  {0, false, 1, 0, 4, 4}
, {1, false, 3, 0, 0, 0}
, {3, false, 4, 1, 0, 0}
, {4, true,  UMM_LASTBLOCK, 3, 0, 0}
, {UMM_LASTBLOCK, false, 0, 4, 0, 0}
};

TEST(Realloc, testReallocAssimilateDown)
{
    void *mem0 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem1 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem2 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    POINTERS_EQUAL((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], mem0);
    POINTERS_EQUAL((void *)&test_umm_heap[   2][UMM_BLOCK_HEADER_SIZE], mem1);
    POINTERS_EQUAL((void *)&test_umm_heap[   3][UMM_BLOCK_HEADER_SIZE], mem2);

    // Free the first block and then realloc the middle block to use it

    umm_free(mem0);

    POINTERS_EQUAL((void *)mem0, (umm_realloc(mem1, UMM_FIRST_BLOCK_BODY_SIZE + 1)));
    CHECK_TRUE(check_blocks(ReallocAssimilateDown, ARRAYELEMENTCOUNT(ReallocAssimilateDown)));
}

struct block_test_values ReallocAssimilateUpDown[] = {
  {0, false, 1, 0, 5, 5}
, {1, false, 4, 0, 0, 0}
, {4, false, 5, 1, 0, 0}
, {5, true,  UMM_LASTBLOCK, 4, 0, 0}
, {UMM_LASTBLOCK, false, 0, 5, 0, 0}
};

TEST(Realloc, testReallocAssimilateUpDown)
{
    void *mem0 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem1 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem2 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem3 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    POINTERS_EQUAL((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], mem0);
    POINTERS_EQUAL((void *)&test_umm_heap[   2][UMM_BLOCK_HEADER_SIZE], mem1);
    POINTERS_EQUAL((void *)&test_umm_heap[   3][UMM_BLOCK_HEADER_SIZE], mem2);
    POINTERS_EQUAL((void *)&test_umm_heap[   4][UMM_BLOCK_HEADER_SIZE], mem3);

    // Free the first and third block and then realloc the middle block to use both

    umm_free(mem0);
    umm_free(mem2);

    POINTERS_EQUAL((void *)mem0, (umm_realloc(mem1, UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE * 2)));
    CHECK_TRUE(check_blocks(ReallocAssimilateUpDown, ARRAYELEMENTCOUNT(ReallocAssimilateUpDown)));
}

struct block_test_values ReallocAssimilateForceDown[] = {
  {0, false, 1, 0, 3, 7}
, {1, false, 3, 0, 0, 0}
 , {3, true,  6, 1, 7, 0}
 , {6, false, 7, 3, 0, 0}
 , {7, true,  UMM_LASTBLOCK, 6, 0, 3}
 , {UMM_LASTBLOCK, false, 0, 7, 0, 0}
 };

TEST(Realloc, testReallocAssimilateForceDown)
{
    void *mem0 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE);
    void *mem1 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem2 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE);
    void *mem3 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    POINTERS_EQUAL((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], mem0);
    POINTERS_EQUAL((void *)&test_umm_heap[   3][UMM_BLOCK_HEADER_SIZE], mem1);
    POINTERS_EQUAL((void *)&test_umm_heap[   4][UMM_BLOCK_HEADER_SIZE], mem2);
    POINTERS_EQUAL((void *)&test_umm_heap[   6][UMM_BLOCK_HEADER_SIZE], mem3);

    // Free the first and third blocks and then realloc the
    // middle block to use the freed space - force down

    umm_free(mem0);
    umm_free(mem2);

    POINTERS_EQUAL((void *)mem0, (umm_realloc(mem1, UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(ReallocAssimilateForceDown, ARRAYELEMENTCOUNT(ReallocAssimilateForceDown)));
}

struct block_test_values ReallocNewBlock[] = {
 {0, false, 1, 0, 2, 6}
, {1, false, 2, 0, 0, 0}
, {2, true, 3, 1, 6, 0}
, {3, false, 4, 2, 0, 0}
, {4, false, 6, 3, 0, 0}
, {6, true,  UMM_LASTBLOCK, 4, 0, 2}
, {UMM_LASTBLOCK, false, 0, 6, 0, 0}
};

TEST(Realloc, testReallocNewBlock)
{
    void *mem0 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem1 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    void *mem2 = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);

    POINTERS_EQUAL((void *)&test_umm_heap[   1][UMM_BLOCK_HEADER_SIZE], mem0);
    POINTERS_EQUAL((void *)&test_umm_heap[   2][UMM_BLOCK_HEADER_SIZE], mem1);
    POINTERS_EQUAL((void *)&test_umm_heap[   3][UMM_BLOCK_HEADER_SIZE], mem2);

    /* Realloc the middle block - should need a totally new block */

    POINTERS_EQUAL((void *)&test_umm_heap[   4][UMM_BLOCK_HEADER_SIZE], (umm_realloc(mem1, UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(ReallocNewBlock, ARRAYELEMENTCOUNT(ReallocNewBlock)));
}
