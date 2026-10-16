#include "load.h"

#include <stdio.h>

int
mf_get_load(struct mf_load *load)
{
    FILE *fp;
    double one;
    double five;
    double fifteen;

    if (load == NULL)
        return -1;

    fp = fopen("/proc/loadavg", "r");
    if (fp == NULL)
        return -1;

    if (fscanf(fp, "%lf %lf %lf",
        &one, &five, &fifteen) != 3) {
        fclose(fp);
        return -1;
    }

    if (fclose(fp) != 0)
        return -1;

    if (!(one >= 0.0 && five >= 0.0 && fifteen >= 0.0))
        return -1;

    load->one = one;
    load->five = five;
    load->fifteen = fifteen;

    return 0;
}
