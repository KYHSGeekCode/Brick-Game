#include "main.h"
using namespace std;
HINSTANCE module;
HWND g_hwnd;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[ ] = "BrickBreaker";

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    MSG msg;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_HREDRAW | CS_VREDRAW;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           TEXT("스와이프 벽돌깨기 pc버전"),       /* Title Text */
           WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, /*  window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           SCRN_X,                 /* The programs width */
           SCRN_Y,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);
    
    /* Run the message loop. It will run until GetMessage() returns 0 *///PeakMessage 요망 
    #if 0
	//#ifdef NOREST
	while( WM_QUIT != msg.message ) 
	{ 
      // 메세지큐에 메세지가 있는지 검사한다. 
      // 메세지가 있건,없건 그냥 리턴한다. 
      if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)) 
      { 
            //메세지 큐에서 메세지를 가져온다. 
            GetMessage(&msg,NULL,0,0); 
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
      } 
      else 
      { 
            //메세지가 없다면 게임루프를 돌린다.
			//if(bflying==TRUE) 
            //	GameLoop(); 
      } 
	}
	#endif
	//#else
	while (GetMessage(&msg,NULL,0,0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//#endif
    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return msg.wParam;
}

HDC hBufferDC;
HBITMAP g_buffer;
status state;
RECT crt;
RECT StartBtnRect={BSL,BST,BSR,BSB};
RECT EndBtnRect={BEL,BET,BER,BEB};
RECT ResumeBtnRect={BRL,BRT,BRR,BRB};
RECT PauseBtnRect={BPL,BPT,BPR,BPB};

HBRUSH RedBrush;
HBRUSH BlueBrush;
HBRUSH GreenBrush;
HBRUSH CarrotBrush;
HGDIOBJ Old;
BOOL bReadyShoot;
int readyShootdX,readyShootdY;
int mouseX,mouseY;
int downMouseX,downMouseY;

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
        	{
        		DeleteObject(RedBrush);
        		DeleteObject(BlueBrush);
        		DeleteObject(GreenBrush);
        		DeleteObject(CarrotBrush);
        		DeleteObject(g_buffer);
        		CleanUp();
        		KillTimer(hwnd,0); 
            	PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
			}
        	break;
        case WM_CREATE:
        	{
        		g_hwnd=hwnd;
        		SetRect(&crt,0,0,SCRN_X,SCRN_Y);		//크기 변경 금지를 위해 
				AdjustWindowRect(&crt,WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,FALSE);
				SetWindowPos(hwnd,NULL,0,0,crt.right-crt.left,crt.bottom-crt.top,SWP_NOMOVE | SWP_NOZORDER);
				//리소스를 얻어온다.
				//필요 없다. MCISendString할 것이기 때문에!!
				//브러시 미리 생성
				RedBrush=CreateSolidBrush(RGB(0xFF,32,0));
				BlueBrush=CreateSolidBrush(RGB(0,128,255));
				GreenBrush=CreateSolidBrush(RGB(0,255,32));
				CarrotBrush=CreateSolidBrush(RGB(255,128,0));
		 		SetTimer(hwnd,0,1000/FPS,NULL);
				state=UNSTARTED;
		 		InvalidateRect(hwnd,NULL,TRUE);
			}
        	break;
		case WM_LBUTTONDOWN:
			{
				mouseX=LOWORD(lParam);
				mouseY=HIWORD(lParam);
				switch(state)
				{
					case UNSTARTED:
						//게임시작 버튼은 UP처리기로 이동 
						if(RectContains(&EndBtnRect,mouseX,mouseY))
						{
							//MessageBox(hwnd,TEXT("Exit?"),TEXT("NOTIFY"),MB_OK);
							if(AskQuit())
							{
								DestroyWindow(hwnd);
							}
							break;
						}
						break;
					case SIMULATING:
					case PLAYING:
						if(RectContains(&PauseBtnRect,mouseX,mouseY))
						{
							state=PAUSED;
							return 0;
						}
						if(state==SIMULATING)return 0;
						//발사 준비
						bReadyShoot=TRUE;
						downMouseX=mouseX;
						downMouseY=mouseY;
						break;
					case PAUSED:
						//재개, 재시작, 종료 버튼 처리
						if(RectContains(&ResumeBtnRect,mouseX,mouseY))
						{
							state=PLAYING;
							break;
						} 
						if(RectContains(&EndBtnRect,mouseX,mouseY))
						{
							if(AskQuit())
							{
								DestroyWindow(hwnd);
							}
							break;
						}
						if(RectContains(&StartBtnRect,mouseX,mouseY))
						{
							state=PLAYING;
							break;
						}
						break;
					case GAMEOVER:
						//재시작, 종료 버튼 처리
						if(RectContains(&StartBtnRect,mouseX,mouseY))
						{
							state=PLAYING;
							break;
						}
						if(RectContains(&EndBtnRect,mouseX,mouseY))
						{
							if(AskQuit())
							{
								DestroyWindow(hwnd);
							}
							break;
						}
						break;
					default:
						break;
				}
			}	
			break;
		case WM_LBUTTONUP:
			{
				switch(state)
				{
					case PLAYING:
						//발사
						bReadyShoot=FALSE;
						ShootBalls(readyShootdX,readyShootdY);
						break;
					case UNSTARTED: 
						if(RectContains(&StartBtnRect,mouseX,mouseY))
						{
							//게임시작 
							state=PLAYING; 
							InitGame();
						}
						break;
				}
			}
			break;
		case WM_RBUTTONDOWN:
			{
				//발사 준비 취소
				bReadyShoot=FALSE;
			}
			break;
		case WM_MOUSEMOVE:
			{
				switch(state)
				{
					case PLAYING:
						if(bReadyShoot==TRUE)
						{
							//발사 각도 재계산
							mouseX=LOWORD(lParam);
							mouseY=HIWORD(lParam);
							readyShootdX=mouseX-downMouseX;
							readyShootdY=mouseY-downMouseY;
							InvalidateRect(hwnd,NULL,TRUE);
						}
					default:
						// 버튼 있는 경우에는 버튼 하이라이트 처리 가능. 
						break; 
				}
			}
			break;
		case WM_KEYDOWN:
			{
				switch(wParam)
				{
					case VK_ESCAPE:
						if(AskQuit())
						{
							DestroyWindow(hwnd);
						}
				}
			}
			break; 
		case WM_PAINT:
			{
				HDC hdc;
				hdc=GetDC(hwnd);
				 switch(state)
				 {
				 	case UNSTARTED:
				 		FillRect(hdc,&StartBtnRect,CarrotBrush);
				 		FillRect(hdc,&EndBtnRect,CarrotBrush);
				 		SetBkMode(hdc,TRANSPARENT);
				 		SetTextColor(hdc,RGB(0xFF,0xFF,0xFF));
				 		DrawText(hdc,TEXT("Start"),strlen("Start"),&StartBtnRect,DT_CENTER|DT_VCENTER);
				 		DrawText(hdc,TEXT("End"),strlen("End"),&EndBtnRect,DT_CENTER|DT_VCENTER);
				 		break;
				 	case SIMULATING:
				 		DrawBitmap(hdc,0,0,g_buffer);
				 		FillRect(hdc,&PauseBtnRect,CarrotBrush);
				 		break;
				 	case PLAYING:
				 		//공 조준선 표시
						MoveToEx(hdc,downMouseX,downMouseY,NULL);
						LineTo(hdc,mouseX,mouseY); 
				 		FillRect(hdc,&PauseBtnRect,CarrotBrush);
				 		break;
				 	case PAUSED:
				 		FillRect(hdc,&StartBtnRect,CarrotBrush);
				 		FillRect(hdc,&EndBtnRect,CarrotBrush);
				 		FillRect(hdc,&ResumeBtnRect,CarrotBrush);
				 		SetBkMode(hdc,TRANSPARENT);
				 		SetTextColor(hdc,RGB(0xFF,0xFF,0xFF));
				 		DrawText(hdc,TEXT("ReStart"),strlen("ReStart"),&StartBtnRect,DT_CENTER|DT_VCENTER);
				 		DrawText(hdc,TEXT("End"),strlen("End"),&EndBtnRect,DT_CENTER|DT_VCENTER);
				 		DrawText(hdc,TEXT("Resume"),strlen("Resume"),&ResumeBtnRect,DT_CENTER|DT_VCENTER);
				 		break;
				 	case GAMEOVER:
				 		FillRect(hdc,&StartBtnRect,CarrotBrush);
				 		FillRect(hdc,&EndBtnRect,CarrotBrush);
				 		SetBkMode(hdc,TRANSPARENT);
				 		SetTextColor(hdc,RGB(0xFF,0xFF,0xFF));
				 		DrawText(hdc,TEXT("ReStart"),strlen("ReStart"),&StartBtnRect,DT_CENTER|DT_VCENTER);
				 		DrawText(hdc,TEXT("End"),strlen("End"),&EndBtnRect,DT_CENTER|DT_VCENTER);
				 		break;
			     }  
			}
			break;
		case WM_TIMER:
			{
				switch(state)
				{
					case SIMULATING:
						//상태 업데이트
						//더블 버퍼에 그림. (WM_PAINT에서 해도 됨)
						GameLoop();
						break;
					default:
						break;
				}
				 
			}
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

#include <vector>

int numofballs;
int leftballs;
int newballs;
int level;
int blocks[BLOCK_COLUMNS][BLOCK_ROWS];
BOOLEAN bFlying;
Ball leader;	//가장 먼저 떨어지는 공
int group_x; 
std::vector<Ball> balls;

void InitGame()    //완전 처음 시작할 때 (Start Button 클릭 시) 
{
     Ball ballfirst=Ball(rand()%SCRN_X);
     LOG("made ballfirst",0)
     balls.push_back(ballfirst);
     LOG("push backed",0)
     memset(blocks,0,sizeof(blocks));
     newballs=0;
     numofballs=1;
     leftballs=1;
     level=0;
     leader=Ball();
     LOG("Made New leader",0)
     LOG("ballfirst 대입",0)
     leader=ballfirst;
     NewStage();
     LOG("End of InitGame",0)
}

#include <math.h>
void NewStage()		//새로운 스테이지 진입 
{
	LOG("NEwStage",0)
	bFlying=FALSE;	//시뮬레이션 중인가? 
	level++;			//level=score 이다. 
	for(int i=BLOCK_ROWS-2; i<=0;i--)
	{
		for(int j=0;j<BLOCK_COLUMNS;j++)
		{
			//아래로 블록을 한 칸씩 내린다.
			blocks[j][i+1]=blocks[j][i];
			if(i+1==BLOCK_ROWS&&blocks[j][i+1]>0)
			{
				//게임 오버
				GameOver(); 
			}
			if(i+1==BLOCK_ROWS&&blocks[j][i+1]==BLOCK_ADDBALL)	//공 추가 Item
			{
				newballs++;
			}
		}
	}
	LOG("end of NEw stage Bricks SetUp",0)
	LOG("New Balls 만큼 추가",0)
	
	//반복문을 이용하여 공을 newballs개수 만큼 추가
	for(int k=0;k<newballs;k++)
	{
		Ball newball=Ball(group_x);
		balls.push_back(newball); 
	}
	LOG("공 추가 끝",0)
	
	numofballs+=newballs;
	leftballs+=newballs;
	//add new lines
	LOG("새 줄 추가",0)
	//문제 발생!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
	int seed=(int)log10(level);
	LOG("log10 호출끝",0) 
	for(int i=0;i<BLOCK_COLUMNS;i++)
	{
		if(rand()%seed)
		{
			blocks[i][0]=level;
		}
		else if(!rand()%BLOCK_COLUMNS)
		{
			blocks[i][0]=BLOCK_ADDBALL;	//공 추가 Item 
		}
		LOG("새 줄 추가중",blocks[i][0])
	}
	LOG("새 줄 추가 끝",0) 
	bReadyShoot=FALSE;
}

void ShootBalls(double _dx,double _dy)	//wndproc에서 마우스 업 때 호출됨 
{
	if(_dy<=0)return; 
	//leader=NULL;
	state=SIMULATING;
	bFlying=TRUE;					//시뮬레이트 중 
	leftballs=numofballs;
	//dx,dy를 Normalize()
	//픽셀 단위
	double size=sqrt(_dx*_dx+_dy*_dy);
	if(size<=0)
	{
		//error();
		//발사하지 않음 
		return;
	}
	_dx/=size;
	_dy/=size;
	_dx*=10;//_dx*=Qsqrt(level);
	_dy*=10;//_dy*=Qsqrt(level);
	//타이머 주기를 바꿀까? 
	//모든 공의 dx,dy 세트함.
	//vector<Ball>::iterator first,last,i;
	//first=balls.begin();
	//last=balls.end();
	for(int i=0;i<balls.size();i++)
	{
		balls[i].dx=_dx;
		balls[i].dy=_dy; 
	}
}

//매 Tick마다 호출됨
void GameLoop()
{
	RECT ballnumrect;
	HDC hdc=GetDC(g_hwnd);
    if (g_buffer==NULL) 
	{
        g_buffer=CreateCompatibleBitmap(hdc,SCRN_X,SCRN_Y);
    }
    hBufferDC=CreateCompatibleDC(hdc);
    HBITMAP OldBit=(HBITMAP)SelectObject(hBufferDC,g_buffer);
    //배경색 
    FillRect(hBufferDC,&crt,(HBRUSH)GetStockObject(WHITE_BRUSH));
    //이제 그린다. 
    //벽돌을 그린다.
	DrawBricks(hBufferDC);
	SetRect(&ballnumrect,leader.x,leader.y,leader.x+20,leader.y+10);
	//공 그리기를 준비하기 위해 여러 가지 일들을 한다. 
	if(bFlying&&state==SIMULATING)	//공이 이동중이라면
	{
		//모두 땅에 떨어졌는지 판단하기 위해 일단은 해 둔다.
		bFlying=FALSE;
		vector<Ball>::iterator first,last,i;
		first=balls.begin();
		last=balls.end();
		for(i=first;i!=last;i++)	//모든 공을 순회하면서 
		{
			//공의 상태 업데이트
			Ball now=*i;
			//우선 공이 가만히 있으면 발사시켜준다.(처음에만)
			//launchBall:
			if(leftballs>0)
			{
				if((now.onGround)&&(now.inGroup)) //발사 전이라면 
				{
					if(first==i)	//첫 번째 공이라면 발사 
					{
						now.onGround=FALSE;
						now.inGroup=FALSE;
					}
					else
					{
						Ball before=*(i-1);	//이전 공
						if(before.inGroup ==FALSE && before.onGround ==FALSE)	//이전 공이 발사되었으면 나도 발사 
						{
				 			now.inGroup =FALSE;
				 			now.onGround =FALSE;
						}
					}
					leftballs--;			//남은 공 감소 
			 	} 
			}
			//모든 공이 발사 상태가 업데이트 되었다면 
			//공을 움직인다.
			//충돌 검사와 착륙 검사도 한다. 
			now.Move();
			//땅에 떨어진 것 검사
			
			//각 공을 그린다.(더블 버퍼에)
			//공을 그린다. 
			//DrawBalls(hBufferDC); 
	
		}
		//그룹에 남아 있는 공을 그린다.
		if(leftballs>0)
		{
			char _tmp[32];
			sprintf(_tmp,"X%d",leftballs);
			SetBkMode(hdc,TRANSPARENT);
			SetTextColor(hdc,RGB(0x0,0x0,0xFF));
			DrawText(hdc,_tmp,strlen(_tmp),&ballnumrect,DT_CENTER|DT_VCENTER);
			Old=SelectObject(hdc,BlueBrush);
			Ellipse(hdc,leader.x,leader.y,leader.x+2*BALL_RADIUS,leader.y+BALL_RADIUS);
			SelectObject(hdc,Old);
		}		 
	 } 
	//마무리
	SelectObject(hBufferDC,OldBit);
    DeleteDC(hBufferDC);
    ReleaseDC(g_hwnd,hdc); 
    InvalidateRect(g_hwnd,NULL,FALSE);
	//게임이 너무 빨리 돌아가는 것 방지..?
	//WaitForSingleObject(handle,Milisec); 
 } 
 
void Ball::Move()
{
	if((!inGroup)&&state==SIMULATING)	//발사된 공 이라면 
	{
		int nx=x+dx;
		int ny=y+dy;
				 
	}
}

Ball::Ball(int _x)
{
	if(x>SCRN_X)_x=SCRN_X;
	if(x<0)_x=0;
 } 
 
Ball::Ball()
{
	
}


void DrawBricks(HDC hdc)
{
	RECT blockRect;
	HBRUSH br;
	for(int i=0;i<BLOCK_ROWS;i++)
	{
		for(int j=0;j<BLOCK_COLUMNS;j++)
		{
			if(ISBLOCK(blocks[j][i]))		//BLOCK이면 
			{
				int sx=j*BLOCK_SIZE_X+(j+1)*BLOCK_MARGIN_X;
				int sy=i*BLOCK_SIZE_Y+(i+1)*BLOCK_MARGIN_Y;
				int ex=sx+BLOCK_SIZE_X;
				int ey=sy+BLOCK_SIZE_Y;
				SetRect(&blockRect,sx,sy,ex,ey);
				if(blocks[j][i]>level/2)
					br=RedBrush;
				else
					br=CarrotBrush;
				FillRect(hdc,&blockRect,br);
				//글씨 출력 
				SetBkMode(hdc,TRANSPARENT);
				SetTextColor(hdc,RGB(0xFF,0xFF,0xFF));
				char _tmp[16];
				ltoa(blocks[j][i],_tmp,10);
				DrawText(hdc,_tmp,strlen(_tmp),&blockRect,DT_CENTER|DT_VCENTER);
			}
			else if(ISBALLITEM(blocks[j][i]))	//공 추가 Item이면 
			{
				int sx=j*BLOCK_SIZE_X+(j+1)*BLOCK_MARGIN_X+BLOCK_SIZE_X/2-BALL_RADIUS;
				int sy=i*BLOCK_SIZE_Y+(i+1)*BLOCK_MARGIN_Y+BLOCK_SIZE_Y/2-BALL_RADIUS;
				int ex=sx+BALL_RADIUS;
				int ey=sy+BALL_RADIUS;
				//공을 그림.
				Old=SelectObject(hdc,GreenBrush); 
				Ellipse(hdc,sx,sy,ex,ey);
				SelectObject(hdc,Old);
			}
		}
	}
}

void DrawBalls(HDC hdc)
{
	
}


void GameOver()
{
	state=GAMEOVER;
}

//출처: wikipedia 
float Qsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return 1/y;
}

