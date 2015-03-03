// ----------------------------------------------------------------------------
// umm_malloc.h - a memory allocator for embedded systems (microcontrollers)
//
// See copyright notice in LICENSE.TXT
// ----------------------------------------------------------------------------

#ifndef UMM_MALLOC_H
#define UMM_MALLOC_H

// ----------------------------------------------------------------------------
// A couple of macros to make packing structures less compiler dependent

#define UMM_H_ATTPACKPRE
#define UMM_H_ATTPACKSUF __attribute__((__packed__))

// ----------------------------------------------------------------------------
// A couple of macros to make it easier to protect the memory allocator
// in a multitasking system. You should set these macros up to use whatever
// your system uses for this purpose. You can disable interrupts entirely, or
// just disable task switching - it's up to you
//
// NOTE WELL that these macros MUST be allowed to nest, because umm_free() is
// called from within umm_malloc()

#define UMM_CRITICAL_ENTRY()
#define UMM_CRITICAL_EXIT()

// ----------------------------------------------------------------------------

typedef struct UMM_HEAP_INFO_t {
  unsigned short int totalEntries;
  unsigned short int usedEntries;
  unsigned short int freeEntries; 

  unsigned short int totalBlocks; 
  unsigned short int usedBlocks; 
  unsigned short int freeBlocks; 
  }
  UMM_HEAP_INFO;
  
extern UMM_HEAP_INFO heapInfo;

extern char   __umm_heap_start[];
extern char   __umm_heap_end[];
extern size_t __umm_heap_size;

void *umm_info( void *ptr, int force );

// ----------------------------------------------------------------------------

#endif // UMM_MALLOC_H
