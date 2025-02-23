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

TEST_GROUP(Metrics)
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

TEST(Metrics, testMetricsRandom)
{
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
        CHECK_COMPARE(1 * 1000 * 1000 + 3600, >=, p[i]);
        CHECK_COMPARE(1 * 1000 * 1000 - 3600, <=, p[i]);
    }
}

TEST(Metrics, testMetricsEmpty)
{
    #ifdef UMM_INLINE_METRICS
    CHECK_COMPARE(0, ==, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    CHECK_COMPARE(0, ==, umm_fragmentation_metric());
    #endif
}

TEST(Metrics, testMetricsFull)
{
    void *p[UMM_LASTBLOCK];
    int i;

    memset(p, sizeof(p), 0);

    for (i = 0; i < (UMM_LASTBLOCK - 1); ++i) {
        p[i] = umm_malloc(UMM_FIRST_BLOCK_BODY_SIZE);
    }

    #ifdef UMM_INLINE_METRICS
    CHECK_COMPARE(0, ==, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    CHECK_COMPARE(0, ==, umm_fragmentation_metric());
    #endif
}

TEST(Metrics, testMetricsSparseFull)
{
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
    CHECK_COMPARE(99, ==, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    CHECK_COMPARE(99, ==, umm_fragmentation_metric());
    #endif
}

TEST(Metrics, testMetricsSparse7of8)
{
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
    CHECK_COMPARE(78, ==, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    CHECK_COMPARE(78, ==, umm_fragmentation_metric());
    #endif
}

TEST(Metrics, testMetricsSparse3of4)
{
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
    CHECK_COMPARE(61, ==, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    CHECK_COMPARE(61, ==, umm_fragmentation_metric());
    #endif
}

TEST(Metrics, testMetricsSparse1of2)
{
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
    CHECK_COMPARE(34, ==, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    CHECK_COMPARE(34, ==, umm_fragmentation_metric());
    #endif
}

TEST(Metrics, testMetricsSparse1of4)
{
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
    CHECK_COMPARE(15, ==, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    CHECK_COMPARE(15, ==, umm_fragmentation_metric());
    #endif
}

TEST(Metrics, testMetricsSparse1of8)
{
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
    CHECK_COMPARE(7, ==, umm_fragmentation_metric());
    #endif
    #ifdef UMM_INFO
    umm_info(0, false);
    CHECK_COMPARE(7, ==, umm_fragmentation_metric());
    #endif
}
