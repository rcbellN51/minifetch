#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
copy_string(char *destination, size_t size, const char *source)
{
    if (source == NULL || *source == '\0')
        source = "unavailable";

    snprintf(destination, size, "%s", source);
}

static void
detect_display(struct mf_ui *ui)
{
    const char *display;

    display = getenv("DISPLAY");

    if (display == NULL || *display == '\0') {
        copy_string(ui->display, sizeof(ui->display), "none");
        return;
    }

    snprintf(ui->display, sizeof(ui->display),
        "X11 (%s)", display);
}

static void
detect_wm(struct mf_ui *ui)
{
    FILE *fp;
    char line[256];
    char command[128];
    char window_id[32];
    char *start;
    char *end;

    copy_string(ui->wm, sizeof(ui->wm), "unavailable");

    if (getenv("DISPLAY") == NULL)
        return;

    fp = popen("xprop -root _NET_SUPPORTING_WM_CHECK 2>/dev/null", "r");

    if (fp == NULL)
        return;

    if (fgets(line, sizeof(line), fp) == NULL) {
        pclose(fp);
        return;
    }

    pclose(fp);

    start = strstr(line, "0x");

    if (start == NULL)
        return;

    end = start;

    while ((*end >= '0' && *end <= '9') ||
        (*end >= 'a' && *end <= 'f') ||
        (*end >= 'A' && *end <= 'F') ||
        *end == 'x') {
        end++;
    }

    if ((size_t)(end - start) >= sizeof(window_id))
        return;

    memcpy(window_id, start, (size_t)(end - start));
    window_id[end - start] = '\0';

    snprintf(command, sizeof(command),
        "xprop -id %s _NET_WM_NAME 2>/dev/null", window_id);

    fp = popen(command, "r");

    if (fp == NULL)
        return;

    if (fgets(line, sizeof(line), fp) == NULL) {
        pclose(fp);
        return;
    }

    pclose(fp);

    start = strchr(line, '"');

    if (start == NULL)
        return;

    start++;
    end = strchr(start, '"');

    if (end == NULL)
        return;

    *end = '\0';

    copy_string(ui->wm, sizeof(ui->wm), start);
}

static void
detect_terminal(struct mf_ui *ui)
{
    const char *value;

    value = getenv("TERM_PROGRAM");

    if (value != NULL && *value != '\0') {
        copy_string(ui->terminal, sizeof(ui->terminal), value);
        return;
    }

    value = getenv("SSH_CONNECTION");

    if (value != NULL && *value != '\0') {
        value = getenv("TERM");

        if (value != NULL && *value != '\0') {
            snprintf(ui->terminal, sizeof(ui->terminal),
                "%s (SSH)", value);
        } else {
            copy_string(ui->terminal, sizeof(ui->terminal), "SSH");
        }

        return;
    }

    value = getenv("TERM");
    copy_string(ui->terminal, sizeof(ui->terminal), value);
}

static void
detect_shell(struct mf_ui *ui)
{
    const char *shell;
    const char *name;

    shell = getenv("SHELL");

    if (shell == NULL || *shell == '\0') {
        copy_string(ui->shell, sizeof(ui->shell), "unavailable");
        return;
    }

    name = strrchr(shell, '/');

    if (name != NULL)
        name++;
    else
        name = shell;

    copy_string(ui->shell, sizeof(ui->shell), name);
}

int
mf_get_ui(struct mf_ui *ui)
{
    if (ui == NULL)
        return -1;

    memset(ui, 0, sizeof(*ui));

    detect_display(ui);
    detect_wm(ui);
    detect_terminal(ui);
    detect_shell(ui);

    return 0;
}
