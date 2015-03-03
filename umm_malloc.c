// ----------------------------------------------------------------------------
// umm_malloc.c - a memory allocator for embedded systems (microcontrollers)
//
// See copyright notice in LICENSE.TXT
// ----------------------------------------------------------------------------
//
// R.Hempel 2007-09-22 - Original
// R.Hempel 2008-12-11 - Added MIT License biolerplate
//                     - realloc() now looks to see if previous block is free
//                     - made common operations functions            
// R.Hempel 2009-03-02 - Added macros to disable tasking
//                     - Added function to dump heap and check for valid free
//                        pointer
// R.Hempel 2009-03-09 - Changed name to umm_malloc to avoid conflicts with
//                        the mm_malloc() library functions
//                     - Added some test code to assimilate a free block
//                        with the very block if possible. Complicated and
//                        not worth the grief.
// R.Hempel 2015-03-02 - Moved the descriptive text to README.md
// ----------------------------------------------------------------------------

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "umm_malloc.h"

// ----------------------------------------------------------------------------
//
// There are a number of defines you can set at compile time that affect how
// the memory allocator will operate. In GNU C, you set these compile time
// defines like this:
//
// -D UMM_TEST_MAIN
//
// Set this if you want to compile in the test suite at the end of this file.
// If you do set this variable, then the function names are left alone as
// umm_malloc() umm_free() and umm_realloc() so that they cannot be confused
// with the C runtime functions malloc() free() and realloc()
//
// If you leave this variable unset, then the function names become malloc()
// free() and realloc() so that they can be used as the C runtime functions
// in an embedded environment.
//
// -D UMM_BEST_FIT (defualt)
//
// Set this if you want to use a best-fit algorithm for allocating new
// blocks
//
// -D UMM_FIRST_FIT
//
// Set this if you want to use a first-fit algorithm for allocating new
// blocks
//
// -D UMM_DBG_LOG_LEVEL=n
//
// Set n to a value from 0 to 6 depending on how verbose you want the debug
// log to be
//
// ----------------------------------------------------------------------------
// 
// Support for this library in a multitasking environment is provided when
// you add bodies to the UMM_CRITICAL_ENTRY and UMM_CRITICAL_EXIT macros
// in umm_malloc.h
//
// ----------------------------------------------------------------------------

#ifndef UMM_FIRST_FIT
#  ifndef UMM_BEST_FIT
#    define UMM_BEST_FIT
#  endif
#endif

#ifndef UMM_DBG_LOG_LEVEL
#  undef  DBG_LOG_LEVEL
#  define DBG_LOG_LEVEL 0
#else
#  undef  DBG_LOG_LEVEL
#  define DBG_LOG_LEVEL UMM_DBG_LOG_LEVEL
#endif

#include "dbglog.h"

// ----------------------------------------------------------------------------

UMM_H_ATTPACKPRE typedef struct umm_ptr_t {
  unsigned short int next;
  unsigned short int prev;
} UMM_H_ATTPACKSUF umm_ptr;


UMM_H_ATTPACKPRE typedef struct umm_block_t {
  union {
    umm_ptr used;
  } header;
  union {
    umm_ptr free;
    unsigned char data[4];
  } body;
} UMM_H_ATTPACKSUF umm_block;

#define UMM_FREELIST_MASK (0x8000)
#define UMM_BLOCKNO_MASK  (0x7FFF)

// ----------------------------------------------------------------------------

#ifndef UMM_TEST_MAIN

  #define umm_free    free
  #define umm_malloc  malloc
  #define umm_realloc realloc

  extern umm_block umm_heap[];

  // Note that _UMM_NUMBLOCKS is a value that is computed at link time, and
  // it represents the number of blocks available for the memory manager.

  extern unsigned short int _UMM_NUMBLOCKS;

  // Link time calculations assign values to symbols, but you can't take
  // the value of something filled in at link time, you can only get its
  // address.
  //
  // That's why we take the address of _UMM_NUMBLOCKS and assign it to
  // the constant value umm_numblocks.

  const unsigned int umm_numblocks = (unsigned int)(&_UMM_NUMBLOCKS);

  #define UMM_NUMBLOCKS (umm_numblocks)

