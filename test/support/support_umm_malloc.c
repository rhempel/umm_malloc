#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "unity.h"
#include "umm_malloc_cfg.h"
#include "umm_malloc.h"

#include "support_umm_malloc.h"

/* Start addresses and the size of the heap */
uint32_t UMM_MALLOC_CFG_HEAP_SIZE = SUPPORT_UMM_MALLOC_HEAP_SIZE;

char test_umm_heap[0x2000][UMM_BLOCK_BODY_SIZE];
void *UMM_MALLOC_CFG_HEAP_ADDR = &test_umm_heap;

int umm_max_critical_depth;
int umm_critical_depth;

bool check_all_bytes(uint8_t *p, size_t s, uint8_t v) {
    while ((*p == v) && s) {
        ++p;
        --s;
    }

    return s == 0;
}

// Note, the get_xxx() functions depend on knowledge of the internals
// of umm_malloc.c which are not to be exposed to the user of the library
//
// Specifically, the array offsets represent the block indexes
// within the umm_block_t as follows:
//
// 0 - header.used.next (plus free flag)
// 1 - header.used.prev
// 2 - body.free.next
// 3 - body.free.prev

bool get_block_is_free(int b) {
    return (((uint16_t *)(&test_umm_heap[b]))[0] & 0x8000) == 0x8000;
}

uint16_t get_block_next(int b) {
    return ((uint16_t *)(&test_umm_heap[b]))[0] & 0x7FFF;
}

uint16_t get_block_prev(int b) {
    return ((uint16_t *)(&test_umm_heap[b]))[1];
}

uint16_t get_block_next_free(int b) {
    return ((uint16_t *)(&test_umm_heap[b]))[2];
}

uint16_t get_block_prev_free(int b) {
    return ((uint16_t *)(&test_umm_heap[b]))[3];
}

char block_test_msg[TEST_MSG_LEN];
char block_actual_msg[TEST_MSG_LEN];
char test_msg[256];

bool check_block(struct block_test_values *t) {
    snprintf(block_test_msg,   TEST_MSG_LEN, "\nTest__: Block %04d f %d n %04d p %04d nf %04d pf %04d", t->block
        , t->is_free
        , t->next
        , t->prev
        , t->next_free
        , t->prev_free);
    snprintf(block_actual_msg, TEST_MSG_LEN, "\nActual: Block %04d f %d n %04d p %04d nf %04d pf %04d\n", t->block
        , get_block_is_free(t->block)
        , get_block_next(t->block)
        , get_block_prev(t->block)
        , get_block_next_free(t->block)
        , get_block_prev_free(t->block));
    strncpy(test_msg, block_test_msg, 256);
    strncat(test_msg, block_actual_msg, 256);

    TEST_ASSERT_EQUAL_MESSAGE(t->is_free,   get_block_is_free(t->block), test_msg);
    TEST_ASSERT_EQUAL_MESSAGE(t->next,      get_block_next(t->block), test_msg);
    TEST_ASSERT_EQUAL_MESSAGE(t->prev,      get_block_prev(t->block), test_msg);
    TEST_ASSERT_EQUAL_MESSAGE(t->next_free, get_block_next_free(t->block), test_msg);
    TEST_ASSERT_EQUAL_MESSAGE(t->prev_free, get_block_prev_free(t->block), test_msg);

    return true;
}

size_t normalize_allocation_size(size_t s) {
    size_t first_block;
    size_t full_blocks;
    size_t extra_bytes;

    // This function normalizes the number of bytes to allocate so that all
    // the test cases work the same. The original test cases were designed
    // for the smallest block body size, so we need to calculate:
    //
    // 1. If we need only the initial block
    // 2. Number of additional full blocks
    // 3. Any extra bytes

    if (s <= (UMM_MIN_BLOCK_BODY_SIZE - UMM_BLOCK_HEADER_SIZE)) {
        first_block = 0;
        full_blocks = 0;
        extra_bytes = s;
    } else {
        first_block = 1;
        full_blocks = ((s - (UMM_MIN_BLOCK_BODY_SIZE - UMM_BLOCK_HEADER_SIZE)) / UMM_MIN_BLOCK_BODY_SIZE);
        extra_bytes = (s - (UMM_MIN_BLOCK_BODY_SIZE - UMM_BLOCK_HEADER_SIZE) - (full_blocks * UMM_MIN_BLOCK_BODY_SIZE));
    }

    return first_block * UMM_FIRST_BLOCK_BODY_SIZE + full_blocks * UMM_BLOCK_BODY_SIZE + extra_bytes;
}

bool check_blocks(struct block_test_values *t, size_t n) {
    int i;
    for (i = 0; i < n; ++i) {
        TEST_ASSERT_TRUE(check_block(&t[i]));
    }
    return true;
}

