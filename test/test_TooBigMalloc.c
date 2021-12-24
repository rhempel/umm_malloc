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

struct block_test_values Initialization_test_values[] =
{ {0, false, 1, 0, 1, 1}
  , {1, true,  UMM_LASTBLOCK, 0, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 1, 0, 0}};

#if 0
void testHeapInitialization(void) {
    DBGLOG_FORCE(true, "support heapsize %08x\n", SUPPORT_UMM_MALLOC_HEAP_SIZE);

    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}
#endif

void testHeapFirstMallocMaxHeapBlocks(void) {
    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS) * UMM_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

void testHeapFirstMallocMaxHeapBlocksMinus1(void) {
    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS-1) * UMM_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

void testHeapFirstMallocMaxHeapBlocksMinus2(void) {
    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS-2) * UMM_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

struct block_test_values MallocMaxHeapBlocksMinus3_test_values[] =
{ {0, false, 1, 0, 0, 0}
  , {1, false, UMM_LASTBLOCK, 0, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 1, 0, 0}};

void testHeapFirstMallocMaxHeapBlocksMinus3(void) {
    // This is a fairly complex test, so we will break it down
    //
    // First allocate the largest block possible ...
    //
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS-3) * UMM_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksMinus3_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksMinus3_test_values)));

    // Then free it ...
    umm_free((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE]);
    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

struct block_test_values MallocMaxHeapBlocksBig_test_values[] =
{ {0, false, 1, 0, UMM_LASTBLOCK-1, UMM_LASTBLOCK-1}
  , {1, false, UMM_LASTBLOCK-1, 0, 0, 0}
  , {UMM_LASTBLOCK-1, true, UMM_LASTBLOCK, 1, 0, 0}
  , {UMM_LASTBLOCK, false, 0, UMM_LASTBLOCK-1, 0, 0}};

struct block_test_values MallocMaxHeapBlocksBigThenSmall_test_values[] =
{ {0, false, 1, 0, 0, 0}
  , {1, false, UMM_LASTBLOCK-1, 0, 0, 0}
  , {UMM_LASTBLOCK-1, 0, UMM_LASTBLOCK, 1, 0, 0}
  , {UMM_LASTBLOCK, false, 0, UMM_LASTBLOCK-1, 0, 0}};

void testHeapTooBigMalloc_BigThenSmallMax(void) {
    // This is a fairly complex test, so we will break it down
    //
    // First allocate the largest block possible that leaves exactly one block free ...
    //
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS-4) * UMM_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksBig_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksBig_test_values)));

    // Then allocate exactly one more block ...
    //
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[UMM_LASTBLOCK-1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(1)));
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksBigThenSmall_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksBigThenSmall_test_values)));

    // Then allocate exactly one more block ... which should fail
    //
    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_malloc(1)));
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksBigThenSmall_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksBigThenSmall_test_values)));

    // Then free the last block ...
    umm_free((void *)&test_umm_heap[UMM_LASTBLOCK-1][UMM_BLOCK_HEADER_SIZE]);
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksBig_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksBig_test_values)));

    // Then free the first block ... which should get us back to the initialized state
    umm_free((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE]);
    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

struct block_test_values MallocMaxHeapBlocksSmall_test_values[] =
{ {0, false, 1, 0, 2, 2}
  , {1, false, 2, 0, 0, 0}
  , {2, true,  UMM_LASTBLOCK, 1, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 2, 0, 0}};

struct block_test_values MallocMaxHeapBlocksSmallThenBig_test_values[] =
{ {0, false, 1, 0, 0, 0}
  , {1, false, 2, 0, 0, 0}
  , {2, 0, UMM_LASTBLOCK, 1, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 2, 0, 0}};

void testHeapTooBigMalloc_SmallThenBigMax(void) {
    // This is a fairly complex test, so we will break it down
    //
    // First allocate the smallest block possible that leaves a large block free ...
    //
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(1)));
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksSmall_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmall_test_values)));

    // Then allocate the largest possible leftover block ...
    //
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[2][UMM_BLOCK_HEADER_SIZE], (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS-4) * UMM_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksSmallThenBig_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmallThenBig_test_values)));

    // Then allocate exactly one more block ... which should fail
    //
    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_malloc(1)));
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksSmallThenBig_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmallThenBig_test_values)));

    // Then free the large block ...
    umm_free((void *)&test_umm_heap[2][UMM_BLOCK_HEADER_SIZE]);
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksSmall_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmall_test_values)));

    // Then free the small block ... which should get us back to the initialized state
    umm_free((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE]);
    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

struct block_test_values MallocMaxHeapBlocksSmallThenBig_ReverseFreetest_values[] =
{ {0, false, 1, 0, 1, 1}
  , {1, true, 2, 0, 0, 0}
  , {2, 0, UMM_LASTBLOCK, 1, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 2, 0, 0}};

void testHeapTooBigMalloc_SmallThenBigMax_ReverseFree(void) {
    // This is a fairly complex test, so we will break it down
    //
    // First allocate the smallest block possible that leaves a large block free ...
    //
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(1)));
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksSmall_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmall_test_values)));

    // Then allocate the largest possible leftover block ...
    //
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[2][UMM_BLOCK_HEADER_SIZE], (umm_malloc((SUPPORT_UMM_MALLOC_BLOCKS-4) * UMM_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksSmallThenBig_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmallThenBig_test_values)));

    // Then allocate exactly one more block ... which should fail
    //
    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_malloc(1)));
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksSmallThenBig_test_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmallThenBig_test_values)));

    // Then free the small block ...
    umm_free((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE]);
    TEST_ASSERT_TRUE(check_blocks(MallocMaxHeapBlocksSmallThenBig_ReverseFreetest_values, ARRAYELEMENTCOUNT(MallocMaxHeapBlocksSmallThenBig_ReverseFreetest_values)));

    // Then free the large block ...
    umm_free((void *)&test_umm_heap[2][UMM_BLOCK_HEADER_SIZE]);
    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

void testHeapFirstMallocMaxNumBlocks_Minus1(void) {
    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + ((INT16_MAX-2) * 500))));
    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

void testHeapFirstMallocMaxNumBlocks(void) {
    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + ((INT16_MAX-1) * 500))));
    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

void testHeapFirstMallocMaxNumBlocks_Plus1(void) {
    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + ((INT16_MAX-0) * 500))));
    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

