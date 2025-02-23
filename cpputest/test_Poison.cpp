#include <umm_malloc.h>
#include <support_umm_malloc.h>

#warning "Add support for the DBGLOG macros - consider a more generic log that plus into Linux or Segger"
///* Use the default DBGLOG_LEVEL and DBGLOG_FUNCTION */
//
//#define DBGLOG_LEVEL 0
//
//#ifdef DBGLOG_ENABLE
//    #include "dbglog/dbglog.h"
//#endif

#include "CppUTest/TestHarness.h"

TEST_GROUP(Poison)
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

#ifdef UMM_POISON_CHECK
TEST(Poison, testPoisonFirst)
{
    CHECK_COMPARE((long int)NULL, !=, umm_test_functions.umm_test_malloc(UMM_FIRST_BLOCK_BODY_SIZE));
}

TEST(Poison, testPoisonClobberLeading)
{
    void *p = umm_test_functions.umm_test_malloc(UMM_BLOCK_BODY_SIZE * 8);

    p = (char *)p - 1;
    *(char *)p = 0x00;

    CHECK_COMPARE(0, ==, POISON_CHECK());
}

TEST(Poison, testPoisonClobberTrailing)
{
    void *p = umm_test_functions.umm_test_malloc(UMM_BLOCK_BODY_SIZE * 8);

    p = (char *)p + UMM_BLOCK_BODY_SIZE * 8;
    *(char *)p = 0x00;

    CHECK_COMPARE(0, ==, POISON_CHECK());
}
#endif

TEST(Poison, testPoisonRandom)
{
    void *p[100];
    int i,j;
    size_t s;

    srand32(0);

    for (i = 0; i < 100; ++i) {
        p[i] = (void *)NULL;
    }

    for (i = 0; i < 100000; ++i) {

        CHECK_COMPARE(0, !=, INTEGRITY_CHECK());
        CHECK_COMPARE(0, !=, POISON_CHECK());

        j = rand32() % 100;
        s = rand32() % 64;

        if (p[j]) {
            umm_test_functions.umm_test_free(p[j]);
        }

        p[j] = umm_test_functions.umm_test_malloc(normalize_allocation_size(s));

        if (0 == s) {
            CHECK_COMPARE((long int)NULL, ==, p[j]);
        } else {
            CHECK_COMPARE((long int)NULL, !=, p[j]);
        }
    }
}

TEST(Poison, testPoisonStress)
{
    uint64_t t = stress_test(100 * 1000, &umm_test_functions);

    umm_info(0, true);
    DBGLOG_FORCE(true, "Free Heap Size:    %d\n", umm_free_heap_size());
    DBGLOG_FORCE(true, "Typical Time (usec): %f\n", (double)t / ((100 * 1000)));
}

TEST(Poison, testPoisonStressLoop)
{
    int i;
    uint64_t t = 0;
    uint64_t total = 0;

    for (i = 0; i < 4; ++i) {
        umm_init();
        t = stress_test(100 * 1000, &umm_test_functions);
        umm_info(0, false);
        DBGLOG_FORCE(true, "Free Heap Size:      %d\n", umm_free_heap_size());
        DBGLOG_FORCE(true, "Typical Time (usec): %f\n", (double)t / ((100 * 1000)));
        total += t;
    }

    DBGLOG_FORCE(true, "Typical Time (usec): %f\n", (double)total / (4 * (100 * 1000)));
}