BOOL RectContains(RECT *rect,int x,int y)
{
	return (rect->left<x&&rect->right>x&&rect->top<y&&rect->bottom>y);
}
//
//VOID CALLBACK Render(HWND hwnd,UINT uMsg, UINT iEvent, DWORD dwTime)
//{
//	//더블 버퍼 스와핑
//	
//}

BOOL AskQuit()
{
	return MessageBox(g_hwnd,TEXT("종료하시겠습니까?"),TEXT("종료"),MB_YESNO|MB_ICONASTERISK)==MB_OK;
}

void DrawBitmap(HDC hdc,int x,int y,HBITMAP hBit)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx,by;
	BITMAP bit;

	MemDC=CreateCompatibleDC(hdc);
	OldBitmap=(HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit,sizeof(BITMAP),&bit);
	bx=bit.bmWidth;
	by=bit.bmHeight;

	BitBlt(hdc,x,y,bx,by,MemDC,0,0,SRCCOPY);

	SelectObject(MemDC,OldBitmap);
	DeleteDC(MemDC);
}

void CleanUp()
{
	leader;
	vector<Ball>::iterator first,last,i;
	first=balls.begin();
	last=balls.end();
	//for(i=first;i!=last;i++)	//모든 공을 순회하면서 
	//{
	//	delete *i;				//공 삭제
	//	*i=NULL; 
	//}
}
