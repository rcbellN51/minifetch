#include "filesystem.h"

#include <stddef.h>
#include <sys/statvfs.h>

int
mf_get_filesystem(const char *path, struct mf_filesystem *fs)
{
    struct statvfs st;
    uint64_t block_size;
    uint64_t total;
    uint64_t free_blocks;
    uint64_t available;

    if (path == NULL || fs == NULL)
        return -1;

    if (statvfs(path, &st) != 0)
        return -1;

    block_size = st.f_frsize != 0 ? st.f_frsize : st.f_bsize;

    if (block_size == 0 || st.f_blocks == 0)
        return -1;

    total = (uint64_t)st.f_blocks * block_size;
    free_blocks = (uint64_t)st.f_bfree * block_size;
    available = (uint64_t)st.f_bavail * block_size;

    if (free_blocks > total || available > total)
        return -1;

    fs->total_bytes = total;
    fs->used_bytes = total - free_blocks;
    fs->available_bytes = available;

    return 0;
}
