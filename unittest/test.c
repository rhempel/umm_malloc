#include "CppUTest/TestHarness.h"

TEST_GROUP(FirstTestGroup)
{
};

TEST(FirstTestGroup, FirstTest)
{
   FAIL("Fail me!");
}

TEST(FirstTestGroup, ThirdTest)
{
   STRCMP_EQUAL("hello", "hello");
   LONGS_EQUAL(1, 1);
   CHECK(true);
}

TEST(FirstTestGroup, SecondTest)
{
   STRCMP_EQUAL("hello", "world");
   LONGS_EQUAL(1, 2);
   CHECK(false);
}

// #include <umm_malloc_cfg.h>
#include <umm_malloc.h>

// #include <support_umm_malloc.h>

///* Use the default DBGLOG_LEVEL and DBGLOG_FUNCTION */
//
//#define DBGLOG_LEVEL 0
//
//#ifdef DBGLOG_ENABLE
//    #include "dbglog/dbglog.h"
//#endif

void setUp(void) {
//    umm_init();
//    umm_critical_depth = 0;
//    umm_max_critical_depth = 0;
}

void tearDown(void) {
//    TEST_ASSERT_LESS_OR_EQUAL(1, umm_max_critical_depth);
}

//struct block_test_values Initialization_test_values[] = {
//  {0, false, 1, 0, 1, 1}
//, {1, true,  UMM_LASTBLOCK, 0, 0, 0}
//, {UMM_LASTBLOCK, false, 0, 1, 0, 0}
//};

TEST(FirstTestGroup, testHeapInitialization)
{
   umm_init();
//    DBGLOG_FORCE(true, "support heapsize %08x\n", SUPPORT_UMM_MALLOC_HEAP_SIZE);

//    TEST_ASSERT_TRUE(check_blocks(Initialization_test_values, ARRAYELEMENTCOUNT(Initialization_test_values)));
   CHECK(true);
}


