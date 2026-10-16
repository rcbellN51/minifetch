#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"

static void
trim_newline(char *s)
{
    size_t len;

    len = strlen(s);

    while (len > 0 &&
        (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

int
mf_get_cpu(struct mf_cpu *cpu)
{
    FILE *fp;
    char line[256];
    char *key;
    char *value;
    char *colon;

    if (cpu == NULL)
        return -1;

    memset(cpu, 0, sizeof(*cpu));

    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL)
        return -1;

    while (fgets(line, sizeof(line), fp) != NULL) {
        colon = strchr(line, ':');
        if (colon == NULL)
            continue;

        *colon = '\0';
        key = line;
        value = colon + 1;

        while (*value == ' ' || *value == '\t')
            value++;

        trim_newline(value);

        if (strncmp(key, "vendor_id", 9) == 0) {
            snprintf(cpu->vendor, sizeof(cpu->vendor),
                "%s", value);
        } else if (strncmp(key, "model name", 10) == 0) {
            snprintf(cpu->model, sizeof(cpu->model),
                "%s", value);
        } else if (strncmp(key, "cpu MHz", 7) == 0) {
            cpu->mhz = (unsigned int)strtoul(value, NULL, 10);
        }
    }

    fclose(fp);

    if (cpu->model[0] == '\0')
        return -1;

    return 0;
}
