#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Align the size to the nearest multiple of the machine word size (typically 8 bytes on 64-bit machines).
#define ALIGN(size) (((size) + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1))

// Define the minimum block size: 16 bytes for free list pointers + 16 bytes for header/footer.
#define MIN_BLOCK_SIZE 32

// Helper macros to work with metadata.
#define GET_BLOCK_SIZE(meta) (meta >> 1)  // Extract the size from metadata.
#define SET_FREE(meta) (meta | 1)  // Mark the block as free.
#define SET_METADATA(size,  is_free) ((size << 1) | is_free)
#define MAX(a,b) (((a)>(b))?(a):(b))

// Memory block metadata structure (header/footer encoded in one word).
typedef struct HeaderFooter {
    size_t metadata; // Encodes size and availability (last bit: 1 = free, 0 = allocated).
} HeaderFooter;

// Free block structure used in the explicit free list.
typedef struct FreeBlock {
    HeaderFooter header; // Size and allocation status encoded here.
    struct FreeBlock* next; // Pointer to the next free block.
    struct FreeBlock* prev; // Pointer to the previous free block.
} FreeBlock;

// Function prototypes.
void* new_malloc(size_t size);
void new_free(void *ptr);
FreeBlock* coalesce(FreeBlock *block);
uint8_t* split_block(FreeBlock *block, size_t total_size);
uint8_t* initialize_allocator(size_t size);
void free_allocator(uint8_t *mem_pool);
void remove_from_free_list(FreeBlock *free_block);
void add_to_free_list(FreeBlock *free_block);
// The start of the free list.
static FreeBlock* free_list = NULL;
// Track the highest initialized address in the pool.
static uint8_t* highest_addr = 0;
// Memory pool (example size: 5 MB).
uint8_t *memory_pool;
#endif