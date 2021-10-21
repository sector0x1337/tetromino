#ifndef __TETROMINO_H__
#define __TETROMINO_H__

#include <stdint.h>
#include <windows.h>

#define MAX_X 10
#define MAX_Y 20
#define WIDTH (MAX_X*20)
#define HEIGHT (MAX_Y*20)
#define BMPSIZE 4*WIDTH*HEIGHT
#define COORD(__X, __Y) ((__X)+((__Y)*MAX_X))

struct scoreBoardEnt {
    uint32_t score;
    char name[16];
};

enum game_mod { gm_run, gm_pause, gm_clearline, gm_highscore_r, gm_highscore_w };

int flip();
int newTet(int addscore);
void blkcopy(int *target, int *b);
void initNextTet();
int updateWnd(PAINTSTRUCT *ps);
LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int step();
void cleanRows();

void setTopscore(struct scoreBoardEnt *sbe);
int writeTopscore();
int readTopscore();
int getSbPos(int score);

#endif
