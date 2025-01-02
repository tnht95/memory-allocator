#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "allocator.h"


void test_basic_allocation_and_free() {
    uint8_t *pool = initialize_allocator(10);

    void* ptr1 = new_malloc(32);
    printf("Allocated 32 bytes at %p\n", ptr1);

    void* ptr2 = new_malloc(64);
    printf("Allocated 64 bytes at %p\n", ptr2);

    new_free(ptr1);
    printf("Freed 32 bytes\n");

    new_free(ptr2);
    printf("Freed 64 bytes\n");

    free_allocator(pool);
}


void test_fragmentation_and_coalescing() {
    uint8_t *pool = initialize_allocator(100);

    void* ptr1 = new_malloc(32);
    printf("Allocated 32 bytes at %p\n", ptr1);

    void* ptr2 = new_malloc(64);
    printf("Allocated 64 bytes at %p\n", ptr2);

    new_free(ptr1);
    printf("Freed 32 bytes\n");

    new_free(ptr2);
    printf("Freed 64 bytes\n");

    void* ptr3 = new_malloc(80);
    printf("Allocated 80 bytes (should reuse coalesced block) at %p\n", ptr3);

    free_allocator(pool);

}


void test_large_allocation() {
    uint8_t *pool = initialize_allocator(100);

    void* ptr1 = new_malloc(1024 * 1024 - 32); // Almost the entire pool
    printf("Allocated large block: %p\n", ptr1);

    void* ptr2 = new_malloc(64); // Should fail
    printf("Allocation for 64 bytes (should fail): %p\n", ptr2);

    new_free(ptr1);
    printf("Freed large block\n");

    free_allocator(pool);

}


void test_alignment() {
    uint8_t *pool = initialize_allocator(100);

    void* ptr = new_malloc(1); // Request 1 byte
    printf("Allocated 1 byte at %p\n", ptr);

    if ((uintptr_t)ptr % sizeof(size_t) == 0) {
        printf("Pointer is correctly aligned\n");
    } else {
        printf("Pointer is not aligned\n");
    }

    new_free(ptr);
    printf("Freed 1 byte\n");

    free_allocator(pool);

}


void test_splitting_blocks() {
    uint8_t *pool = initialize_allocator(64);

    void* ptr1 = new_malloc(128); // Allocate a large block
    printf("Allocated 128 bytes at %p\n", ptr1);

    void* ptr2 = new_malloc(32); // Allocate a smaller block, causing a split
    printf("Allocated 32 bytes at %p\n", ptr2);

    new_free(ptr1);
    printf("Freed 128 bytes\n");

    new_free(ptr2);
    printf("Freed 32 bytes\n");

    free_allocator(pool);

}



void test_reuse_freed_blocks() {
    uint8_t *pool = initialize_allocator(4000);

    void* ptr1 = new_malloc(64);
    printf("Allocated 64 bytes at %p\n", ptr1);

    new_free(ptr1);
    printf("Freed 64 bytes\n");

    void* ptr2 = new_malloc(32); // Should reuse the freed block
    printf("Allocated 32 bytes at %p (reuse freed block)\n", ptr2);

    new_free(ptr2);
    printf("Freed 32 bytes\n");

    free_allocator(pool);

}


void test_stress() {
    printf("Test: Stress Test\n");
    uint8_t *pool = initialize_allocator(64000);

    void* ptrs[200];
    for (int i = 0; i < 5; ++i) {
        size_t size = (rand() % 128) + 16;
        ptrs[i] = new_malloc(size);
        printf("Allocated %zu bytes at %p\n", size, ptrs[i]);
    }

    for (int i = 0; i < 5; ++i) {
        if (rand() % 2) {
            new_free(ptrs[i]);
            printf("Freed block at %p\n", ptrs[i]);
        }
    }
    for (int i = 0; i < 5; ++i) {
        size_t size = (rand() % 128) + 16;
        ptrs[i] = new_malloc(size);
        printf("Allocated %zu bytes at %p\n", size, ptrs[i]);
    }


    free_allocator(pool);
}



void run_tests() {
    printf("Running tests...\n");

    test_basic_allocation_and_free();
    test_fragmentation_and_coalescing();
    test_large_allocation();
    test_alignment();
    test_splitting_blocks();
    test_reuse_freed_blocks();
    test_stress();

    printf("All tests completed.\n");
}

int main() {
    run_tests();

    return 0;

}
