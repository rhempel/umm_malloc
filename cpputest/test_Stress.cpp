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

TEST_GROUP(Stress)
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

TEST(Stress, testStress)
{
    uint64_t t = stress_test(100 * 1/*000*/, &umm_test_functions);

    umm_info(0, true);
    DBGLOG_FORCE(true, "Free Heap Size:    %d\n", umm_free_heap_size());
    DBGLOG_FORCE(true, "Typical Time (usec): %f\n", (double)t / ((100 * 1000)));
}

TEST(Stress, testStressLoop)
{
    int i;
    uint64_t t = 0;
    uint64_t total = 0;

    for (i = 0; i < 4; ++i) {
        umm_init();
        t = stress_test(100 * 1/*000*/, &umm_test_functions);
        umm_info(0, false);
        DBGLOG_FORCE(true, "Free Heap Size:      %d\n", umm_free_heap_size());
        DBGLOG_FORCE(true, "Typical Time (usec): %f\n", (double)t / ((100 * 1000)));
        total += t;
    }

    DBGLOG_FORCE(true, "Typical Time (usec): %f\n", (double)total / (4 * (100 * 1000)));
}
