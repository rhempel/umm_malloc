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

void *p[5];

TEST_GROUP(Free)
{
    void setup(void) {
        umm_init();
        umm_critical_depth = 0;
        umm_max_critical_depth = 0;

        p[0] = umm_malloc(4);
        p[1] = umm_malloc(4);
        p[2] = umm_malloc(4);
        p[3] = umm_malloc(4);
        p[4] = umm_malloc(4);
    }

    void teardown(void) {
        CHECK_COMPARE(1, >= ,umm_max_critical_depth);
    }
};

struct block_test_values FreeDoNothing_test_values[] =
{
  {0, false, 1, 0,  6,  6}
, {1, false, 2, 0,  0,  0}
, {2, false, 3, 1,  0,  0}
, {3, false, 4, 2,  0,  0}
, {4, false, 5, 3,  0,  0}
, {5, false, 6, 4,  0,  0}
, {6, true,  UMM_LASTBLOCK, 5,  0,  0}
, {UMM_LASTBLOCK, false, 0, 6,  0,  0}
};

TEST(Free, testFreeNullPtr)
{
    umm_free((void *)NULL);

    CHECK_TRUE(check_blocks(FreeDoNothing_test_values, ARRAYELEMENTCOUNT(FreeDoNothing_test_values)));

}

TEST(Free, testFreeLowPtr)
{
    umm_free((test_umm_heap - 1));

    CHECK_TRUE(check_blocks(FreeDoNothing_test_values, ARRAYELEMENTCOUNT(FreeDoNothing_test_values)));
}

TEST(Free, testFreeHighPtr)
{
    umm_free(test_umm_heap + SUPPORT_UMM_MALLOC_HEAP_SIZE);

    CHECK_TRUE(check_blocks(FreeDoNothing_test_values, ARRAYELEMENTCOUNT(FreeDoNothing_test_values)));
}

struct block_test_values FreeFirst_test_values[] = {
  {0, false, 1, 0, 1, 1}
, {1, true,  UMM_LASTBLOCK, 0, 0, 0}
, {UMM_LASTBLOCK, false, 0, 1, 0, 0}
};

TEST(Free, testFreeFirst)
{
    umm_init();
    umm_free(umm_malloc(4));

    CHECK_TRUE(check_blocks(FreeFirst_test_values, ARRAYELEMENTCOUNT(FreeFirst_test_values)));
}

struct block_test_values FreeLast_test_values[] = {
  {0, false, 1, 0,  5,  5}
, {1, false, 2, 0,  0,  0}
, {2, false, 3, 1,  0,  0}
, {3, false, 4, 2,  0,  0}
, {4, false, 5, 3,  0,  0}
, {5, true,  UMM_LASTBLOCK, 4,  0,  0}
, {UMM_LASTBLOCK, false, 0, 5,  0,  0}
};

TEST(Free, testFreeLast)
{
    umm_free(p[4]);

    CHECK_TRUE(check_blocks(FreeLast_test_values, ARRAYELEMENTCOUNT(FreeLast_test_values)));
}

struct block_test_values FreeSecondLast_test_values[] = {
  {0, false, 1, 0,  4,  6}
, {1, false, 2, 0,  0,  0}
, {2, false, 3, 1,  0,  0}
, {3, false, 4, 2,  0,  0}
, {4, true,  5, 3,  6,  0}
, {5, false, 6, 4,  0,  0}
, {6, true,  UMM_LASTBLOCK, 5,  0,  4}
, {UMM_LASTBLOCK, false, 0, 6,  0,  0}};

TEST(Free, testFreeSecondLast)
{
    umm_free(p[3]);

    CHECK_TRUE(check_blocks(FreeSecondLast_test_values, ARRAYELEMENTCOUNT(FreeSecondLast_test_values)));
}

struct block_test_values FreeAssimilateUp_test_values[] = {
  {0, false, 1, 0,  3,  6}
, {1, false, 2, 0,  0,  0}
, {2, false, 3, 1,  0,  0}
, {3, true,  5, 2,  6,  0}
, {5, false, 6, 3,  0,  0}
, {6, true,  UMM_LASTBLOCK, 5,  0,  3}
, {UMM_LASTBLOCK, false, 0, 6,  0,  0}
};

TEST(Free, testFreeAssimilateUp)
{
    umm_free(p[3]);
    umm_free(p[2]);

    CHECK_TRUE(check_blocks(FreeAssimilateUp_test_values, ARRAYELEMENTCOUNT(FreeAssimilateUp_test_values)));
}

struct block_test_values FreeAssimilateDown_test_values[] = {
  {0, false, 1, 0,  3,  6}
, {1, false, 2, 0,  0,  0}
, {2, false, 3, 1,  0,  0}
, {3, true,  5, 2,  6,  0}
, {5, false, 6, 3,  0,  0}
, {6, true,  UMM_LASTBLOCK, 5,  0,  3}
, {UMM_LASTBLOCK, false, 0, 6,  0,  0}
};

