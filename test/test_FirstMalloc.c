#include "unity.h"

#include "umm_malloc.h"

#include "support_umm_malloc.h"

void setUp(void)
{
    umm_init();
    umm_critical_depth = 0;
    umm_max_critical_depth = 0;
}

void tearDown(void)
{
    TEST_ASSERT_LESS_OR_EQUAL (1, umm_max_critical_depth);
}

struct block_test_values Initialization_test_values[] =
  { {0            , false, 1            , 0, 1, 1}
  , {1            , true,  UMM_LASTBLOCK, 0, 0, 0}
  , {UMM_LASTBLOCK, false, 0            , 1, 0, 0}
    };

void testHeapInitialization(void)
{
    DBGLOG_FORCE(true, "support heapsize %08x\n", SUPPORT_UMM_MALLOC_HEAP_SIZE);

    TEST_ASSERT_TRUE (check_blocks (Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

void testHeapFirstMalloc0Bytes(void)
{
    TEST_ASSERT_EQUAL_PTR ((void *)NULL, (umm_malloc (0)));
    TEST_ASSERT_TRUE (check_blocks (Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
}

struct block_test_values MallocMin_test_values[] =
    { {0            , false, 1            , 0, 2, 2}
    , {1            , false, 2            , 0, 0, 0}
    , {2            , true,  UMM_LASTBLOCK, 1, 0, 0}
    , {UMM_LASTBLOCK, false, 0            , 2, 0, 0}
    };

void testHeapFirstMalloc1Bytes(void)
{
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc (1)));
    TEST_ASSERT_TRUE (check_blocks (MallocMin_test_values, ARRAYELEMENTCOUNT(MallocMin_test_values)));
}

void testHeapFirstMalloc2Bytes(void)
{
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc (2)));
    TEST_ASSERT_TRUE (check_blocks (MallocMin_test_values, ARRAYELEMENTCOUNT(MallocMin_test_values)));
}

void testHeapFirstMalloc3Bytes(void)
{
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc (3)));
    TEST_ASSERT_TRUE (check_blocks (MallocMin_test_values, ARRAYELEMENTCOUNT(MallocMin_test_values)));
}

void testHeapFirstMalloc4Bytes(void)
{
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc (4)));
    TEST_ASSERT_TRUE (check_blocks (MallocMin_test_values, ARRAYELEMENTCOUNT(MallocMin_test_values)));
}

struct block_test_values MallocSmall_test_values[] =
    { {0            , false, 1            , 0, 3, 3}
    , {1            , false, 3            , 0, 0, 0}
    , {3            , true,  UMM_LASTBLOCK, 1, 0, 0}
    , {UMM_LASTBLOCK, false, 0            , 3, 0, 0}
    };

void testHeapFirstMalloc5Bytes(void)
{
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc (5)));
    TEST_ASSERT_TRUE (check_blocks (MallocSmall_test_values, ARRAYELEMENTCOUNT(MallocSmall_test_values)));
}

void testHeapFirstMalloc12Bytes(void)
{
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc (12)));
    TEST_ASSERT_TRUE (check_blocks (MallocSmall_test_values, ARRAYELEMENTCOUNT(MallocSmall_test_values)));
}

struct block_test_values MallocSmallPlus_test_values[] =
    { {0            , false, 1            , 0, 4, 4}
    , {1            , false, 4            , 0, 0, 0}
    , {4            , true,  UMM_LASTBLOCK, 1, 0, 0}
    , {UMM_LASTBLOCK, false, 0            , 4, 0, 0}
    };

void testHeapFirstMalloc13Bytes(void)
{
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc (13)));
    TEST_ASSERT_TRUE (check_blocks (MallocSmallPlus_test_values, ARRAYELEMENTCOUNT(MallocSmallPlus_test_values)));
}

struct block_test_values MallocMid_test_values[] =
    { {0            , false, 1            , 0, 502, 502}
    , {1            , false, 502          , 0,   0,   0}
    , {502          , true,  UMM_LASTBLOCK, 1,   0,   0}
    , {UMM_LASTBLOCK, false, 0            , 502, 0,   0}
    };

void testHeapFirstMalloc4000Bytes(void)
{
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc (4000)));
    TEST_ASSERT_TRUE (check_blocks (MallocMid_test_values, ARRAYELEMENTCOUNT(MallocMid_test_values)));
}

struct block_test_values MallocLarge_test_values[] =
    { {0             , false, 1            , 0, 0, 0}
    , {1             , false, UMM_LASTBLOCK, 0, 0, 0}
    , {UMM_LASTBLOCK , false, 0            , 1, 0, 0}
    };

void testHeapFirstMalloc65516Bytes(void)
{
    TEST_ASSERT_EQUAL_PTR((void *)&test_umm_heap[1][UMM_BLOCK_HEADER_SIZE], (umm_malloc (65516)));
    TEST_ASSERT_TRUE (check_blocks (MallocLarge_test_values, ARRAYELEMENTCOUNT(MallocLarge_test_values)));
}

void testHeapFirstMalloc65517Bytes(void)
{
    TEST_ASSERT_EQUAL_PTR((void *)NULL, (umm_malloc (65517)));
}
