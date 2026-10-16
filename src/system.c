#include <stdio.h>
#include "system.h"

int
mf_get_uptime(double *seconds)
{
    FILE *fp;
    double value;

    if (seconds == NULL)
        return -1;

    fp = fopen("/proc/uptime", "r");
    if (fp == NULL)
        return -1;

    if (fscanf(fp, "%lf", &value) != 1) {
        fclose(fp);
        return -1;
    }

    fclose(fp);

    if (value < 0)
        return -1;

    *seconds = value;
    return 0;
}
