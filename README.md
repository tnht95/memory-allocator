# Memory Allocator

This project is a custom **Memory Allocator** implemented in C. The allocator manages memory allocation and deallocation efficiently, providing an alternative to standard system allocators.

# Memory Allocator

This project is a custom **Memory Allocator** implemented in C/C++ (or other specified language). The allocator manages memory allocation and deallocation efficiently, providing an alternative to standard system allocators.

## Features

### Current Implementation
- **Implicit Free List**: Uses a linear structure to keep track of allocated and free memory blocks. Efficient for small workloads and simple use cases.
- **Explicit Free List**: Maintains a linked list of free blocks, improving performance by reducing the search time for free memory during allocation.
- **First-Fit Allocation Strategy**: Allocates the first suitable free block to minimize search overhead.
- **Coalescing**: Merges adjacent free blocks  to minimize fragmentation and improve memory reuse.
- **Block Splitting**: Splits larger free blocks when a smaller allocation is requested to optimize memory usage.

### Minimum Block Size
The minimum block size is **4 words** (32 bytes on a 64-bit architecture):
- **1 word** for the header.
- **1 word** for the footer.
- **2 words** for the free list pointers (used in explicit free lists).

### Memory Block Structure
Each memory block managed by the allocator includes the following components:
- **Header**: Contains metadata such as the block size and allocation status.
- **Data**: The usable memory space allocated to the user.
- **Footer**: Stores metadata (like the block size) to facilitate boundary tagging for coalescing and mirrors critical metadata from the header for backward traversal.

Here is a representation of the memory block structure:
```
+-----------------+----------------+-----------------+
|     Header      |      Data      |     Footer      |
+-----------------+----------------+-----------------+
```


### Enhancements
- **Boundary Tagging**: Facilitates coalescing by storing metadata at both the start and end of blocks, improving efficiency.
- **Alignment Support**: Ensures memory blocks meet alignment requirements for specific data types or system architecture.
- **Error Handling**: Detects invalid memory access, double free attempts, and buffer overflows for robust behavior.
- **Detailed Statistics**: Tracks metrics such as memory utilization, fragmentation percentage, and allocation counts for analysis and optimization.

## Future Plans
- **Thread Safety**: Add synchronization mechanisms such as mutexes or lock-free data structures to support multi-threaded applications.
- **Segregated Free Lists**: Use multiple free lists categorized by block size to further reduce search time for memory allocation.
- **Memory Mapping and Paging**: Extend support for custom memory pools, file-backed memory regions, or large-page allocations for specialized workloads.
- **Performance Benchmarking**: Include benchmarks comparing the allocator's performance to standard library allocators under various workloads.

## Usage

Clone the repository and build the project:
```bash
$ git clone https://github.com/yourusername/memory-allocator.git
$ cd memory-allocator
$ make
```

Link the custom allocator with your program:
```bash
$ gcc -o your_program your_program.c
```

### Example

```c
#include "allocator.h"

int main() {
    void *ptr = new_malloc(64); // Allocate 64 bytes
    new_free(ptr);              // Free the allocated memory
    return 0;
}
```

## Testing

Run the included test suite to verify functionality:
```bash
$ make test
```
