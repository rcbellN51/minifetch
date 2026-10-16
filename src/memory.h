#ifndef MINIFETCH_MEMORY_H
#define MINIFETCH_MEMORY_H

#include <stdint.h>

struct mf_memory {
    uint64_t total_bytes;
    uint64_t free_bytes;
    uint64_t contiguous_bytes;
    uint64_t cached_bytes;
};

int mf_get_memory(struct mf_memory *memory);

#endif
