#ifndef MINIFETCH_DISPLAY_H
#define MINIFETCH_DISPLAY_H

#include <sys/utsname.h>

#include "cpu.h"
#include "filesystem.h"
#include "load.h"
#include "memory.h"
#include "ui.h"

#define MF_FILESYSTEM_COUNT 3

struct mf_system_info {
    struct utsname uname_info;

    struct mf_ui ui;
    int has_ui;

    double uptime;
    int has_uptime;

    struct mf_memory memory;
    int has_memory;

    struct mf_cpu cpu;
    int has_cpu;

    struct mf_load load;
    int has_load;

    struct mf_filesystem filesystems[MF_FILESYSTEM_COUNT];
    int has_filesystem[MF_FILESYSTEM_COUNT];
};

enum mf_border_style {
    MF_BORDER_UNICODE,
    MF_BORDER_ASCII
};

void mf_display(const struct mf_system_info *info,
    enum mf_border_style style);

#endif
