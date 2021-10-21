#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdint.h>

#include "tetromino.h"


int feld[MAX_X*MAX_Y];
int lines=0;
int score=0;
int level=1;
enum game_mod game=gm_run;
extern int curX;
extern int curY;
extern int curBlock[10];
extern int hsn_off;
extern int rows[20];
extern int rowclearstep;

void cleanRows() {
    int i, j, c, line, u;
    do {
        line=0;
        for (j=19;j>=0;j--) {
            c=0;
            for (i=0;i<10;i++) {
                if (feld[COORD(i,j)]) {
                    c++;
                }
            }
            if (c==10) {
                line=1;
                for (u=j;u>0;u--) {
                    for (i=0;i<10;i++) {
                        feld[COORD(i,u)]=feld[COORD(i,u-1)];
                    }
                }
                for (i=0;i<10;i++) {
                    feld[COORD(i,0)]=0;
                }
                lines++;
                level=level+(lines/10);
                lines=lines%10;
                score=score+100*level;
                break;
            }
        }
    } while (line);
}

int step() {
    int i, j, u;
    int mov=1;

    if (game==gm_run) {
        memset(rows, 0, sizeof(int)*20);
    }
    
    if ((curY+curBlock[1])>=20) {
        mov=0;
    } else {
        for (i=0;i<curBlock[0];i++) {
            for (j=0;j<curBlock[1];j++) {
                if (curBlock[2+i+j*curBlock[0]] && feld[COORD(i+curX, j+curY+1)]) {
                    mov=0;
                    break;
                }
            }
            if (!mov) {
                break;
            }
        }
    }

    if (mov) {
        curY++;
        return 1;
    } else {
        for (i=0;i<curBlock[0];i++) {
            for (j=0;j<curBlock[1];j++) {
                if (curBlock[2+i+curBlock[0]*j]) {
                    feld[COORD(curX+i, curY+j)]=curBlock[2+i+curBlock[0]*j];
                }
            }
        }
        do {
            int bc;

            u=0;

            for (j=19;j>=0;j--) {
                bc=0;
                for (i=0;i<10;i++) {
                    if (feld[COORD(i,j)]) {
                        bc++;
                    }
                }
                if (bc==10) {
                    rows[j]=1;
                    rowclearstep=3;
                    game=gm_clearline;
                } else {
                    rows[j]=0;
                }
            }
        } while (u);
        if (game==gm_run) {
            if (!newTet(1)) {
                game=gm_highscore_w;
                hsn_off=0;
            }
        }
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgs, int show) {
    WNDCLASSEX wcx;
    HWND wnd;
    MSG msg;
    char *klasse="tetromino";

    wcx.cbClsExtra=0;
    wcx.cbSize=sizeof(WNDCLASSEX);
    wcx.cbWndExtra=0;
    wcx.hbrBackground=(HBRUSH)CreateSolidBrush(RGB(20, 20, 20));
    wcx.hCursor=LoadCursor(NULL, IDC_ARROW);
    wcx.hIcon=LoadIcon(hThisInstance, MAKEINTRESOURCE(100));
    wcx.hIconSm=wcx.hIcon;
    wcx.hInstance=hThisInstance;
    wcx.lpfnWndProc=wndProc;
    wcx.lpszClassName=klasse;
    wcx.lpszMenuName=NULL;
    wcx.style=CS_DBLCLKS;

    RegisterClassEx(&wcx);
    wnd=CreateWindowEx(0, klasse, "Tetromino-Puzzle", WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 450, 570, HWND_DESKTOP, NULL, hThisInstance, 0);

    ShowWindow(wnd, show);
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}