#include "display.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#ifndef MF_DATADIR
#define MF_DATADIR "/usr/local/share/minifetch"
#endif

#define MF_ART_WIDTH       48
#define MF_GAP             1
#define MF_BOX_WIDTH       55
#define MF_COLOR_WIDTH     21
#define MF_MIN_COLUMNS     (MF_ART_WIDTH + MF_GAP + MF_BOX_WIDTH)
#define MF_LABEL_WIDTH     12

#define MF_RESET           "\033[0m"
#define MF_SYSTEM_COLOR    "\033[38;5;22m"
#define MF_HARDWARE_COLOR  "\033[38;5;24m"
#define MF_UI_COLOR        "\033[38;5;221m"

#define MF_MAX_ART_LINES   32
#define MF_MAX_LINE_LENGTH 8192
#define MF_INFO_LINES      32
#define MF_INFO_LENGTH     256

struct mf_border_chars {
    const char *top_left;
    const char *top_right;
    const char *bottom_left;
    const char *bottom_right;
    const char *horizontal;
    const char *vertical;
};

static const struct mf_border_chars mf_unicode_border = {
    "\342\225\255", "\342\225\256",
    "\342\225\260", "\342\225\257",
    "\342\224\200", "\342\224\202"
};

static const struct mf_border_chars mf_ascii_border = {
    "+", "+", "+", "+", "-", "|"
};

static const struct mf_border_chars *
border_chars(enum mf_border_style style)
{
    return style == MF_BORDER_ASCII ?
        &mf_ascii_border : &mf_unicode_border;
}

struct mf_block {
    const char *color;
    char lines[MF_INFO_LINES][MF_INFO_LENGTH];
    size_t count;
};

static const char *const mf_mounts[MF_FILESYSTEM_COUNT] = {
    "/",
    "/usr",
    "/home"
};

static void build_system_block(const struct mf_system_info *info,
    struct mf_block *block);
static void build_hardware_block(const struct mf_system_info *info,
    struct mf_block *block);
static void build_ui_block(const struct mf_system_info *info,
    struct mf_block *block);

static void
display_plain(const struct mf_system_info *info)
{
    struct mf_block system;
    struct mf_block hardware;
    struct mf_block ui;
    size_t i;

    build_system_block(info, &system);
    build_hardware_block(info, &hardware);
    build_ui_block(info, &ui);

    printf(" minifetch - MINIX system information\n\n");

    for (i = 0; i < system.count; i++)
        printf("%s\n", system.lines[i]);

    for (i = 0; i < hardware.count; i++)
        printf("%s\n", hardware.lines[i]);

    for (i = 0; i < ui.count; i++)
        printf("%s\n", ui.lines[i]);
}

static int
terminal_columns(void)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0)
        return 0;

    return (int)ws.ws_col;
}

static void
block_add(struct mf_block *block, const char *text)
{
    if (block->count >= MF_INFO_LINES)
        return;

    snprintf(block->lines[block->count], MF_INFO_LENGTH,
        "%s", text);

    block->count++;
}

static void
block_add_field(struct mf_block *block,
    const char *label, const char *value)
{
    char buffer[MF_INFO_LENGTH];

    snprintf(buffer, sizeof(buffer), "%*s %s",
        MF_LABEL_WIDTH, label, value);

    block_add(block, buffer);
}

static void
build_system_block(const struct mf_system_info *info,
    struct mf_block *block)
{
    char buffer[MF_INFO_LENGTH];

    block->color = MF_SYSTEM_COLOR;
    block->count = 0;

    // block_add(block, "SYSTEM");
    // block_add(block, "");

    block_add_field(block, "OS:", info->uname_info.sysname);
    block_add_field(block, "Release:", info->uname_info.release);
    block_add_field(block, "Version:", info->uname_info.version);
    block_add_field(block, "Machine:", info->uname_info.machine);
    block_add_field(block, "Hostname:", info->uname_info.nodename);

    if (info->has_uptime) {
        unsigned long total_minutes;
        unsigned long days;
        unsigned long hours;
        unsigned long minutes;

        total_minutes = (unsigned long)(info->uptime / 60.0);
        days = total_minutes / 1440;
        hours = (total_minutes % 1440) / 60;
        minutes = total_minutes % 60;

        if (days > 0) {
            snprintf(buffer, sizeof(buffer),
                "%lu day%s, %lu hour%s, %lu minute%s",
                days, days == 1 ? "" : "s",
                hours, hours == 1 ? "" : "s",
                minutes, minutes == 1 ? "" : "s");
        } else if (hours > 0) {
            snprintf(buffer, sizeof(buffer),
                "%lu hour%s, %lu minute%s",
                hours, hours == 1 ? "" : "s",
                minutes, minutes == 1 ? "" : "s");
        } else {
            snprintf(buffer, sizeof(buffer),
                "%lu minute%s",
                minutes, minutes == 1 ? "" : "s");
        }

        block_add_field(block, "Uptime:", buffer);
    } else {
        block_add_field(block, "Uptime:", "unavailable");
    }
}

