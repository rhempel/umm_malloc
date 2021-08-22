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

#ifdef UMM_POISON_CHECK
void testPoisonFirst(void) {
    TEST_ASSERT_NOT_NULL(umm_test_functions.umm_test_malloc(UMM_FIRST_BLOCK_BODY_SIZE));
}

void testPoisonClobberLeading(void) {
    void *p = umm_test_functions.umm_test_malloc(UMM_BLOCK_BODY_SIZE * 8);

    p = p - 1;
    *(char *)p = 0x00;

    TEST_ASSERT_EQUAL(0, POISON_CHECK());
}

void testPoisonClobberTrailing(void) {
    void *p = umm_test_functions.umm_test_malloc(UMM_BLOCK_BODY_SIZE * 8);

    p = p + UMM_BLOCK_BODY_SIZE * 8;
    *(char *)p = 0x00;

    TEST_ASSERT_EQUAL(0, POISON_CHECK());
}
#endif

void testPoisonRandom(void) {
    void *p[100];
    int i,j;
    size_t s;

    srand32(0);

    for (i = 0; i < 100; ++i) {
        p[i] = (void *)NULL;
    }

    for (i = 0; i < 100000; ++i) {

        TEST_ASSERT_NOT_EQUAL(0, INTEGRITY_CHECK());
        TEST_ASSERT_NOT_EQUAL(0, POISON_CHECK());

        j = rand32() % 100;
        s = rand32() % 64;

        if (p[j]) {
            umm_test_functions.umm_test_free(p[j]);
        }

        p[j] = umm_test_functions.umm_test_malloc(normalize_allocation_size(s));

        if (0 == s) {
            TEST_ASSERT_NULL(p[j]);
        } else {
            TEST_ASSERT_NOT_NULL(p[j]);
        }
    }
}

void testPoisonStress(void) {
    uint64_t t = stress_test(100 * 1000, &umm_test_functions);

    umm_info(0, true);
    DBGLOG_FORCE(true, "Free Heap Size:    %d\n", umm_free_heap_size());
    DBGLOG_FORCE(true, "Typical Time (usec): %f\n", (double)t / ((100 * 1000)));
}

void testPoisonStressLoop(void) {
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
