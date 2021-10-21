#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <windows.h>

#include "tetromino.h"
#define PREV_BLOCK_KL 10

uint8_t bmp[BMPSIZE];
uint32_t prev_bmp[(4*PREV_BLOCK_KL)*(2*PREV_BLOCK_KL)];
char hs_name[16];
int hsn_off;
int sc=0;
int rows[20];
int rowclearstep;

extern int curX, curY;
extern int feld[MAX_X*MAX_Y];
extern int curBlock[10];
extern enum game_mod game;
extern int lines;
extern int score;
extern int level;
extern int nextBlock[10];
extern struct scoreBoardEnt top[10];
extern int sbec;
extern char *scorefile;

int updateWnd(PAINTSTRUCT *ps) {
    int c, i, x, y, bx, by;
    HDC hdcmem;
    HBITMAP hbmp;
    RECT rct;
    HBRUSH hbr;
    HPEN hp;
    char str[200];

    SetTextColor(ps->hdc, RGB(100,100,255));
    SetBkColor(ps->hdc, RGB(20,20,20));


    if (game==gm_pause) {
        rct.top=100;
        rct.left=100;
        rct.bottom=200;
        rct.right=400;
        hbr=CreateSolidBrush(RGB(20,20,20));
        hp=CreatePen(PS_SOLID, 1, RGB(20,20,20));
        SelectObject(ps->hdc, hbr);
        SelectObject(ps->hdc, hp);
        Rectangle(ps->hdc, 0, 0, 400, 500);
        
        DrawText(ps->hdc, "Pause....", -1, &rct, DT_LEFT|DT_TOP);
        DeleteObject(hbr);
        DeleteObject(hp);
        return 1;
    }

    for (i=0;i<(WIDTH*HEIGHT);i++) {
        x=(i%200)/20;
        y=i/4000;

        c=feld[COORD(x,y)];
        if (game == gm_run && x>=curX && x<(curX+curBlock[0]) && y>=curY && y<(curY+curBlock[1]) && curBlock[2+x-curX+(y-curY)*curBlock[0]]) {
            c=curBlock[2+x-curX+(y-curY)*curBlock[0]];
        }

        if (game==gm_clearline && rows[y]) {
            bmp[4*i]=0xff;
            bmp[4*i+1]=0xff;
            bmp[4*i+2]=0xff;
        } else {
            by=i/WIDTH;
            bx=i%WIDTH;
            if ((bx%20)==0 || (by%20)==0 || (bx%20)==1 || (by%20)==1) {
                bmp[4*i]=((c&1)?255:0);
                bmp[4*i+1]=((c&2)?255:0);
                bmp[4*i+2]=((c&4)?255:0);
            } else {
                bmp[4*i]=((c&1)?155:0);
                bmp[4*i+1]=((c&2)?175:0);
                bmp[4*i+2]=((c&4)?125:0);
            }
            
        }
    }

    for (i=0;i<(10*4*10*2);i++) {
        x=(i%40)/10;
        y=i/400;

        if (x<nextBlock[0] && y<nextBlock[1]) {
            c=nextBlock[2+x+y*nextBlock[0]];
            prev_bmp[i]=(c?0x6464ff:0x141414);
        } else {
            prev_bmp[i]=RGB(20,20,20);
        }


    }
    hp=CreatePen(PS_SOLID, 1, RGB(20, 20, 20));
    hbr=CreateSolidBrush(RGB(20, 20, 20));
    SelectObject(ps->hdc, hp);
    SelectObject(ps->hdc, hbr);

    hdcmem=CreateCompatibleDC(ps->hdc);
    hbmp=CreateBitmap(200, 400, 1, 32, bmp);
    SelectObject(hdcmem, hbmp);
    BitBlt(ps->hdc, 10, 10, 210, 410, hdcmem, 0, 0, SRCCOPY);
    DeleteObject(hbmp);

    if (game==gm_highscore_w) {
        sprintf(str, "Bitte Name eingeben\n>> %s_\n", hs_name);
    } else if (game==gm_run || game==gm_clearline) {
        int csb=getSbPos(score);

        sprintf(str, "Score: %i\nLevel: %i\n", score, level);
        if (csb!=-1) {
            sprintf(str+strlen(str), "Aktueller Platz: %i\n", csb+1);
        }
    }

    rct.top=10;
    rct.left=220;
    rct.right=400;
    rct.bottom=400;
    Rectangle(ps->hdc, 210, 0, 450, 570);
    DrawText(ps->hdc, str, -1, &rct, DT_LEFT|DT_TOP);

    rct.top=rct.top+50;
    if (game==gm_highscore_r) {
        for (i=0;i<sbec;i++) {
            char scrstr[100];
            sprintf(scrstr, "%i.: %i: %s", i+1, top[i].score, top[i].name);
            rct.top=rct.top+20;
            DrawText(ps->hdc, scrstr, -1, &rct, DT_LEFT|DT_TOP);
        }
    }


    if (game==gm_run) {
        hbmp=CreateBitmap(40, 20, 1, 32, prev_bmp);
        SelectObject(hdcmem, hbmp);
        BitBlt(ps->hdc, 220, 100, 260, 160, hdcmem, 0, 0, SRCCOPY);
        DeleteObject(hbmp);
        DeleteDC(hdcmem);
    }

    DeleteObject(hp);
    DeleteObject(hbr);

    return 1;
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
        case WM_CREATE: {
            int i;
            UINT_PTR p=1;

            readTopscore();

            srand(time(NULL));
            for (i=0;i<(MAX_X*MAX_Y);i++) {
                feld[i]=0;
            }
            SetTimer(hwnd, p, 50, NULL);
            initNextTet();
            newTet(0);
            break;
        }

        case WM_TIMER: {
            if (game==gm_clearline) {
                if (rowclearstep) {
                    rowclearstep--;
                } else {
                    cleanRows();
                    game=gm_run;
                    if (!newTet(1)) {
                        game=gm_highscore_w;
                        hsn_off=0;
                    }
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                
                return 0;

            }
            if (game!=gm_run) {
                return 0;
            }

            if (sc) {
                sc--;
                break;
            }
            sc=20-level;
            step();
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            
            BeginPaint(hwnd, &ps);
            updateWnd(&ps);
            EndPaint(hwnd, &ps);
            break;
        }

        case WM_KEYDOWN: {
            int i, j;

            if (game==gm_highscore_r) {
                game=gm_run;
                for (i=0;i<(MAX_X*MAX_Y);i++) {
                    feld[i]=0;
                }
                newTet(0);
                break;
            }
            if (game==gm_pause) {
                game=gm_run;
                InvalidateRect(hwnd, NULL, FALSE);
                break;
            }

            if (game==gm_highscore_w) {

                if (wParam==0x08) {
                    if (hsn_off==0) {
                        break;
                    }
                    hsn_off--;
                    hs_name[hsn_off]='\0';
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                }

                if (hsn_off>=15 && wParam!=0x0d) {
                    break;
                }

                if (wParam==0x0d) { // Enter
                    struct scoreBoardEnt s;

                    memset(&s, 0, sizeof(struct scoreBoardEnt));
                    strncpy(s.name, hs_name, 16);
                    s.score=score;
                    if (hsn_off) {
                        setTopscore(&s);
                        if (writeTopscore()) {
                            MessageBox(0, "Konnte Scoreboard nicht speichern.", scorefile, MB_OK|MB_ICONERROR);
                        }
                    }

                    game=gm_highscore_r;
                    memset(feld, 0, sizeof(feld));
                    memset(hs_name, 0, 16);
                    hsn_off=0;
                    newTet(0);
                    lines=0;
                    score=0;
                    level=1;
                } else {

                    if (!(wParam>=0x30 && wParam<=0x39) && !(wParam>=0x41 && wParam<=0x5a)) {
                        break;
                    }
                    hs_name[hsn_off++]=wParam;
                    hs_name[hsn_off]='\0';
                }
                InvalidateRect(hwnd, NULL, FALSE);
                break;
            }

            switch ((lParam>>15)&0xff) {
                case 0xa0:
                    step();
                    break;
                case 0x96:
                    if (curX<=0) {
                        return 0;
                    }
                    for (i=0;i<curBlock[0];i++) {
                        for (j=0;j<curBlock[1];j++) {
                            if (feld[COORD(curX-1+i, curY+j)] && curBlock[2+i+j*curBlock[0]]) {
                                return 0;
                            }
                        }
                    }
                    curX--;
                    break;

                case 0x9a:
                    if ((curX+curBlock[0])>=10) {
                        return 0;
                    }
                    for (i=0;i<curBlock[0];i++) {
                        for (j=0;j<curBlock[1];j++) {
                            if (feld[COORD(curX+1+i, curY+j)] && curBlock[2+i+j*curBlock[0]]) {
                                return 0;
                            }
                        }
                    }
                    curX++;
                    break;

                case 0x90:
                    flip();
                    break;

                case 0x72: // [SPACE]
                    while (step()) {
                        continue;
                    }
                    break;

                case 0x32: // Pause
                    game=gm_pause;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
            }

            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}