#else

  umm_block umm_heap[2600];

  const unsigned short int umm_numblocks = sizeof(umm_heap)/sizeof(umm_block);

  #define UMM_NUMBLOCKS (umm_numblocks)

#endif

// ----------------------------------------------------------------------------

#define UMM_BLOCK(b)  (umm_heap[b])

#define UMM_NBLOCK(b) (UMM_BLOCK(b).header.used.next)
#define UMM_PBLOCK(b) (UMM_BLOCK(b).header.used.prev)
#define UMM_NFREE(b)  (UMM_BLOCK(b).body.free.next)
#define UMM_PFREE(b)  (UMM_BLOCK(b).body.free.prev)
#define UMM_DATA(b)   (UMM_BLOCK(b).body.data)

// ----------------------------------------------------------------------------
// One of the coolest things about this little library is that it's VERY
// easy to get debug information about the memory heap by simply iterating
// through all of the memory blocks.
//
// As you go through all the blocks, you can check to see if it's a free
// block by looking at the high order bit of the next block index. You can
// also see how big the block is by subtracting the next block index from
// the current block number.
//
// The umm_info function does all of that and makes the results available
// in the heapInfo structure.
// ----------------------------------------------------------------------------

UMM_HEAP_INFO heapInfo;

void *umm_info( void *ptr, int force ) {

  unsigned short int blockNo = 0;

  // Protect the critical section...
  //
  UMM_CRITICAL_ENTRY();
  
  // Clear out all of the entries in the heapInfo structure before doing
  // any calculations..
  //
  memset( &heapInfo, 0, sizeof( heapInfo ) );

  DBG_LOG_FORCE( force, "\n\nDumping the umm_heap...\n" );

  DBG_LOG_FORCE( force, "|0x%08x|B %5i|NB %5i|PB %5i|Z %5i|NF %5i|PF %5i|\n",
                        (unsigned int)(&UMM_BLOCK(blockNo)),
                        blockNo,
                        UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
                        UMM_PBLOCK(blockNo),
                        (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK )-blockNo,
                        UMM_NFREE(blockNo),
                        UMM_PFREE(blockNo) );

  // Now loop through the block lists, and keep track of the number and size
  // of used and free blocks. The terminating condition is an nb pointer with
  // a value of zero...
  
  blockNo = UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK;

  while( UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK ) {
    ++heapInfo.totalEntries;
    heapInfo.totalBlocks += (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK )-blockNo;

    // Is this a free block?

    if( UMM_NBLOCK(blockNo) & UMM_FREELIST_MASK ) {
      ++heapInfo.freeEntries;
      heapInfo.freeBlocks += (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK )-blockNo;

      DBG_LOG_FORCE( force, "|0x%08x|B %5i|NB %5i|PB %5i|Z %5i|NF %5i|PF %5i|\n",
                            (unsigned int)(&UMM_BLOCK(blockNo)),
                            blockNo,
                            UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
                            UMM_PBLOCK(blockNo),
                            (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK )-blockNo,
                            UMM_NFREE(blockNo),
                            UMM_PFREE(blockNo) );
     
      // Does this block address match the ptr we may be trying to free?

      if( ptr == &UMM_BLOCK(blockNo) ) {
       
        // Release the critical section...
        //
        UMM_CRITICAL_EXIT();
 
        return( ptr );
      }
    } else {
      ++heapInfo.usedEntries;
      heapInfo.usedBlocks += (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK )-blockNo;

      DBG_LOG_FORCE( force, "|0x%08x|B %5i|NB %5i|PB %5i|Z %5i|\n",
                            (unsigned int)(&UMM_BLOCK(blockNo)),
                            blockNo,
                            UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
                            UMM_PBLOCK(blockNo),
                            (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK )-blockNo );
    }

    blockNo = UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK;
  }

  // Update the accounting totals with information from the last block, the
  // rest must be free!

  heapInfo.freeBlocks  += UMM_NUMBLOCKS-blockNo;
  heapInfo.totalBlocks += UMM_NUMBLOCKS-blockNo;

  DBG_LOG_FORCE( force, "|0x%08x|B %5i|NB %5i|PB %5i|Z %5i|NF %5i|PF %5i|\n",
                        (unsigned int)(&UMM_BLOCK(blockNo)),
                        blockNo,
                        UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
                        UMM_PBLOCK(blockNo),
                        UMM_NUMBLOCKS-blockNo,
                        UMM_NFREE(blockNo),
                        UMM_PFREE(blockNo) );

  DBG_LOG_FORCE( force, "Total Entries %5i    Used Entries %5i    Free Entries %5i\n",
                        heapInfo.totalEntries,
                        heapInfo.usedEntries,
                        heapInfo.freeEntries );

  DBG_LOG_FORCE( force, "Total Blocks  %5i    Used Blocks  %5i    Free Blocks  %5i\n",
                        heapInfo.totalBlocks,
                        heapInfo.usedBlocks,
                        heapInfo.freeBlocks  );

  // Release the critical section...
  //
  UMM_CRITICAL_EXIT();
 
  return( NULL );
}