TEST(Free, testFreeAssimilateDown)
{
    umm_free(p[2]);
    umm_free(p[3]);

    CHECK_TRUE(check_blocks(FreeAssimilateDown_test_values, ARRAYELEMENTCOUNT(FreeAssimilateDown_test_values)));
}

struct block_test_values FreeAssimilateUpDown_test_values[] = {
  {0, false, 1, 0,  2,  6}
, {1, false, 2, 0,  0,  0}
, {2, true,  5, 1,  6,  0}
, {5, false, 6, 2,  0,  0}
, {6, true,  UMM_LASTBLOCK, 5,  0,  2}
, {UMM_LASTBLOCK, false, 0, 6,  0,  0}
};

TEST(Free, testFreeAssimilateUpDown)
{
    umm_free(p[3]);
    umm_free(p[1]);
    umm_free(p[2]);

    CHECK_TRUE(check_blocks(FreeAssimilateUpDown_test_values, ARRAYELEMENTCOUNT(FreeAssimilateUpDown_test_values)));
}

struct block_test_values FreeAssimilateDownUp_test_values[] = {
  {0, false, 1, 0,  2,  6}
, {1, false, 2, 0,  0,  0}
, {2, true,  5, 1,  6,  0}
, {5, false, 6, 2,  0,  0}
, {6, true,  UMM_LASTBLOCK, 5,  0,  2}
, {UMM_LASTBLOCK, false, 0, 6,  0,  0}
};

TEST(Free, testFreeAssimilateDownUp)
{
    umm_free(p[2]);
    umm_free(p[1]);
    umm_free(p[3]);

    CHECK_TRUE(check_blocks(FreeAssimilateDownUp_test_values, ARRAYELEMENTCOUNT(FreeAssimilateDownUp_test_values)));
}

struct block_test_values FreeAssimilateFirst_test_values[] = {
  {0, false, 1, 0,  1,  6}
, {1, true,  3, 0,  6,  0}
, {3, false, 4, 1,  0,  0}
, {4, false, 5, 3,  0,  0}
, {5, false, 6, 4,  0,  0}
, {6, true,  UMM_LASTBLOCK, 5,  0,  1}
, {UMM_LASTBLOCK, false, 0, 6,  0,  0}
};

TEST(Free, testFreeAssimilateFirst)
{
    umm_free(p[1]);
    umm_free(p[0]);

    CHECK_TRUE(check_blocks(FreeAssimilateFirst_test_values, ARRAYELEMENTCOUNT(FreeAssimilateFirst_test_values)));
}

struct block_test_values FreeAssimilateLast_test_values[] = {
  {0, false, 1, 0,  4,  4}
, {1, false, 2, 0,  0,  0}
, {2, false, 3, 1,  0,  0}
, {3, false, 4, 2,  0,  0}
, {4, true,  UMM_LASTBLOCK, 3,  0,  0}
, {UMM_LASTBLOCK, false, 0, 4,  0,  0}
};

TEST(Free, testFreeAssimilateLast)
{
    umm_free(p[3]);
    umm_free(p[4]);

    CHECK_TRUE(check_blocks(FreeAssimilateLast_test_values, ARRAYELEMENTCOUNT(FreeAssimilateLast_test_values)));
}


struct block_test_values FreeHiLo_test_values[] = {
  {0, false, 1, 0,  2,  6}
, {1, false, 2, 0,  0,  0}
, {2, true,  3, 1,  4,  0}
, {3, false, 4, 2,  0,  0}
, {4, true,  5, 3,  6,  2}
, {5, false, 6, 4,  0,  0}
, {6, true,  UMM_LASTBLOCK, 5,  0,  4}
, {UMM_LASTBLOCK, false, 0, 6,  0,  0}
};

TEST(Free, testFreeHiLo)
{
    umm_free(p[3]);
    umm_free(p[1]);

    CHECK_TRUE(check_blocks(FreeHiLo_test_values, ARRAYELEMENTCOUNT(FreeHiLo_test_values)));
}

struct block_test_values FreeLoHi_test_values[] = {
  {0, false, 1, 0,  4,  6}
, {1, false, 2, 0,  0,  0}
, {2, true,  3, 1,  6,  4}
, {3, false, 4, 2,  0,  0}
, {4, true,  5, 3,  2,  0}
, {5, false, 6, 4,  0,  0}
, {6, true,  UMM_LASTBLOCK, 5,  0,  2}
, {UMM_LASTBLOCK, false, 0, 6,  0,  0}
};

TEST(Free, testFreeLoHi) {
    umm_free(p[1]);
    umm_free(p[3]);

    CHECK_TRUE(check_blocks(FreeLoHi_test_values, ARRAYELEMENTCOUNT(FreeLoHi_test_values)));
}
