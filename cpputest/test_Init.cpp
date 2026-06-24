#include <umm_malloc.h>
#include <support_umm_malloc.h>

#include "CppUTest/TestHarness.h"

TEST_GROUP(Init)
{
    void setup(void) {
        umm_critical_depth = 0;
        umm_max_critical_depth = 0;
    }

    void teardown(void) {
        CHECK_COMPARE(1, >= ,umm_max_critical_depth);
    }
};

static struct block_test_values Initialization_test_values[] =
{
  {0, false, 1, 0, 1, 1}
, {1, true,  UMM_LASTBLOCK, 0, 0, 0}
, {UMM_LASTBLOCK, false, 0, 1, 0, 0}
};

static char umm_very_large_heap[0x8010][UMM_BLOCK_BODY_SIZE];

TEST(Init, testInitZeroSizeHeap)
{
   umm_multi_init_heap(&umm_test_heap_config, umm_very_large_heap, 0);
   CHECK_EQUAL_TEXT((void *)NULL, umm_test_heap_config.pheap, "No heap should have been allocated" );
   CHECK_EQUAL_TEXT(0, umm_test_heap_config.heap_size, "No heap should have been allocated" );
   CHECK_EQUAL_TEXT(0, umm_test_heap_config.numblocks, "No heap should have been allocated" );
}

TEST(Init, testInitUnderSizeHeap)
{
   umm_multi_init_heap(&umm_test_heap_config, umm_very_large_heap, UMM_BLOCK_BODY_SIZE*7);
   CHECK_EQUAL_TEXT((void *)NULL, umm_test_heap_config.pheap, "No heap should have been allocated" );
   CHECK_EQUAL_TEXT(0, umm_test_heap_config.heap_size, "No heap should have been allocated" );
   CHECK_EQUAL_TEXT(0, umm_test_heap_config.numblocks, "No heap should have been allocated" );
}

static struct block_test_values minimum_heap_test_values[] =
{
  {0, false, 1, 0, 1, 1}
, {1, true,  7, 0, 0, 0}
, {7, false, 0, 1, 0, 0}
};

TEST(Init, testInitMinimumSizeHeap)
{
   umm_multi_init_heap(&umm_test_heap_config, umm_very_large_heap, UMM_BLOCK_BODY_SIZE*8);
   CHECK_EQUAL_TEXT(umm_very_large_heap, umm_test_heap_config.pheap, "Heap should have been allocated" );
   CHECK_EQUAL_TEXT(UMM_BLOCK_BODY_SIZE*8, umm_test_heap_config.heap_size, "Heap should have been allocated" );
   CHECK_EQUAL_TEXT(8, umm_test_heap_config.numblocks, "Heap should have been allocated" );

   CHECK_TRUE(check_blocks(&umm_test_heap_config, minimum_heap_test_values, ARRAYELEMENTCOUNT(minimum_heap_test_values)));
}

static struct block_test_values maximum_heap_test_values[] =
{
  {0, false, 1, 0, 1, 1}
, {1, true,  32766, 0, 0, 0}
, {32766, false, 0, 1, 0, 0}
};

TEST(Init, testInitMaximumSizeHeap)
{
   umm_multi_init_heap(&umm_test_heap_config, umm_very_large_heap, UMM_BLOCK_BODY_SIZE*32767);
   CHECK_EQUAL_TEXT(umm_very_large_heap, umm_test_heap_config.pheap, "No heap should have been allocated" );
   CHECK_EQUAL_TEXT(UMM_BLOCK_BODY_SIZE*32767, umm_test_heap_config.heap_size, "Heap should have been allocated" );
   CHECK_EQUAL_TEXT(32767, umm_test_heap_config.numblocks, "Heap should have been allocated" );

   CHECK_TRUE(check_blocks(&umm_test_heap_config, maximum_heap_test_values, ARRAYELEMENTCOUNT(minimum_heap_test_values)));
}

static struct block_test_values odd_heap_test_values[] =
{
  {0, false, 1, 0, 1, 1}
, {1, true,  199, 0, 0, 0}
, {199, false, 0, 1, 0, 0}
};

TEST(Init, testInitOddSizeHeap)
{
   umm_multi_init_heap(&umm_test_heap_config, umm_very_large_heap, UMM_BLOCK_BODY_SIZE*200 + 5);
   CHECK_EQUAL_TEXT(umm_very_large_heap, umm_test_heap_config.pheap, "No heap should have been allocated" );
   CHECK_EQUAL_TEXT(UMM_BLOCK_BODY_SIZE*200, umm_test_heap_config.heap_size, "Heap should have been allocated" );
   CHECK_EQUAL_TEXT(200, umm_test_heap_config.numblocks, "Heap should have been allocated" );

   CHECK_TRUE(check_blocks(&umm_test_heap_config, odd_heap_test_values, ARRAYELEMENTCOUNT(minimum_heap_test_values)));
}

TEST(Init, testInitOverSizeHeap)
{
   umm_multi_init_heap(&umm_test_heap_config, umm_very_large_heap, UMM_BLOCK_BODY_SIZE*32768);
   CHECK_EQUAL_TEXT((void *)NULL, umm_test_heap_config.pheap, "No heap should have been allocated" );
   CHECK_EQUAL_TEXT(0, umm_test_heap_config.heap_size, "No heap should have been allocated" );
   CHECK_EQUAL_TEXT(0, umm_test_heap_config.numblocks, "No heap should have been allocated" );
}

// This one tests for uint16 overflow - the result would be 32 blocks :-)

TEST(Init, testInitExtremeSizeHeap)
{
   umm_multi_init_heap(&umm_test_heap_config, umm_very_large_heap, UMM_BLOCK_BODY_SIZE*(65536 + 32));
   CHECK_EQUAL_TEXT((void *)NULL, umm_test_heap_config.pheap, "No heap should have been allocated" );
   CHECK_EQUAL_TEXT(0, umm_test_heap_config.heap_size, "No heap should have been allocated" );
   CHECK_EQUAL_TEXT(0, umm_test_heap_config.numblocks, "No heap should have been allocated" );
}
