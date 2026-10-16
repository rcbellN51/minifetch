#ifndef MINIFETCH_LOAD_H
#define MINIFETCH_LOAD_H

struct mf_load {
    double one;
    double five;
    double fifteen;
};

int mf_get_load(struct mf_load *load);

#endif
