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

TEST_GROUP(TooBigMalloc)
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

static struct block_test_values Initialization_test_values[] = {
 {0, false, 1, 0, 1, 1}
, {1, true,  UMM_LASTBLOCK, 0, 0, 0}
, {UMM_LASTBLOCK, false, 0, 1, 0, 0}
};

TEST(TooBigMalloc, testHeapFirstMallocMaxHeapBlocks)
{
    POINTERS_EQUAL((void *)NULL, (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS) * UMM_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

TEST(TooBigMalloc, testHeapFirstMallocMaxHeapBlocksMinus1)
{
    POINTERS_EQUAL((void *)NULL, (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS-1) * UMM_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

TEST(TooBigMalloc, testHeapFirstMallocMaxHeapBlocksMinus2)
{
    POINTERS_EQUAL((void *)NULL, (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS-2) * UMM_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

TEST(TooBigMalloc, testHeapFirstMallocInsaneBigMalloc)
{
    POINTERS_EQUAL((void *)NULL, (umm_malloc((INT16_MAX + 1) * UMM_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

struct block_test_values MallocMaxHeapBlocksMinus3_test_values[] = {
 {0, false, 1, 0, 0, 0}
, {1, false, UMM_LASTBLOCK, 0, 0, 0}
, {UMM_LASTBLOCK, false, 0, 1, 0, 0}
};

TEST(TooBigMalloc, testHeapFirstMallocMaxHeapBlocksMinus3)
{
    // This is a fairly complex test, so we will break it down
    //
    // First allocate the largest block possible ...
    //
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS-3) * UMM_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksMinus3_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksMinus3_test_values)));

    // Then free it ...
    umm_free((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE]);
    CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

struct block_test_values MallocMaxHeapBlocksBig_test_values[] = {
 {0, false, 1, 0, UMM_LASTBLOCK-1, UMM_LASTBLOCK-1}
, {1, false, UMM_LASTBLOCK-1, 0, 0, 0}
, {UMM_LASTBLOCK-1, true, UMM_LASTBLOCK, 1, 0, 0}
, {UMM_LASTBLOCK, false, 0, UMM_LASTBLOCK-1, 0, 0}
};

struct block_test_values MallocMaxHeapBlocksBigThenSmall_test_values[] = {
 {0, false, 1, 0, 0, 0}
, {1, false, UMM_LASTBLOCK-1, 0, 0, 0}
, {UMM_LASTBLOCK-1, 0, UMM_LASTBLOCK, 1, 0, 0}
, {UMM_LASTBLOCK, false, 0, UMM_LASTBLOCK-1, 0, 0}
};

TEST(TooBigMalloc, testHeapTooBigMalloc_BigThenSmallMax)
{
    // This is a fairly complex test, so we will break it down
    //
    // First allocate the largest block possible that leaves exactly one block free ...
    //
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS-4) * UMM_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksBig_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksBig_test_values)));

    // Then allocate exactly one more block ...
    //
    POINTERS_EQUAL((void *)&test_umm_heap[UMM_LASTBLOCK-1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(1)));
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksBigThenSmall_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksBigThenSmall_test_values)));

    // Then allocate exactly one more block ... which should fail
    //
    POINTERS_EQUAL((void *)NULL, (umm_malloc(1)));
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksBigThenSmall_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksBigThenSmall_test_values)));

    // Then free the last block ...
    umm_free((void *)&test_umm_heap[UMM_LASTBLOCK-1][UMM_BLOCK_HEADER_SIZE]);
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksBig_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksBig_test_values)));

    // Then free the first block ... which should get us back to the initialized state
    umm_free((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE]);
    CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

struct block_test_values MallocMaxHeapBlocksSmall_test_values[] = {
  {0, false, 1, 0, 2, 2}
, {1, false, 2, 0, 0, 0}
, {2, true,  UMM_LASTBLOCK, 1, 0, 0}
, {UMM_LASTBLOCK, false, 0, 2, 0, 0}
};

struct block_test_values MallocMaxHeapBlocksSmallThenBig_test_values[] = {
 {0, false, 1, 0, 0, 0}
, {1, false, 2, 0, 0, 0}
, {2, 0, UMM_LASTBLOCK, 1, 0, 0}
, {UMM_LASTBLOCK, false, 0, 2, 0, 0}
};

TEST(TooBigMalloc, testHeapTooBigMalloc_SmallThenBigMax)
{
    // This is a fairly complex test, so we will break it down
    //
    // First allocate the smallest block possible that leaves a large block free ...
    //
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(1)));
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksSmall_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmall_test_values)));

    // Then allocate the largest possible leftover block ...
    //
    POINTERS_EQUAL((void *)&test_umm_heap[2][UMM_BLOCK_HEADER_SIZE], (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS-4) * UMM_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksSmallThenBig_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmallThenBig_test_values)));

    // Then allocate exactly one more block ... which should fail
    //
    POINTERS_EQUAL((void *)NULL, (umm_malloc(1)));
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksSmallThenBig_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmallThenBig_test_values)));

    // Then free the large block ...
    umm_free((void *)&test_umm_heap[2][UMM_BLOCK_HEADER_SIZE]);
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksSmall_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmall_test_values)));

    // Then free the small block ... which should get us back to the initialized state
    umm_free((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE]);
    CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

struct block_test_values MallocMaxHeapBlocksSmallThenBig_ReverseFreetest_values[] = {
  {0, false, 1, 0, 1, 1}
, {1, true, 2, 0, 0, 0}
, {2, 0, UMM_LASTBLOCK, 1, 0, 0}
, {UMM_LASTBLOCK, false, 0, 2, 0, 0}
};

TEST(TooBigMalloc, testHeapTooBigMalloc_SmallThenBigMax_ReverseFree)
{
    // This is a fairly complex test, so we will break it down
    //
    // First allocate the smallest block possible that leaves a large block free ...
    //
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(1)));
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksSmall_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmall_test_values)));

    // Then allocate the largest possible leftover block ...
    //
    POINTERS_EQUAL((void *)&test_umm_heap[2][UMM_BLOCK_HEADER_SIZE], (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS-4) * UMM_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksSmallThenBig_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmallThenBig_test_values)));

    // Then allocate exactly one more block ... which should fail
    //
    POINTERS_EQUAL((void *)NULL, (umm_malloc(1)));
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksSmallThenBig_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmallThenBig_test_values)));

    // Then free the small block ...
    umm_free((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE]);
    CHECK_TRUE(check_blocks(MallocMaxHeapBlocksSmallThenBig_ReverseFreetest_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmallThenBig_ReverseFreetest_values)));

    // Then free the large block ...
    umm_free((void *)&test_umm_heap[2][UMM_BLOCK_HEADER_SIZE]);
    CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

TEST(TooBigMalloc, testHeapFirstMallocMaxNumBlocks_Minus1)
{
    POINTERS_EQUAL((void *)NULL, (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + ((INT16_MAX-2) * 500))));
    CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

TEST(TooBigMalloc, testHeapFirstMallocMaxNumBlocks)
{
    POINTERS_EQUAL((void *)NULL, (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + ((INT16_MAX-1) * 500))));
    CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

TEST(TooBigMalloc, testHeapFirstMallocMaxNumBlocks_Plus1)
{
    POINTERS_EQUAL((void *)NULL, (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + ((INT16_MAX-0) * 500))));
    CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}