static void
build_hardware_block(const struct mf_system_info *info,
    struct mf_block *block)
{
    char buffer[MF_INFO_LENGTH];
    char label[32];
    size_t i;

    block->color = MF_HARDWARE_COLOR;
    block->count = 0;

    // block_add(block, "HARDWARE");
    // block_add(block, "");

    if (info->has_cpu) {
        if (info->cpu.mhz > 0) {
            snprintf(buffer, sizeof(buffer),
                "%s (%.2f GHz)",
                info->cpu.model,
                (double)info->cpu.mhz / 1000.0);
        } else {
            snprintf(buffer, sizeof(buffer),
                "%s", info->cpu.model);
        }

        block_add_field(block, "CPU:", buffer);
    } else {
        block_add_field(block, "CPU:", "unavailable");
    }

    if (info->has_memory && info->memory.total_bytes != 0) {
        double used;
        double total;
        double percent;

        used = (double)(info->memory.total_bytes -
            info->memory.free_bytes) / 1048576.0;
        total = (double)info->memory.total_bytes / 1048576.0;
        percent = used * 100.0 / total;

        snprintf(buffer, sizeof(buffer),
            "%.0f / %.0f MiB (%.0f%% used)",
            used, total, percent);

        block_add_field(block, "Memory:", buffer);
    } else {
        block_add_field(block, "Memory:", "unavailable");
    }

    if (info->has_load) {
        snprintf(buffer, sizeof(buffer),
            "%.2f, %.2f, %.2f (1, 5, 15 min)",
            info->load.one, info->load.five, info->load.fifteen);

        block_add_field(block, "Load:", buffer);
    } else {
        block_add_field(block, "Load:", "unavailable");
    }

    for (i = 0; i < MF_FILESYSTEM_COUNT; i++) {
        snprintf(label, sizeof(label),
            "Disk %s:", mf_mounts[i]);

        if (info->has_filesystem[i] &&
            info->filesystems[i].total_bytes != 0) {
            const struct mf_filesystem *fs;
            double used;
            double total;
            double percent;

            fs = &info->filesystems[i];
            used = (double)fs->used_bytes / 1048576.0;
            total = (double)fs->total_bytes / 1048576.0;
            percent = used * 100.0 / total;

            snprintf(buffer, sizeof(buffer),
                "%.0f / %.0f MiB (%.0f%% used)",
                used, total, percent);

            block_add_field(block, label, buffer);
        } else {
            block_add_field(block, label, "unavailable");
        }
    }
}

static void
build_ui_block(const struct mf_system_info *info,
    struct mf_block *block)
{
    block->color = MF_UI_COLOR;
    block->count = 0;

    // block_add(block, "UI");
    // block_add(block, "");

    if (info->has_ui) {
        block_add_field(block, "Display:", info->ui.display);
        block_add_field(block, "WM:", info->ui.wm);
        block_add_field(block, "Terminal:", info->ui.terminal);
        block_add_field(block, "Shell:", info->ui.shell);
    } else {
        block_add_field(block, "Display:", "unavailable");
        block_add_field(block, "WM:", "unavailable");
        block_add_field(block, "Terminal:", "unavailable");
        block_add_field(block, "Shell:", "unavailable");
    }
}

static void
print_border(const char *color,
    const struct mf_border_chars *chars, int bottom)
{
    const char *left;
    const char *right;
    size_t i;

    left = bottom ? chars->bottom_left : chars->top_left;
    right = bottom ? chars->bottom_right : chars->top_right;

    printf("%s%s", color, left);

    for (i = 0; i < MF_BOX_WIDTH - 2; i++)
        fputs(chars->horizontal, stdout);

    printf("%s%s", right, MF_RESET);
}

static void
print_box_line(const char *text, const char *color,
    const struct mf_border_chars *chars)
{
    size_t length;
    size_t i;

    length = strlen(text);

    if (length > MF_BOX_WIDTH - 4)
        length = MF_BOX_WIDTH - 4;

    printf("%s%s%s ", color, chars->vertical, MF_RESET);
    fwrite(text, 1, length, stdout);

    for (i = length; i < MF_BOX_WIDTH - 4; i++)
        putchar(' ');

    printf(" %s%s%s", color, chars->vertical, MF_RESET);
}

static size_t
block_height(const struct mf_block *block)
{
    return block->count + 2;
}

static void
print_block_row(const struct mf_block *block, size_t row,
    const struct mf_border_chars *chars)
{
    if (row == 0 || row == block->count + 1) {
        print_border(block->color, chars,
            row == block->count + 1);
    } else {
        print_box_line(block->lines[row - 1],
            block->color, chars);
    }
}

