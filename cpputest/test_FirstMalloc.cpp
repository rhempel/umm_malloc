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

TEST_GROUP(FirstMalloc)
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

struct block_test_values Initialization_test_values[] =
{
  {0, false, 1, 0, 1, 1}
, {1, true,  UMM_LASTBLOCK, 0, 0, 0}
, {UMM_LASTBLOCK, false, 0, 1, 0, 0}
};

TEST(FirstMalloc, testHeapInitialization)
{
// DBGLOG_FORCE(true, "support heapsize %08x\n", SUPPORT_UMM_MALLOC_HEAP_SIZE);
   CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

TEST(FirstMalloc, testHeapFirstMalloc0Bytes)
{
    POINTERS_EQUAL((void *)NULL, (umm_malloc(0)));
    CHECK_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

struct block_test_values MallocFirstBlock_test_values[] =
{ {0, false, 1, 0, 2, 2}
, {1, false, 2, 0, 0, 0}
, {2, true,  UMM_LASTBLOCK, 1, 0, 0}
, {UMM_LASTBLOCK, false, 0, 2, 0, 0}
};

TEST(FirstMalloc, testHeapFirstMalloc1Bytes)
{
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(1)));
    CHECK_TRUE(check_blocks(MallocFirstBlock_test_values, ARRAYELEMENTCOUNT(MallocFirstBlock_test_values)));
}

TEST(FirstMalloc, testHeapFirstMalloc2Bytes)
{
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(2)));
    CHECK_TRUE(check_blocks(MallocFirstBlock_test_values, ARRAYELEMENTCOUNT(MallocFirstBlock_test_values)));
}

TEST(FirstMalloc, testHeapMallocFirstBlockBodyMinusOneBytes)
{
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE - 1)));
    CHECK_TRUE(check_blocks(MallocFirstBlock_test_values, ARRAYELEMENTCOUNT(MallocFirstBlock_test_values)));
}

TEST(FirstMalloc, testHeapMallocFirstBlockBodyBytes)
{
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(MallocFirstBlock_test_values, ARRAYELEMENTCOUNT(MallocFirstBlock_test_values)));
}

struct block_test_values MallocSecondBlock_test_values[] =
{ {0, false, 1, 0, 3, 3}
, {1, false, 3, 0, 0, 0}
, {3, true,  UMM_LASTBLOCK, 1, 0, 0}
, {UMM_LASTBLOCK, false, 0, 3, 0, 0}
};

TEST(FirstMalloc, testHeapMallocFirstBlockBodyPlusOneBytes)
{
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + 1)));
    CHECK_TRUE(check_blocks(MallocSecondBlock_test_values, ARRAYELEMENTCOUNT(MallocSecondBlock_test_values)));
}

TEST(FirstMalloc, testHeapMallocFirstBlockBodyPlusFullBlockBytes)
{
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE)));
    CHECK_TRUE(check_blocks(MallocSecondBlock_test_values, ARRAYELEMENTCOUNT(MallocSecondBlock_test_values)));
}

struct block_test_values MallocThirdBlock_test_values[] =
{ {0, false, 1, 0, 4, 4}
, {1, false, 4, 0, 0, 0}
, {4, true,  UMM_LASTBLOCK, 1, 0, 0}
, {UMM_LASTBLOCK, false, 0, 4, 0, 0}
};

TEST(FirstMalloc, testHeapMallocFirstAndSecondBlockBodyPlusOneBytes)
{
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE + 1)));
    CHECK_TRUE(check_blocks(MallocThirdBlock_test_values, ARRAYELEMENTCOUNT(MallocThirdBlock_test_values)));
}

struct block_test_values MallocFirstBlockBodyPlus500Blocks_test_values[] =
{ {0, false, 1, 0, 502, 502}
, {1, false, 502, 0,   0,   0}
, {502, true,  UMM_LASTBLOCK, 1,   0,   0}
, {UMM_LASTBLOCK, false, 0, 502, 0,   0}
};

TEST(FirstMalloc, testHeapMallocFirstBlockBodyPlus500Blocks)
{
    // Note that this test will actually allocate 510 blocks as the first
    // block contains the prev/next pointer pair.
    //
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE * 500)));
    CHECK_TRUE(check_blocks(MallocFirstBlockBodyPlus500Blocks_test_values, ARRAYELEMENTCOUNT(MallocFirstBlockBodyPlus500Blocks_test_values)));
}

struct block_test_values MallocAllBlocks_test_values[] =
{ {0, false, 1, 0, 0, 0}
, {1, false, UMM_LASTBLOCK, 0, 0, 0}
, {UMM_LASTBLOCK, false, 0, 1, 0, 0}
};

TEST(FirstMalloc, testHeapAllBlocks)
{
    POINTERS_EQUAL((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE * (SUPPORT_UMM_MALLOC_BLOCKS - 3))));
    CHECK_TRUE(check_blocks(MallocAllBlocks_test_values, ARRAYELEMENTCOUNT(MallocAllBlocks_test_values)));
}

TEST(FirstMalloc, testHeapAllBlocksPlusOneByte)
{
    POINTERS_EQUAL((void *)NULL, (umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE + UMM_BLOCK_BODY_SIZE * (SUPPORT_UMM_MALLOC_BLOCKS - 3) + 1)));
}
