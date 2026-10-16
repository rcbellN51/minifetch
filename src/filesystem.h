#ifndef MINIFETCH_FILESYSTEM_H
#define MINIFETCH_FILESYSTEM_H

#include <stdint.h>

struct mf_filesystem {
    uint64_t total_bytes;
    uint64_t used_bytes;
    uint64_t available_bytes;
};

int mf_get_filesystem(const char *path, struct mf_filesystem *fs);

#endif