static void
print_information_row(const struct mf_block *system,
    const struct mf_block *hardware, const struct mf_block *ui,
    size_t row, const struct mf_border_chars *chars)
{
    size_t system_height;
    size_t hardware_height;
    size_t hardware_row;
    size_t ui_row;

    system_height = block_height(system);
    hardware_height = block_height(hardware);

    if (row < system_height) {
        print_block_row(system, row, chars);
        return;
    }

    hardware_row = row - system_height;

    if (hardware_row < hardware_height) {
        print_block_row(hardware, hardware_row, chars);
        return;
    }

    ui_row = hardware_row - hardware_height;

    if (ui_row < block_height(ui))
        print_block_row(ui, ui_row, chars);
}

static void
print_color_block_row(size_t row,
    const struct mf_border_chars *chars,
    enum mf_border_style style)
{
    const char *swatch;
    const char *left;
    const char *right;
    size_t i;
    int color;

    swatch = style == MF_BORDER_ASCII ?
        "o" : "\342\227\217";

    for (i = 0; i < MF_BOX_WIDTH - MF_COLOR_WIDTH; i++)
        putchar(' ');

    if (row == 0 || row == 3) {
        left = row == 0 ?
            chars->top_left : chars->bottom_left;
        right = row == 0 ?
            chars->top_right : chars->bottom_right;

        printf("%s%s", MF_SYSTEM_COLOR, left);

        for (i = 0; i < MF_COLOR_WIDTH - 2; i++)
            fputs(chars->horizontal, stdout);

        printf("%s%s", right, MF_RESET);
        return;
    }

    printf("%s%s%s  ",
        MF_SYSTEM_COLOR, chars->vertical, MF_RESET);

    for (i = 0; i < 8; i++) {
        color = (int)((row - 1) * 8 + i);

        printf("\033[38;5;%dm%s%s",
            color, swatch, MF_RESET);

        if (i < 7)
            putchar(' ');
    }

    printf("  %s%s%s",
        MF_SYSTEM_COLOR, chars->vertical, MF_RESET);
}

static int
display_boxed(const struct mf_system_info *info,
    int show_artwork, enum mf_border_style style)
{
    char path[1024];
    char artwork[MF_MAX_ART_LINES][MF_MAX_LINE_LENGTH];
    struct mf_block system;
    struct mf_block hardware;
    struct mf_block ui;
    const char *home;
    const struct mf_border_chars *chars;
    FILE *fp;
    size_t art_count;
    size_t info_height;
    size_t total_rows;
    size_t row;
    size_t length;

    art_count = 0;
    chars = border_chars(style);

    if (show_artwork) {
        home = getenv("HOME");

        if (home == NULL)
            return -1;

        snprintf(path, sizeof(path),
            "%s/.config/minifetch/minix.ascii", home);

        fp = fopen(path, "r");

        if (fp == NULL)
            fp = fopen(MF_DATADIR "/minix.ascii", "r");

        if (fp == NULL)
            return -1;

        while (art_count < MF_MAX_ART_LINES &&
            fgets(artwork[art_count], MF_MAX_LINE_LENGTH, fp) != NULL) {
            length = strlen(artwork[art_count]);

            if (length > 0 &&
                artwork[art_count][length - 1] == '\n')
                artwork[art_count][length - 1] = '\0';

            art_count++;
        }

        fclose(fp);
    }

    build_system_block(info, &system);
    build_hardware_block(info, &hardware);
    build_ui_block(info, &ui);

    info_height = block_height(&system) +
        block_height(&hardware) +
        block_height(&ui);

    total_rows = info_height + 4;

    if (show_artwork && art_count > total_rows)
        total_rows = art_count;

    for (row = 0; row < total_rows; row++) {
        if (show_artwork) {
            size_t i;

            if (row < art_count) {
                fputs(artwork[row], stdout);
            } else {
                for (i = 0; i < MF_ART_WIDTH; i++)
                    putchar(' ');
            }

            printf("%s", MF_RESET);

            for (i = 0; i < MF_GAP; i++)
                putchar(' ');
        }

        if (row < info_height) {
            print_information_row(&system, &hardware, &ui,
                row, chars);
        } else if (row < info_height + 4) {
            print_color_block_row(row - info_height,
                chars, style);
        }

        putchar('\n');
    }

    printf("%s", MF_RESET);

    return 0;
}

void
mf_display(const struct mf_system_info *info,
    enum mf_border_style style)
{
    int columns;

    if (info == NULL)
        return;

    columns = terminal_columns();

    if (!isatty(STDOUT_FILENO) || columns < MF_BOX_WIDTH) {
        display_plain(info);
        return;
    }

    if (columns >= MF_MIN_COLUMNS) {
        if (display_boxed(info, 1, style) == 0)
            return;
    }

    if (display_boxed(info, 0, style) == 0)
        return;

    display_plain(info);
}
