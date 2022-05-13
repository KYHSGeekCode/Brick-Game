#ifndef _MAIN_H
#define _MAIN_H
#define DEBUG

#include <windows.h>
#include <vector>
#include <math.h>
#include <stdio.h>

#include "resource.h" 

#ifdef DEBUG
#define LOG(format,data1) \
		MessageBox(0,format,itoa(data1,NULL,10),MB_OK);
#else 
#define LOG(str,data1)

#endif

//constants
#define SCRN_X 350
#define SCRN_Y 400
#define FPS 20
#define BTN_MARGIN 10
#define BTN_SIZE_X 100
#define BTN_SIZE_Y 60
//Start Button
#define BSL SCRN_X/2-BTN_SIZE_X/2
#define BSR SCRN_X/2+BTN_SIZE_X/2
#define BST SCRN_Y/2-BTN_SIZE_Y/2
#define BSB SCRN_X/2+BTN_SIZE_Y/2
//End Button
#define BEL BSL
#define BER BSR
#define BET BST+BTN_SIZE_Y/2+BTN_MARGIN
#define BEB BSB+BTN_SIZE_Y/2+BTN_MARGIN
//Resume Button
#define BRL BEL
#define BRR BER
#define BRT BET+BTN_SIZE_Y/2+BTN_MARGIN
#define BRB BEB+BTN_SIZE_Y/2+BTN_MARGIN
//Pause Button
#define BPL BRL
#define BPR BRR
#define BPT BRT+BTN_SIZE_Y/2+BTN_MARGIN
#define BPB BRB+BTN_SIZE_Y/2+BTN_MARGIN

#define BLOCK_SIZE_X 40
#define BLOCK_SIZE_Y 20
#define BLOCK_MARGIN_X 5
#define BLOCK_MARGIN_Y 5

#define BALL_RADIUS 8

enum status
{
	UNSTARTED,
	PLAYING,
	SIMULATING,
	PAUSED,
	GAMEOVER
};
extern status state;

#define BLOCK_COLUMNS 8
#define BLOCK_ROWS 15

#define BLOCK_ADDBALL -1

#define ISBLOCK(id) (id>0)
#define ISBALLITEM(id) (id==-1)


//types
class Ball
{
      public:
             int x, y, dx, dy;
             BOOL onGround;
             BOOL inGroup;
             Ball(int _x);
             Ball();
             void Move();
};

//prototypes
void InitGame();    //완전 처음 시작할 때
void GameOver();
void NewStage();		//새로운 스테이지 진입
void ShootBalls(double _dx,double _dy);	//wndproc에서 마우스 업 때 호출됨
//매 Tick마다 호출됨
void GameLoop();
//VOID CALLBACK Render(HWND hwnd,UINT uMsg, UINT iEvent, DWORD dwTime);
BOOL AskQuit();
void DrawBricks(HDC);
void DrawBalls(HDC);
void CleanUp();
void DrawBitmap(HDC hdc,int x,int y,HBITMAP hBit);

BOOL RectContains(RECT *rect,int x,int y);
//출처: wikipedia 
float Qsqrt( float number );

#endif
