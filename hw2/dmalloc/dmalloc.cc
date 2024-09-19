#define M61_DISABLE 1
#include "dmalloc.hh"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cinttypes>
#include <cassert>
#include<limits.h>
#include <unordered_map>
#include <utility>

// (Helper functions, types, structs, macros, globals, etc.)
std::unordered_map<uintptr_t, std::pair<const char*, long>> lines;
std::unordered_map<uintptr_t, size_t> mallocsizes;

unsigned long long nactive = 0;         // # active allocations
unsigned long long active_size= 0;     // # bytes in active allocations
unsigned long long ntotal = 0;          // # total allocations
unsigned long long total_size = 0;      // # bytes in total allocations
unsigned long long nfail = 0;           // # failed allocation attempts
unsigned long long fail_size = 0;       // # bytes in failed alloc attempts
uintptr_t heap_min = 0xDEADBEEF;                 // smallest allocated addr
uintptr_t heap_max = 0xDEADBEEF;                 // largest allocated addr
size_t size_buffer = 8;
size_t size_canary = 8;
uintptr_t canary_value = 0xDEADBEEF;

/// dmalloc_malloc(sz, file, line)
///    Return a pointer to `sz` bytes of newly-allocated dynamic memory.
///    The memory is not initialized. If `sz == 0`, then dmalloc_malloc must
///    return a unique, newly-allocated pointer value. The allocation
///    request was at location `file`:`line`.

void* dmalloc_malloc(size_t sz, const char* file, long line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings
    void* ptr = base_malloc(sz + size_buffer + 2 * size_canary);
    
    if (ptr == NULL || (size_t) -1 <= sz) {
        nfail += 1;
        fail_size += sz;
        return NULL;
    }

    *(size_t*) ptr = sz;
    size_t* underflow_canary = (size_t*) ((uintptr_t) ptr + size_buffer);
    *underflow_canary = canary_value;

    total_size += sz;
    active_size += sz;
    ntotal += 1;
    nactive += 1;

    void* after = (void*) ((uintptr_t) ptr + size_buffer + size_canary);
    size_t* overflow_canary = (size_t*) ((uintptr_t) after + sz);
    *overflow_canary = canary_value;

    if (heap_min == 0xDEADBEEF || (uintptr_t) ptr < heap_min) {
        heap_min = (uintptr_t) ptr;
    }
    if (heap_max == 0xDEADBEEF || (uintptr_t) after + sz > heap_max) {
        heap_max = (uintptr_t) after + sz;
    }

    auto pair = std::make_pair(file, line);
    mallocsizes[(uintptr_t) after] = sz;
    lines[(uintptr_t) after] = pair;
    return after;
}


/// dmalloc_free(ptr, file, line)
///    Free the memory space pointed to by `ptr`, which must have been
///    returned by a previous call to dmalloc_malloc. If `ptr == NULL`,
///    does nothing. The free was called at location `file`:`line`.

void dmalloc_free(void* ptr, const char* file, long line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings
    // Your code here.
    if (ptr == NULL) {
        return;
    }

    if ((uintptr_t) ptr > heap_max || (uintptr_t) ptr < heap_min) {
        fprintf(stderr, "MEMORY BUG: %s:%ld: invalid free of pointer %p, not in heap", file, line, ptr);
        exit(1);
    }

    auto i1 = lines.find((uintptr_t) ptr);
    auto i2 = mallocsizes.find((uintptr_t) ptr);
    
    if (i1 != lines.end()){
        lines.erase(i1);
        // mallocsizes.erase(i2);
    }
    else {
        if (i2 != mallocsizes.end()) {
            fprintf(stderr, "MEMORY BUG: %s:%ld: invalid free of pointer %p, double free", file, line, ptr);
            exit(1);
        }
        else {
            fprintf(stderr, "MEMORY BUG: %s:%ld: invalid free of pointer %p, not allocated", file, line, ptr);
            exit(1);
        }
    }
    

    size_t ptr_size = *(size_t*) ((uintptr_t)ptr - size_buffer - size_canary);
    size_t* overflow_canary = (size_t*) ((uintptr_t) ptr + ptr_size);
    size_t* underflow_canary = (size_t*) ((uintptr_t) ptr - size_canary);
    // printf("%p", overflow_canary);
    // printf("%p", underflow_canary);

    if (*underflow_canary != canary_value || *overflow_canary != canary_value) {
        fprintf(stderr, "MEMORY BUG: %s:%ld: detected wild write during free of pointer %p", file, line, ptr);
        exit(1);
    }
    nactive -= 1;

    // if (*overflow_canary != 0xDEADBEEF || *underflow_canary != 0xDEADBEEF){
    //     fprintf(stderr, "Boundary Write Error");
    //     exit(1);
    // }

    active_size -= ptr_size;

    base_free(ptr);
}


/// dmalloc_calloc(nmemb, sz, file, line)
///    Return a pointer to newly-allocated dynamic memory big enough to
///    hold an array of `nmemb` elements of `sz` bytes each. If `sz == 0`,
///    then must return a unique, newly-allocated pointer value. Returned
///    memory should be initialized to zero. The allocation request was at
///    location `file`:`line`.

void* dmalloc_calloc(size_t nmemb, size_t sz, const char* file, long line) {
    // Your code here (to fix test014).
    if (nmemb * sz > INT_MAX ||  sz > INT_MAX || nmemb > INT_MAX) {
        nfail += 1;
        return NULL;
    }
    void* ptr = dmalloc_malloc(nmemb * sz, file, line);
    if (ptr) {
        memset(ptr, 0, nmemb * sz);
    }
    return ptr;
}


/// dmalloc_get_statistics(stats)
///    Store the current memory statistics in `*stats`.

void dmalloc_get_statistics(dmalloc_statistics* stats) {
    // Stub: set all statistics to enormous numbers
    // memset(stats, 255, sizeof(dmalloc_statistics));
    // Your code here.
    stats->nactive = nactive;    
    stats->active_size = active_size;     // # bytes in active allocations
    stats->ntotal = ntotal;          // # total allocations
    stats->total_size = total_size;      // # bytes in total allocations
    stats->nfail = nfail;           // # failed allocation attempts
    stats->fail_size = fail_size;
    stats->heap_max = heap_max;                 // smallest allocated addr
    stats->heap_min = heap_min;
}


/// dmalloc_print_statistics()
///    Print the current memory statistics.

void dmalloc_print_statistics() {
    dmalloc_statistics stats;
    dmalloc_get_statistics(&stats);

    printf("alloc count: active %10llu   total %10llu   fail %10llu\n",
           stats.nactive, stats.ntotal, stats.nfail);
    printf("alloc size:  active %10llu   total %10llu   fail %10llu\n",
           stats.active_size, stats.total_size, stats.fail_size);
}


/// dmalloc_print_leak_report()
///    Print a report of all currently-active allocated blocks of dynamic
///    memory.

void dmalloc_print_leak_report() {
    for (auto i : lines) {
        size_t size = mallocsizes[i.first];        
        fprintf(stdout, "LEAK CHECK: %s:%ld: allocated object %p with size %ld\n", i.second.first, i.second.second, (void*) i.first, size);
    }
}


/// dmalloc_print_heavy_hitter_report()
///    Print a report of heavily-used allocation locations.

void dmalloc_print_heavy_hitter_report() {
    // Your heavy-hitters code here
}
