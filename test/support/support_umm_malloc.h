#ifndef _SUPPORT_UMM_MALLOC_H
#define _SUPPORT_UMM_MALLOC_H

#include <stdbool.h>
#include <stdint.h>

#include <umm_malloc_cfg.h>
/* Use the default DBGLOG_LEVEL and DBGLOG_FUNCTION */

#ifdef UMM_DBGLOG_ENABLE
    #include "dbglog/dbglog.h"
#endif

#define UMM_BLOCK_HEADER_SIZE (4)
#define UMM_FIRST_BLOCK_BODY_SIZE (UMM_BLOCK_BODY_SIZE - UMM_BLOCK_HEADER_SIZE)

extern char test_umm_heap[][UMM_BLOCK_BODY_SIZE];
extern int umm_max_critical_depth;
extern int umm_critical_depth;

struct block_test_values {
    uint16_t block;
    bool is_free;
    uint16_t next;
    uint16_t prev;
    uint16_t next_free;
    uint16_t prev_free;
};

extern size_t normalize_allocation_size(size_t);
extern bool check_block(struct block_test_values *);
extern bool check_blocks(struct block_test_values *, size_t);

extern void srand32(uint32_t);
extern uint32_t rand32(void);

// Note, the block size calculation depends on knowledge of the internals
// of umm_malloc.c which are not to be exposed to the user of the library
//
// We now can configure the block body size - to make the tests pass
// without changing the ptr indexes we scale the UMM_MALLOC_HEAP_SIZE
// to the UMM_BLOCK_BODY_SIZE

#define ARRAYELEMENTCOUNT(x) (sizeof (x) / sizeof (x)[0])

#define SUPPORT_UMM_MALLOC_BLOCKS (0x2000)
#define SUPPORT_UMM_MALLOC_HEAP_SIZE (SUPPORT_UMM_MALLOC_BLOCKS * UMM_BLOCK_BODY_SIZE)
#define UMM_LASTBLOCK ((SUPPORT_UMM_MALLOC_HEAP_SIZE / UMM_BLOCK_BODY_SIZE) - 1)

#define TEST_MSG_LEN (132)

struct umm_test_functions {
    void *(*umm_test_malloc)(size_t);
    void *(*umm_test_calloc)(size_t, size_t);
    void *(*umm_test_realloc)(void *, size_t);
    void (*umm_test_free)(void *);
    bool (*umm_test_check)(void);
};

extern struct umm_test_functions umm_test_functions;

extern uint64_t stress_test(int, struct umm_test_functions *);

#endif // _SUPPORT_UMM_MALLOC_H