// ----------------------------------------------------------------------------

static unsigned short int umm_blocks( size_t size ) {

  // The calculation of the block size is not too difficult, but there are
  // a few little things that we need to be mindful of.
  //
  // When a block removed from the free list, the space used by the free
  // pointers is available for data. That's what the first calculation
  // of size is doing.

  if( size <= (sizeof(((umm_block *)0)->body)) )
    return( 1 );

  // If it's for more than that, then we need to figure out the number of
  // additional whole blocks the size of an umm_block are required.

  size -= ( 1 + (sizeof(((umm_block *)0)->body)) );

  return( 2 + size/(sizeof(umm_block)) );
}

// ----------------------------------------------------------------------------

static void umm_make_new_block( unsigned short int c,
                                unsigned short int blocks,
                                unsigned short int freemask ) {

     UMM_NBLOCK(c+blocks) = UMM_NBLOCK(c) & UMM_BLOCKNO_MASK;
     UMM_PBLOCK(c+blocks) = c;

     UMM_PBLOCK(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) = (c+blocks);
     UMM_NBLOCK(c)                                = (c+blocks) | freemask;
}

// ----------------------------------------------------------------------------

static void umm_disconnect_from_free_list( unsigned short int c ) {
    // Disconnect this block from the FREE list

    UMM_NFREE(UMM_PFREE(c)) = UMM_NFREE(c);
    UMM_PFREE(UMM_NFREE(c)) = UMM_PFREE(c);

    // And clear the free block indicator

    UMM_NBLOCK(c) &= (~UMM_FREELIST_MASK);
}

// ----------------------------------------------------------------------------

static void umm_assimilate_up( unsigned short int c ) {

  if( UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_FREELIST_MASK ) {
    // The next block is a free block, so assimilate up and remove it from
    // the free list

    DBG_LOG_DEBUG( "Assimilate up to next block, which is FREE\n" );

    // Disconnect the next block from the FREE list

    umm_disconnect_from_free_list( UMM_NBLOCK(c) );

    // Assimilate the next block with this one

    UMM_PBLOCK(UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_BLOCKNO_MASK) = c;
    UMM_NBLOCK(c) = UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_BLOCKNO_MASK;
  } 
}

// ----------------------------------------------------------------------------

static unsigned short int umm_assimilate_down( unsigned short int c, unsigned short int freemask ) {

    UMM_NBLOCK(UMM_PBLOCK(c)) = UMM_NBLOCK(c) | freemask;
    UMM_PBLOCK(UMM_NBLOCK(c)) = UMM_PBLOCK(c);

    return( UMM_PBLOCK(c) );
}

// ----------------------------------------------------------------------------

