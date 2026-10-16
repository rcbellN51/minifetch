#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

#include "display.h"
#include "system.h"

int
main(int argc, char *argv[])
{
    struct mf_system_info info;
    enum mf_border_style style;
    int i;

    style = MF_BORDER_UNICODE;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--ascii") == 0) {
            style = MF_BORDER_ASCII;
        } else if (strcmp(argv[i], "--unicode") == 0) {
            style = MF_BORDER_UNICODE;
        } else {
            fprintf(stderr, "Usage: %s [--ascii | --unicode]\n",
                argv[0]);
            return EXIT_FAILURE;
        }
    }

    memset(&info, 0, sizeof(info));

    if (uname(&info.uname_info) != 0) {
        perror("uname");
        return 1;
    }

    info.has_uptime =
        mf_get_uptime(&info.uptime) == 0;

    info.has_memory =
        mf_get_memory(&info.memory) == 0;

    info.has_cpu =
        mf_get_cpu(&info.cpu) == 0;

    info.has_load =
        mf_get_load(&info.load) == 0;

    info.has_ui =
        mf_get_ui(&info.ui) == 0;

    info.has_filesystem[0] =
        mf_get_filesystem("/", &info.filesystems[0]) == 0;

    info.has_filesystem[1] =
        mf_get_filesystem("/usr", &info.filesystems[1]) == 0;

    info.has_filesystem[2] =
        mf_get_filesystem("/home", &info.filesystems[2]) == 0;

    mf_display(&info, style);

    return 0;
}
