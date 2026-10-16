#ifndef MINIFETCH_CPU_H
#define MINIFETCH_CPU_H

struct mf_cpu {
    char vendor[64];
    char model[128];
    unsigned int mhz;
};

int mf_get_cpu(struct mf_cpu *cpu);

#endif
