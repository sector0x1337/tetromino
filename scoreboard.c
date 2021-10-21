#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tetromino.h"

#define SWP_ENT(__A, __B) do { \
    struct scoreBoardEnt _S; \
    memcpy(&_S, __A, sizeof(struct scoreBoardEnt)); \
    memcpy(__A, __B, sizeof(struct scoreBoardEnt)); \
    memcpy(__B, &_S, sizeof(struct scoreBoardEnt)); \
} while(0)


#define SORT_ENT do { \
    int __i, __j; \
    for (__i=0;__i<sbec;__i++) { \
        for (__j=__i;__j<sbec;__j++) { \
            if (top[__i].score<top[__j].score) { \
                SWP_ENT(&top[__i], &top[__j]); \
            } \
        } \
    } \
} while(0)


char *scorefile="tetromino.dat";
struct scoreBoardEnt top[10];
int sbec;

int getSbPos(int score) {
    int i;


    for (i=0;i<sbec;i++) {
        if (score>top[i].score) {
            return i;
        }
    }
    return (i==10?-1:i);
}


void setTopscore(struct scoreBoardEnt *sbe) {

    if (sbec<10) {
        memcpy(&top[sbec++], sbe, sizeof(struct scoreBoardEnt));
        SORT_ENT;
        return;
    }
    memcpy(&top[9], sbe, sizeof(struct scoreBoardEnt));
    SORT_ENT;
}

int writeTopscore() {
    FILE *f;

    if (sbec==0) {
        return 0;
    }

    if ((f=fopen(scorefile, "wb"))==NULL) {
        return __LINE__;
    }
    
    fwrite(top, sizeof(struct scoreBoardEnt), sbec, f);

    return 0;
}

int readTopscore() {
    FILE *f;
    struct scoreBoardEnt t;

    sbec=0;
    if ((f=fopen(scorefile, "rb"))==NULL) {
        return __LINE__;
    }

    while (fread(&t, sizeof(struct scoreBoardEnt), 1, f)==1) {
        memcpy(&top[sbec++], &t, sizeof(struct scoreBoardEnt));
    }
    fclose(f);

    if (sbec==0) {
        return 0;
    }
    SORT_ENT;

    return 0;
}
