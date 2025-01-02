#include "allocator.h"

// Initialize the allocator by setting up the memory pool.
uint8_t *initialize_allocator(size_t pool_size) {
    pool_size = MAX(MIN_BLOCK_SIZE, pool_size);
    memory_pool = malloc(pool_size);
    if (!memory_pool) {
        fprintf(stderr, "Failed to allocate memory pool.\n");
        exit(EXIT_FAILURE);
    }

    // Entire pool is initially one free block.
    free_list = (FreeBlock *) memory_pool;
    free_list->header.metadata = SET_METADATA(pool_size, 1);

    free_list->next = NULL;
    free_list->prev = NULL;
    highest_addr = memory_pool;

    // Set the footer to match the header for easy coalescing.
    HeaderFooter *footer = (HeaderFooter *) ((uint8_t *) free_list + pool_size - sizeof(HeaderFooter));
    footer->metadata = SET_METADATA(pool_size, 1);
    return memory_pool;
}

// Allocate memory of at least `size` bytes.
void *new_malloc(size_t size) {
    if (size == 0) return NULL;

    size = ALIGN(size); // Align the requested size.
    // Include header and footer.
    // Make sure the size is sufficient for Explicit Free List
    size_t total_size = MAX(size + 2 * sizeof(HeaderFooter), MIN_BLOCK_SIZE);
    FreeBlock *current = free_list;

    while (current) {
        size_t current_size = GET_BLOCK_SIZE(current->header.metadata);
        if (current_size >= total_size) {
            uint8_t *result = split_block(current, total_size); // Split the block if necessary.
            return result; // Return the user space, not the header
        }
        current = current->next; // Move to the next free block
    }

    return NULL; // No suitable block found.
}

void new_free(void *ptr) {
    if (!ptr) return;
    // get the header
    FreeBlock *block = (FreeBlock *) ((uint8_t *) ptr - sizeof(HeaderFooter));
    block->header.metadata = SET_FREE(block->header.metadata);

    HeaderFooter *footer = (HeaderFooter *) ((uint8_t *) block + GET_BLOCK_SIZE(block->header.metadata) -
                                             sizeof(HeaderFooter));
    footer->metadata = block->header.metadata;

    // Attempt to merge with adjacent free blocks.
    FreeBlock *coalesced_block = coalesce(block);
    add_to_free_list(coalesced_block);

}

// Coalesce adjacent free blocks to avoid fragmentation.
FreeBlock *coalesce(FreeBlock *block) {
    FreeBlock *result = block;
    size_t block_size = GET_BLOCK_SIZE(block->header.metadata);

    // Try to coalesce with the next block.
    HeaderFooter *next_header = (HeaderFooter *) ((uint8_t *) block + block_size);
    if ((uint8_t *) next_header <= highest_addr) {

        size_t next_size = GET_BLOCK_SIZE(next_header->metadata);
        if (next_header->metadata & 1) { // Check if the next block is free.
            block_size += next_size;
            block->header.metadata = SET_METADATA(block_size, 1);
            HeaderFooter *footer = (HeaderFooter *) ((uint8_t *) block + block_size - sizeof(HeaderFooter));
            footer->metadata = SET_METADATA(block_size, 1);

            // Remove the next block from the free list.
            FreeBlock *next = (FreeBlock *) next_header;
            remove_from_free_list(next);
        }
        result = block;
    }

    // Try to coalesce with the previous block.
    HeaderFooter *prev_footer = (HeaderFooter *) ((uint8_t *) block - sizeof(HeaderFooter));
    if ((uint8_t *) prev_footer >= memory_pool) {
        size_t prev_size = GET_BLOCK_SIZE(prev_footer->metadata);
        if (prev_footer->metadata & 1) { // Check if the previous block is free.

            FreeBlock *prev = (FreeBlock *) ((uint8_t *) block - prev_size);
            block_size += prev_size;
            prev->header.metadata = SET_METADATA(block_size, 1);
            HeaderFooter *footer = (HeaderFooter *) ((uint8_t *) prev + block_size - sizeof(HeaderFooter));
            footer->metadata = SET_METADATA(block_size, 1);

            remove_from_free_list(prev);
            result = prev;
        }
    }

    return result;
}


// Split a free block into two if it is large enough.
uint8_t *split_block(FreeBlock *block, size_t total_size) {
    size_t block_size = GET_BLOCK_SIZE(block->header.metadata);
    size_t remaining_free_size = block_size - total_size;

    remove_from_free_list(block);

    // check if the remaining free block too small to store the free list
    if (remaining_free_size >= MIN_BLOCK_SIZE) {
        FreeBlock *free_block = (FreeBlock *) ((uint8_t *) block + total_size);
        free_block->header.metadata = SET_METADATA(remaining_free_size, 1);
        HeaderFooter *footer = (HeaderFooter *) ((uint8_t *) free_block + remaining_free_size - sizeof(HeaderFooter));
        footer->metadata = SET_METADATA(remaining_free_size, 1);


        // return this block for user
        block->header.metadata = SET_METADATA(total_size, 0);
        HeaderFooter *block_footer = (HeaderFooter *) ((uint8_t *) block + total_size - sizeof(HeaderFooter));
        block_footer->metadata = SET_METADATA(total_size, 0);

        // Update the high watermark if this allocation extends it.
        highest_addr = MAX(highest_addr, (uint8_t *) block + total_size);

        // join and add free block to free list
        FreeBlock *coalesced_free_block = coalesce(free_block);
        add_to_free_list(coalesced_free_block);
        return (uint8_t *) block + sizeof(HeaderFooter);
    }

    // consume the whole free chunk
    block->header.metadata = SET_METADATA(total_size, 0);
    HeaderFooter *block_footer = (HeaderFooter *) ((uint8_t *) block + total_size - sizeof(HeaderFooter));
    block_footer->metadata = SET_METADATA(total_size, 0);

    highest_addr = MAX(highest_addr, (uint8_t *) block + total_size);


    return (uint8_t *) block + sizeof(HeaderFooter);
}

void free_allocator(uint8_t *mem_pool) {
    free(mem_pool);
}

void add_to_free_list(FreeBlock *free_block) {
    free_block->next = free_list;
    if (free_list) {
        free_list->prev = free_block;
    }
    free_block->prev = NULL;
    free_list = free_block;
}

void remove_from_free_list(FreeBlock *block) {
    if (!block) return;

    if (block->prev) {
        block->prev->next = block->next;
    } else {
        free_list = block->next; // If no `prev`, it was the head of the list.
    }

    if (block->next) {
        block->next->prev = block->prev;
    }

    block->next = block->prev = NULL; // Clear pointers to prevent dangling references.
}