void umm_free( void *ptr ) {

  unsigned short int c;

  // If we're being asked to free a NULL pointer, well that's just silly!

  if( (void *)0 == ptr ) {
    DBG_LOG_DEBUG( "free a null pointer -> do nothing\n" );

  return;
  }

  // FIXME: At some point it might be a good idea to add a check to make sure
  //        that the pointer we're being asked to free up is actually within
  //        the umm_heap!
  //
  // NOTE:  See the new umm_info() function that you can use to see if a ptr is
  //        on the free list!

  // Protect the critical section...
  //
  UMM_CRITICAL_ENTRY();

  // Figure out which block we're in. Note the use of truncated division...

  c = (ptr-(void *)(&(umm_heap[0])))/sizeof(umm_block);

  DBG_LOG_DEBUG( "Freeing block %6i\n", c );

  // Now let's assimilate this block with the next one if possible.

  umm_assimilate_up( c );

  // Then assimilate with the previous block if possible

  if( UMM_NBLOCK(UMM_PBLOCK(c)) & UMM_FREELIST_MASK ) {

    DBG_LOG_DEBUG( "Assimilate down to next block, which is FREE\n" );

    c = umm_assimilate_down(c, UMM_FREELIST_MASK);
  } else {
    // The previous block is not a free block, so add this one to the head
    // of the free list

    DBG_LOG_DEBUG( "Just add to head of free list\n" );

    UMM_PFREE(UMM_NFREE(0)) = c;
    UMM_NFREE(c)            = UMM_NFREE(0);
    UMM_PFREE(c)            = 0;
    UMM_NFREE(0)            = c;

    UMM_NBLOCK(c)          |= UMM_FREELIST_MASK;
  }

#if(0)
  // The following is experimental code that checks to see if the block we just 
  // freed can be assimilated with the very last block - it's pretty convoluted in
  // terms of block index manipulation, and has absolutely no effect on heap
  // fragmentation. I'm not sure that it's worth including but I've left it
  // here for posterity.

  if( 0 == UMM_NBLOCK(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK ) ) {

   if( UMM_PBLOCK(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) != UMM_PFREE(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) ) {
      UMM_NFREE(UMM_PFREE(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK)) = c;
      UMM_NFREE(UMM_PFREE(c))                                = UMM_NFREE(c);
      UMM_PFREE(UMM_NFREE(c))                                = UMM_PFREE(c);
      UMM_PFREE(c)                                           = UMM_PFREE(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK);
    }

    UMM_NFREE(c)  = 0;
    UMM_NBLOCK(c) = 0;
  }
#endif

  // Release the critical section...
  //
  UMM_CRITICAL_EXIT();
}

// ----------------------------------------------------------------------------

void *umm_malloc( size_t size ) {

           unsigned short int blocks;
  volatile unsigned short int blockSize;

           unsigned short int bestSize;
           unsigned short int bestBlock;

           unsigned short int cf;

  // the very first thing we do is figure out if we're being asked to allocate
  // a size of 0 - and if we are we'll simply return a null pointer. if not
  // then reduce the size by 1 byte so that the subsequent calculations on
  // the number of blocks to allocate are easier...

  if( 0 == size ) {
    DBG_LOG_DEBUG( "malloc a block of 0 bytes -> do nothing\n" );
  
    return( (void *)NULL );
  }

  // Protect the critical section...
  //
  UMM_CRITICAL_ENTRY();

  blocks = umm_blocks( size );

  // Now we can scan through the free list until we find a space that's big
  // enough to hold the number of blocks we need.
  //
  // This part may be customized to be a best-fit, worst-fit, or first-fit
  // algorithm

  cf = UMM_NFREE(0);

  bestBlock = UMM_NFREE(0);
  bestSize  = 0x7FFF;

  while( UMM_NFREE(cf) ) {
    blockSize = (UMM_NBLOCK(cf) & UMM_BLOCKNO_MASK) - cf;

    DBG_LOG_TRACE( "Looking at block %6i size %6i\n", cf, blockSize );

#if defined UMM_FIRST_FIT
    // This is the first block that fits!
    if( (blockSize >= blocks) )
        break;
#elif defined UMM_BEST_FIT
    if( (blockSize >= blocks) && (blockSize < bestSize) ) {
      bestBlock = cf;
      bestSize  = blockSize;
    }
#endif

    cf = UMM_NFREE(cf);
  }

  if( 0x7FFF != bestSize ) {
    cf        = bestBlock;
    blockSize = bestSize;
  }

  if( UMM_NBLOCK(cf) & UMM_BLOCKNO_MASK ) {
    // This is an existing block in the memory heap, we just need to split off
    // what we need, unlink it from the free list and mark it as in use, and
    // link the rest of the block back into the freelist as if it was a new
    // block on the free list...

    if( blockSize == blocks ) {
      // It's an exact fit and we don't neet to split off a block.
      DBG_LOG_DEBUG( "Allocating %6i blocks starting at %6i - exact\n", blocks, cf );

      // Disconnect this block from the FREE list

      umm_disconnect_from_free_list( cf );

    } else {
     // It's not an exact fit and we need to split off a block.
     DBG_LOG_DEBUG( "Allocating %6i blocks starting at %6i - existing\n", blocks, cf );

     umm_make_new_block( cf, blockSize-blocks, UMM_FREELIST_MASK );

     cf += blockSize-blocks;
     }
  } else {
    // We're at the end of the heap - allocate a new block, but check to see if
    // there's enough memory left for the requested block! Actually, we may need
    // one more than that if we're initializing the umm_heap for the first
    // time, which happens in the next conditional...

    if( UMM_NUMBLOCKS <= cf+blocks+1 ) {
      DBG_LOG_DEBUG(  "Can't allocate %5i blocks at %5i\n", blocks, cf );

      // Release the critical section...
      //
      UMM_CRITICAL_EXIT();

      return( (void *)NULL );
    }

    // Now check to see if we need to initialize the free list...this assumes
    // that the BSS is set to 0 on startup. We should rarely get to the end of
    // the free list so this is the "cheapest" place to put the initialization!

    if( 0 == cf ) {
      DBG_LOG_DEBUG( "Initializing malloc free block pointer\n" );
      UMM_NBLOCK(0) = 1;
      UMM_NFREE(0)  = 1;
      cf            = 1;
    }

    DBG_LOG_DEBUG( "Allocating %6i blocks starting at %6i - new     \n", blocks, cf );

    UMM_NFREE(UMM_PFREE(cf)) = cf+blocks;

    memcpy( &UMM_BLOCK(cf+blocks), &UMM_BLOCK(cf), sizeof(umm_block) );

    UMM_NBLOCK(cf)           = cf+blocks;
    UMM_PBLOCK(cf+blocks)    = cf;
  }

  // Release the critical section...
  //
  UMM_CRITICAL_EXIT();

  return( (void *)&UMM_DATA(cf) );
}

