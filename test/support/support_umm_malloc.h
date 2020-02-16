#ifndef _SUPPORT_UMM_MALLOC_H
#define _SUPPORT_UMM_MALLOC_H

#include <stdbool.h>
#include <stdint.h>

/* Use the default DBGLOG_LEVEL and DBGLOG_FUNCTION */

#define DBGLOG_LEVEL 0

#include "dbglog/dbglog.h"

// #include <support_umm_heap.h>

extern char test_umm_heap[];
extern int umm_max_critical_depth;
extern int umm_critical_depth;

struct block_test_values {
    uint16_t block;
    bool     is_free;
    uint16_t next;
    uint16_t prev;
    uint16_t next_free;
    uint16_t prev_free;
};

extern bool check_block(struct block_test_values *);
extern bool check_blocks(struct block_test_values *, size_t);

extern void srand32(uint32_t);
extern uint32_t rand32(void);

// Note, the block size calculation depends on knowledge of the internals
// of umm_malloc.c which are not to be exposed to the user of the library

#define SUPPORT_UMM_MALLOC_HEAP_SIZE (0x10000)
#define UMM_BLOCK_SIZE (8)
#define UMM_LASTBLOCK ((SUPPORT_UMM_MALLOC_HEAP_SIZE-UMM_BLOCK_SIZE)/UMM_BLOCK_SIZE)

#define ARRAYELEMENTCOUNT(x) (sizeof (x) / sizeof (x)[0])

#define TEST_MSG_LEN (132)

struct umm_test_functions {
   void *(*umm_test_malloc)(size_t);
   void *(*umm_test_calloc)(size_t, size_t);
   void *(*umm_test_realloc)(void *, size_t);
   void  (*umm_test_free)(void *);
   bool  (*umm_test_check)(void);
};

extern struct umm_test_functions umm_test_poison;

extern uint64_t stress_test( int, struct umm_test_functions *);

#endif // _SUPPORT_UMM_MALLOC_H
