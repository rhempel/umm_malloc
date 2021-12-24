#include <string.h>

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

void testMetricsRandom(void) {
    int p[1000];
    int i;

    for (i = 0; i < 1000; ++i) {
        p[i] = 0;
    }

    srand32(0);

    for (i = 0; i < (1000 * 1000 * 1000); ++i) {
        p[rand32() % 1000]++;
    }

    for (i = 0; i < 1000; ++i) {
        TEST_ASSERT_LESS_OR_EQUAL(1 * 1000 * 1000 + 3600, p[i]);
        TEST_ASSERT_GREATER_OR_EQUAL(1 * 1000 * 1000 - 3600, p[i]);
    }
}

void testMetricsEmpty(void) {
    #ifdef UMM_INLINE_METRICS
    TEST_ASSERT_EQUAL(0, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    TEST_ASSERT_EQUAL(0, umm_fragmentation_metric());
    #endif
}

void testMetricsFull(void) {
    void *p[UMM_LASTBLOCK];
    int i;

    memset(p, sizeof(p), 0);

    for (i = 0; i < (UMM_LASTBLOCK - 1); ++i) {
        p[i] = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    }

    #ifdef UMM_INLINE_METRICS
    TEST_ASSERT_EQUAL(0, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    TEST_ASSERT_EQUAL(0, umm_fragmentation_metric());
    #endif
}

void testMetricsSparseFull(void) {
    void *p[UMM_LASTBLOCK];
    int i;

    memset(p, sizeof(p), 0);

    for (i = 0; i < (UMM_LASTBLOCK); ++i) {
        p[i] = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    }

    for (i = 1; i < (UMM_LASTBLOCK); i += 2) {
        umm_free(p[i]);
    }

    #ifdef UMM_INLINE_METRICS
    TEST_ASSERT_EQUAL(99, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    TEST_ASSERT_EQUAL(99, umm_fragmentation_metric());
    #endif
}

void testMetricsSparse7of8(void) {
    void *p[UMM_LASTBLOCK];
    int i;

    memset(p, sizeof(p), 0);

    for (i = 0; i < ((UMM_LASTBLOCK * 7) / 8); ++i) {
        p[i] = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    }

    for (i = 1; i < ((UMM_LASTBLOCK * 7) / 8); i += 2) {
        umm_free(p[i]);
    }

    #ifdef UMM_INLINE_METRICS
    TEST_ASSERT_EQUAL(78, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    TEST_ASSERT_EQUAL(78, umm_fragmentation_metric());
    #endif
}

void testMetricsSparse3of4(void) {
    void *p[UMM_LASTBLOCK];
    int i;

    memset(p, sizeof(p), 0);

    for (i = 0; i < ((UMM_LASTBLOCK * 3) / 4); ++i) {
        p[i] = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    }

    for (i = 1; i < ((UMM_LASTBLOCK * 3) / 4); i += 2) {
        umm_free(p[i]);
    }

    #ifdef UMM_INLINE_METRICS
    TEST_ASSERT_EQUAL(61, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    TEST_ASSERT_EQUAL(61, umm_fragmentation_metric());
    #endif
}

void testMetricsSparse1of2(void) {
    void *p[UMM_LASTBLOCK];
    int i;

    memset(p, sizeof(p), 0);

    for (i = 0; i < ((UMM_LASTBLOCK * 1) / 2); ++i) {
        p[i] = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    }

    for (i = 1; i < ((UMM_LASTBLOCK * 1) / 2); i += 2) {
        umm_free(p[i]);
    }

    #ifdef UMM_INLINE_METRICS
    TEST_ASSERT_EQUAL(34, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    TEST_ASSERT_EQUAL(34, umm_fragmentation_metric());
    #endif
}

void testMetricsSparse1of4(void) {
    void *p[UMM_LASTBLOCK];
    int i;

    memset(p, sizeof(p), 0);

    for (i = 0; i < ((UMM_LASTBLOCK * 1) / 4); ++i) {
        p[i] = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    }

    for (i = 1; i < ((UMM_LASTBLOCK * 1) / 4); i += 2) {
        umm_free(p[i]);
    }

    #ifdef UMM_INLINE_METRICS
    TEST_ASSERT_EQUAL(15, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    TEST_ASSERT_EQUAL(15, umm_fragmentation_metric());
    #endif
}

void testMetricsSparse1of8(void) {
    void *p[UMM_LASTBLOCK];
    int i;

    memset(p, sizeof(p), 0);

    for (i = 0; i < ((UMM_LASTBLOCK * 1) / 8); ++i) {
        p[i] = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    }

    for (i = 1; i < ((UMM_LASTBLOCK * 1) / 8); i += 2) {
        umm_free(p[i]);
    }

    #ifdef UMM_INLINE_METRICS
    TEST_ASSERT_EQUAL(7, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    TEST_ASSERT_EQUAL(7, umm_fragmentation_metric());
    #endif
}