// ----------------------------------------------------------------------------

void *umm_realloc( void *ptr, size_t size ) {

  unsigned short int blocks;
  unsigned short int blockSize;

  unsigned short int c;

  size_t curSize;

  // This code looks after the case of a NULL value for ptr. The ANSI C
  // standard says that if ptr is NULL and size is non-zero, then we've
  // got to work the same a malloc(). If size is also 0, then our version
  // of malloc() returns a NULL pointer, which is OK as far as the ANSI C
  // standard is concerned.

  if( ((void *)NULL == ptr) ) {
    DBG_LOG_DEBUG( "realloc the NULL pointer - call malloc()\n" );

    return( umm_malloc(size) );
  }

  // Now we're sure that we have a non_NULL ptr, but we're not sure what
  // we should do with it. If the size is 0, then the ANSI C standard says that
  // we should operate the same as free.

  if( 0 == size ) {
    DBG_LOG_DEBUG( "realloc to 0 size, just free the block\n" );

    umm_free( ptr );
    
    return( (void *)NULL );
  }

  // Protect the critical section...
  //
  UMM_CRITICAL_ENTRY();

  // Otherwise we need to actually do a reallocation. A naiive approach
  // would be to malloc() a new block of the correct size, copy the old data
  // to the new block, and then free the old block.
  //
  // While this will work, we end up doing a lot of possibly unnecessary
  // copying. So first, let's figure out how many blocks we'll need.

  blocks = umm_blocks( size );

  // Figure out which block we're in. Note the use of truncated division...

  c = (ptr-(void *)(&(umm_heap[0])))/sizeof(umm_block);

  // Figure out how big this block is...

  blockSize = (UMM_NBLOCK(c) - c);

  // Figure out how many bytes are in this block
    
  curSize   = (blockSize*sizeof(umm_block))-(sizeof(((umm_block *)0)->header));

  // Ok, now that we're here, we know the block number of the original chunk
  // of memory, and we know how much new memory we want, and we know the original
  // block size...

  if( blockSize == blocks ) {
    // This space intentionally left blank - return the original pointer!

    DBG_LOG_DEBUG( "realloc the same size block - %i, do nothing\n", blocks );

    // Release the critical section...
    //
    UMM_CRITICAL_EXIT();

    return( ptr );
  }

  // Now we have a block size that could be bigger or smaller. Either
  // way, try to assimilate up to the next block before doing anything...
  //
  // If it's still too small, we have to free it anyways and it will save the
  // assimilation step later in free :-)

  umm_assimilate_up( c );

  // Now check if it might help to assimilate down, but don't actually
  // do the downward assimilation unless the resulting block will hold the
  // new request! If this block of code runs, then the new block will
  // either fit the request exactly, or be larger than the request.

  if( (UMM_NBLOCK(UMM_PBLOCK(c)) & UMM_FREELIST_MASK) &&
      (blocks <= (UMM_NBLOCK(c)-UMM_PBLOCK(c)))    ) {
  
    // Check if the resulting block would be big enough...

    DBG_LOG_DEBUG( "realloc() could assimilate down %i blocks - fits!\n\r", c-UMM_PBLOCK(c) );

    // Disconnect the previous block from the FREE list

    umm_disconnect_from_free_list( UMM_PBLOCK(c) );

    // Connect the previous block to the next block ... and then
    // realign the current block pointer

    c = umm_assimilate_down(c, 0);

    // Move the bytes down to the new block we just created, but be sure to move
    // only the original bytes.

    memmove( (void *)&UMM_DATA(c), ptr, curSize );
 
    // And don't forget to adjust the pointer to the new block location!

    ptr    = (void *)&UMM_DATA(c);
  }

  // Now calculate the block size again...and we'll have three cases

  blockSize = (UMM_NBLOCK(c) - c);

  if( blockSize == blocks ) {
    // This space intentionally left blank - return the original pointer!

    DBG_LOG_DEBUG( "realloc the same size block - %i, do nothing\n", blocks );

  } else if (blockSize > blocks ) {
    // New block is smaller than the old block, so just make a new block
    // at the end of this one and put it up on the free list...

    DBG_LOG_DEBUG( "realloc %i to a smaller block %i, shrink and free the leftover bits\n", blockSize, blocks );

    umm_make_new_block( c, blocks, 0 );
    
    umm_free( (void *)&UMM_DATA(c+blocks) );
  } else {
    // New block is bigger than the old block...
    
    void *oldptr = ptr;

    DBG_LOG_DEBUG( "realloc %i to a bigger block %i, make new, copy, and free the old\n", blockSize, blocks );

    // Now umm_malloc() a new/ one, copy the old data to the new block, and
    // free up the old block, but only if the malloc was sucessful!

    if( (ptr = umm_malloc( size )) ) {
       memcpy( ptr, oldptr, curSize );
    }

    umm_free( oldptr );
  }

  // Release the critical section...
  //
  UMM_CRITICAL_EXIT();

  return( ptr );
}

