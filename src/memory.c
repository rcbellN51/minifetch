#include <stdio.h>
#include <stdint.h>
#include "memory.h"

int
mf_get_memory(struct mf_memory *memory)
{
    FILE *fp;
    unsigned long page_size;
    unsigned long total;
    unsigned long free_pages;
    unsigned long contiguous;
    unsigned long cached;

    if (memory == NULL)
        return -1;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
        return -1;

    if (fscanf(fp, "%lu %lu %lu %lu %lu",
        &page_size,
        &total,
        &free_pages,
        &contiguous,
        &cached) != 5) {
        fclose(fp);
        return -1;
    }

    fclose(fp);

    if (page_size == 0 || total == 0 ||
        free_pages > total || contiguous > total ||
        cached > total)
        return -1;

    memory->total_bytes =
        (uint64_t)total * page_size;

    memory->free_bytes =
        (uint64_t)free_pages * page_size;

    memory->contiguous_bytes =
        (uint64_t)contiguous * page_size;

    memory->cached_bytes =
        (uint64_t)cached * page_size;

    return 0;
}