static uint64_t seed = 0;

void srand32(uint32_t s) {
    seed = s;
}

uint32_t rand32(void) {
    seed = (uint32_t)(((uint64_t)2862933555777941757 * seed) + 3037000493);

    return seed >> 10;
}

#define STRESS_TEST_ENTRIES (256)

#ifdef UMM_POISON_CHECK
struct umm_test_functions umm_test_functions = {
    umm_poison_malloc,
    umm_poison_calloc,
    umm_poison_realloc,
    umm_poison_free,
    umm_poison_check,
};
#else
bool  umm_check(void) {
    return true;
}

struct umm_test_functions umm_test_functions = {
    umm_malloc,
    umm_calloc,
    umm_realloc,
    umm_free,
    umm_check,
};
#endif

#define UMM_TEST_GETTIME(a) (clock_gettime(CLOCK_REALTIME, &a))

#define UMM_TEST_DIFFTIME(a,b) ((b.tv_sec - a.tv_sec) * (uint64_t)(1000 * 1000 * 1000) \
    + (b.tv_nsec - a.tv_nsec + 100))

uint64_t stress_test(int iterations, struct umm_test_functions *f) {
    void *p[STRESS_TEST_ENTRIES];
    int i,j,k;
    size_t s;

    uint64_t umm_malloc_time = 0;

    struct timespec start, end;

    srand32(0);

    for (j = 0; j < STRESS_TEST_ENTRIES; ++j) {
        p[j] = (void *)NULL;
    }

    for (i = 0; i < iterations; ++i) {
        j = rand32() % STRESS_TEST_ENTRIES;

        switch (rand32() % 16) {

            case  0:
            case  1:
            case  2:
            case  3:
            case  4:
            case  5:
            case  6: {
                UMM_TEST_GETTIME(start);
                p[j] = f->umm_test_realloc(p[j], 0);
                UMM_TEST_GETTIME(end);
                umm_malloc_time += UMM_TEST_DIFFTIME(start,end);

                TEST_ASSERT_NULL(p[j]);
                break;
            }
            case  7:
            case  8: {
                s = normalize_allocation_size(rand32() % 64);
                UMM_TEST_GETTIME(start);
                p[j] = f->umm_test_realloc(p[j], s);
                UMM_TEST_GETTIME(end);
                umm_malloc_time += UMM_TEST_DIFFTIME(start,end);

                if (s) {
                    TEST_ASSERT_NOT_NULL(p[j]);
                    memset(p[j], 0xfe, s);
                } else {
                    TEST_ASSERT_NULL(p[j]);
                }
                break;
            }

            case  9:
            case 10:
            case 11:
            case 12: {
                s = normalize_allocation_size(rand32() % 100);
                UMM_TEST_GETTIME(start);
                p[j] = f->umm_test_realloc(p[j], s);
                UMM_TEST_GETTIME(end);
                umm_malloc_time += UMM_TEST_DIFFTIME(start,end);

                if (s) {
                    TEST_ASSERT_NOT_NULL(p[j]);
                    memset(p[j], 0xfe, s);
                } else {
                    TEST_ASSERT_NULL(p[j]);
                }
                break;
            }

            case 13:
            case 14: {
                s = normalize_allocation_size(rand32() % 200);
                UMM_TEST_GETTIME(start);
                f->umm_test_free(p[j]);
                p[j] = f->umm_test_calloc(1, s);
                UMM_TEST_GETTIME(end);
                umm_malloc_time += UMM_TEST_DIFFTIME(start,end);

                if (s) {
                    TEST_ASSERT_NOT_NULL(p[j]);
                    TEST_ASSERT_TRUE(check_all_bytes(p[j], s, 0x00));
                    memset(p[j], 0xfe, s);
                } else {
                    TEST_ASSERT_NULL(p[j]);
                }
                break;
            }

            default: {
                s = normalize_allocation_size(rand32() % 400);
                UMM_TEST_GETTIME(start);
                f->umm_test_free(p[j]);
                p[j] = f->umm_test_malloc(s);
                UMM_TEST_GETTIME(end);
                umm_malloc_time += UMM_TEST_DIFFTIME(start,end);

                if (s) {
                    TEST_ASSERT_NOT_NULL(p[j]);
                    memset(p[j], 0xfe, s);
                } else {
                    TEST_ASSERT_NULL(p[j]);
                }
                break;
            }
        }

        TEST_ASSERT_NOT_EQUAL(0, INTEGRITY_CHECK());
        TEST_ASSERT_NOT_EQUAL(0, POISON_CHECK());
    }

    return umm_malloc_time;
}
