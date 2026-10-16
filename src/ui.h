#ifndef MINIFETCH_UI_H
#define MINIFETCH_UI_H

struct mf_ui {
    char display[64];
    char wm[64];
    char terminal[64];
    char shell[64];
};

int mf_get_ui(struct mf_ui *ui);

#endif
