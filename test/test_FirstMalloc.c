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

void testHeapInitialization(void) {
    DBGLOG_FORCE(true, "support heapsize %08x\n", SUPPORT_UMM_MALLOC_HEAP_SIZE);

    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

void testHeapFirstMalloc0Bytes(void) {
    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_malloc(0)));
    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

struct block_test_values MallocFirstBlock_test_values[] =
{ {0, false, 1, 0, 2, 2}
  , {1, false, 2, 0, 0, 0}
  , {2, true,  UMM_LASTBLOCK, 1, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 2, 0, 0}};

void testHeapFirstMalloc1Bytes(void) {
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(1)));
    TEST_ASSERT_TRUE(check_blocks(MallocFirstBlock_test_values, ARRAYELEMENTCOUNT(MallocFirstBlock_test_values)));
}

void testHeapFirstMalloc2Bytes(void) {
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(2)));
    TEST_ASSERT_TRUE(check_blocks(MallocFirstBlock_test_values, ARRAYELEMENTCOUNT(MallocFirstBlock_test_values)));
}

void testHeapMallocFirstBlockBodyMinusOneBytes(void) {
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE - 1)));
    TEST_ASSERT_TRUE(check_blocks(MallocFirstBlock_test_values, ARRAYELEMENTCOUNT(MallocFirstBlock_test_values)));
}

void testHeapMallocFirstBlockBodyBytes(void) {
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(MallocFirstBlock_test_values, ARRAYELEMENTCOUNT(MallocFirstBlock_test_values)));
}

struct block_test_values MallocSecondBlock_test_values[] =
{ {0, false, 1, 0, 3, 3}
  , {1, false, 3, 0, 0, 0}
  , {3, true,  UMM_LASTBLOCK, 1, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 3, 0, 0}};

void testHeapMallocFirstBlockBodyPlusOneBytes(void) {
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + 1)));
    TEST_ASSERT_TRUE(check_blocks(MallocSecondBlock_test_values, ARRAYELEMENTCOUNT(MallocSecondBlock_test_values)));
}

void testHeapMallocFirstBlockBodyPlusFullBlockBytes(void) {
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE)));
    TEST_ASSERT_TRUE(check_blocks(MallocSecondBlock_test_values, ARRAYELEMENTCOUNT(MallocSecondBlock_test_values)));
}

struct block_test_values MallocThirdBlock_test_values[] =
{ {0, false, 1, 0, 4, 4}
  , {1, false, 4, 0, 0, 0}
  , {4, true,  UMM_LASTBLOCK, 1, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 4, 0, 0}};

void testHeapMallocFirstAndSecondBlockBodyPlusOneBytes(void) {
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE + 1)));
    TEST_ASSERT_TRUE(check_blocks(MallocThirdBlock_test_values, ARRAYELEMENTCOUNT(MallocThirdBlock_test_values)));
}

struct block_test_values MallocFirstBlockBodyPlus500Blocks_test_values[] =
{ {0, false, 1, 0, 502, 502}
  , {1, false, 502, 0,   0,   0}
  , {502, true,  UMM_LASTBLOCK, 1,   0,   0}
  , {UMM_LASTBLOCK, false, 0, 502, 0,   0}};

void testHeapMallocFirstBlockBodyPlus500Blocks(void) {
    // Note that this test will actually allocate 510 blocks as the first
    // block contains the prev/next pointer pair.
    //
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE * 500)));
    TEST_ASSERT_TRUE(check_blocks(MallocFirstBlockBodyPlus500Blocks_test_values, ARRAYELEMENTCOUNT(MallocFirstBlockBodyPlus500Blocks_test_values)));
}

struct block_test_values MallocAllBlocks_test_values[] =
{ {0, false, 1, 0, 0, 0}
  , {1, false, UMM_LASTBLOCK, 0, 0, 0}
  , {UMM_LASTBLOCK, false, 0, 1, 0, 0}};

void testHeapAllBlocks(void) {
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE * (SUPPORT_UMM_MALLOC_BLOCKS - 3))));
    TEST_ASSERT_TRUE(check_blocks(MallocAllBlocks_test_values, ARRAYELEMENTCOUNT(MallocAllBlocks_test_values)));
}

void testHeapAllBlocksPlusOneByte(void) {
    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE * (SUPPORT_UMM_MALLOC_BLOCKS - 3) + 1)));
}
