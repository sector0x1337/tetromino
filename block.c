#include <stdlib.h>
#include <string.h>

#include "tetromino.h"


int block_i[]={
    4,1,
    1,1,1,1
};

int block_j[]={
    3,2,
    2,0,0,
    2,2,2
};

int block_l[]={
    3,2,
    0,0,3,
    3,3,3
};

int block_s[]={
    3,2,
    0,4,4,
    4,4,0
};

int block_z[]={
    3,2,
    5,5,0,
    0,5,5
};

int block_o[]={
    2,2,
    6,6,
    6,6
};

int block_t[]={
    3,2,
    0,7,0,
    7,7,7
};

int *rtet[]= {
    block_i,
    block_j,
    block_l,
    block_o,
    block_s,
    block_t,
    block_z
};

int curBlock[10];
int nextBlock[10];
int curX=0;
int curY=0;
extern enum game_mod game;
extern int feld[MAX_X*MAX_Y];
extern int score;
extern int level;

int flip() {
    int tx, i, j, nb[10];

    nb[0]=curBlock[1];
    nb[1]=curBlock[0];
    for (i=0;i<nb[0];i++) {
        for (j=0;j<nb[1];j++) {
            nb[2+i+j*nb[0]]=curBlock[2+j+(nb[0]-i-1)*nb[1]];
        }
    }

    tx=-1;
    if (nb[0]==4) {
        tx=curX-2;
    }
    if (nb[0]==1) {
        tx=curX+2;
    }
    if (nb[0]==3) {
        tx=curX-1;
    }
    if (nb[0]==2&&nb[1]==3) {
        tx=curX+1;
    }
    if (nb[0]==2&&nb[1]==2) {
        tx=curX;
    }
    if (tx<0 || tx+nb[0]>10) {
        return 0;
    }

    if (tx==-1) { // passiert das??
        return 0;
    }

    for (i=0;i<nb[0];i++) {
        for (j=0;j<nb[1];j++) {
            if (nb[2+i+j*nb[0]] && feld[COORD(tx+i, curY+j)]) {
                return 0;
            }
        }
    }
    
    curX=tx;
    memcpy(curBlock, nb, sizeof(int)*10);
    return 1;
}

void initNextTet() {
    blkcopy(nextBlock, rtet[rand()%7]);
}

int newTet(int addscore) {
    int i, j, nb[10];

    blkcopy(nb, nextBlock);
    curX=5-(nb[0]>>1)-1;
    curY=0;

    for (i=0;i<nb[0];i++) {
        for (j=0;j<nb[1];j++) {
            if (nb[i+j*nb[0]] && feld[COORD(curX+i, curY+j)]) {
                return 0;
            }
        }
    }
    if (addscore) {
        score=score+level;
    }

    blkcopy(curBlock, nb);
    initNextTet();

    return 1;
}

void blkcopy(int *target, int *b) {
    int i;

    for (i=0;i<(2+b[0]*b[1]);i++) {
        target[i]=b[i];
    }
}