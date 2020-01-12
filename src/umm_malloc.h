/* ----------------------------------------------------------------------------
 * umm_malloc.h - a memory allocator for embedded systems (microcontrollers)
 *
 * See copyright notice in LICENSE.TXT
 * ----------------------------------------------------------------------------
 */

#ifndef UMM_MALLOC_H
#define UMM_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

extern void  umm_init( void );
extern void *umm_malloc( size_t size );
extern void *umm_calloc( size_t num, size_t size );
extern void *umm_realloc( void *ptr, size_t size );
extern void  umm_free( void *ptr );

#ifdef UMM_METRICS
extern int umm_fragmentation_metric( void );
extern int umm_fragmentation_metric_freeblocks( void );
#else
#define umm_fragmentation_metric() (0)
#define umm_fragmentation_freeblocks() (0)
#endif // UMM_METRICS

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif

#endif /* UMM_MALLOC_H */