// ----------------------------------------------------------------------------

#ifdef UMM_TEST_MAIN

main() {

  void * ptr_array[256];

  size_t i;

  int idx;

  printf( "Size of umm_heap is %i\n", sizeof(umm_heap)       );
  printf( "Size of header   is %i\n", sizeof(umm_heap[0])    );
  printf( "Size of nblock   is %i\n", sizeof(umm_heap[0].header.used.next) );
  printf( "Size of pblock   is %i\n", sizeof(umm_heap[0].header.used.prev) );
  printf( "Size of nfree    is %i\n", sizeof(umm_heap[0].body.free.next)   );
  printf( "Size of pfree    is %i\n", sizeof(umm_heap[0].body.free.prev)   );

  memset( umm_heap, 0, sizeof(umm_heap) );

  umm_info( NULL, 1 );
  
   for( idx=0; idx<256; ++idx )
     ptr_array[idx] = (void *)NULL;
 
   for( idx=0; idx<6553500; ++idx ) {
     i = rand()%256;
 
     switch( rand() % 16 ) {
 
     case  0:
     case  1:
     case  2:
     case  3:
     case  4:
     case  5:
     case  6: ptr_array[i] = umm_realloc(ptr_array[i], 0);
                break;
     case  7:
     case  8: ptr_array[i] = umm_realloc(ptr_array[i], rand()%40 );
                break;
 
     case  9:
     case 10:
     case 11:
     case 12: ptr_array[i] = umm_realloc(ptr_array[i], rand()%100 );
                break;
 
     case 13:
     case 14: ptr_array[i] = umm_realloc(ptr_array[i], rand()%200 );
                break;
 
     default: ptr_array[i] = umm_realloc(ptr_array[i], rand()%400 );
              break;
       }
 
     }

  umm_info( NULL, 1 );

}

#endif
