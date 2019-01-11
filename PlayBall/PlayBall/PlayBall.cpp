#include <windows.h>
#include "resource.h"
#include "Handleball.h"


#define COL_BLACK (RGB(0,0,0))


#define MAX_LOADSTRING 100
#define MIN_WIDTH	550
#define MIN_HEIGHT  400

#define TIMER_ID  100			// ��ʱ��ID��
#define BALL_R	  10			// ��İ뾶
#define MOVE_LEN  5				// ���ƶ�����

#define TIMER_INTERVAL	20		// ��ʱ��ʱ����
#define TRANS_TINTERVAL 10		// ����Ч��������ʱ����

// ��timerID
#define ID_BALL_MOVE	100		// ���˶�������ID
#define ID_TRANS_UP		101		// ����
#define ID_TRANS_DOWN	102		// ����
#define ID_AUTORUN		103		// �Զ�����

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
TCHAR szEvent[MAX_LOADSTRING];					// �����¼��������ƣ����ڼ��������е�����ʵ��
TCHAR szActiveExist[MAX_LOADSTRING];			// �Զ�����Ϣ����
TCHAR szBgMusic[MAX_LOADSTRING];				// ���������ļ���

UINT UM_ACTIVEEXIST=NULL;						// �Զ�����Ϣ������⵽�д˳��������ʵ��������ʱ�����䴰�ڷ��ʹ���Ϣ

HBITMAP g_hbmpBg=NULL;							// ����λͼ���
HBITMAP g_hbmpDream=NULL;							// 
BITMAP  g_BgbmpInfo;								// ����λͼ��Ϣ
BITMAP	g_DreamBmpInfo;		
HMENU g_hMenu=NULL;								// �˵����


bool g_isInit=true;							    // �Ƿ������ڳ�ʼ���У�Init������������Ϊtrue���������ĺ�����������Ϊfalse��OnPaint���ݴ˻�ͼ
bool g_isStart=false;								// Ϊtrue����ִ����start
bool g_isStay=false;								// Ϊtrue����ִ����stay
bool g_isFullScreen=false;						// �Ƿ�ȫ��

bool g_needContinue=false;						// ����С������ʱ���ã��ָ�ʱ���ݴ���������
bool g_canShowBall=false;							// �Ƿ��������Ļ��ʾС��	
bool g_isTimerExist=false;						// TIMER�������
HandleBall g_HandleBallInst;						// ����ʵ��

int g_TransUp=0;								// ����͸����
int g_TransDown=255;							

HDC g_hCompatibleDC = 0;						// ����DC
HDC g_hdcBk = 0;
HDC g_hDc;

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void SetMenuItem(HWND hWnd);
void Init(HWND hWnd,HandleBall *pHandleBall);			// ��ʼ������
void StartOrStop(HWND hWnd,HandleBall *pHandleBall);	// ��ʼ��ֹͣ
void StayOrMove(HWND hWnd,HandleBall *pHandleBall);		// ��ͣ�����		
BOOL OnSizing(HWND hWnd ,WPARAM wParam ,LPARAM lParam,LONG MinHeight,LONG MinWidth);
bool FullScreen(HWND hWnd ,RECT *pRect,LONG *pStyle,bool isEnterFull);	
BOOL OnSize(HWND hWnd,WPARAM wParam,LPARAM lParam);

//	��Ӧtimer�������������timer�˺���һһ����
void OnTimer(HWND hWnd,WPARAM wParam ,LPARAM lParam);

// ��֤��������ʵ��Ψһ�Եĺ�������
DWORD WINAPI WaitForExit(LPVOID lpParameter);
bool isExist();

// ���ر������ֲ�д���ļ�
bool LoadAndWriteMIDFile(HMODULE hModule,LPWSTR lpFileName);

//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOVEBALL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= CreateSolidBrush(COL_BLACK);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	hWnd = CreateWindowEx(WS_EX_LAYERED,szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}
	// 
	SetLayeredWindowAttributes(hWnd,RGB(255,0,0),0,LWA_COLORKEY | LWA_ALPHA);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	RECT WndRect,ClientRect;
	HMENU hSubMenu;

	static LONG s_WndStyle;		// ���洰����ʽ
	static RECT s_WndRt;			// ���洰���������

	if(message==UM_ACTIVEEXIST)
	{
		SetLayeredWindowAttributes(hWnd,RGB(255,0,0),0,LWA_COLORKEY | LWA_ALPHA);
		SetTimer(hWnd,ID_TRANS_UP,TRANS_TINTERVAL,NULL);
		ShowWindow(hWnd,SW_SHOWNORMAL);
		return 1;
	}
	switch (message)
	{
	case WM_CREATE:
	{
					  g_hDc = GetDC(hWnd);
					  g_hCompatibleDC = CreateCompatibleDC(g_hDc);
					  RECT rt = { 0 };
					  GetClientRect(GetDesktopWindow(), &rt);
					  HBITMAP hBitmap = CreateCompatibleBitmap(g_hDc, rt.right, rt.bottom);
					  HGDIOBJ oldObj = SelectObject(g_hCompatibleDC, hBitmap);
					  g_hdcBk = CreateCompatibleDC(g_hDc);
					  SelectObject(g_hdcBk, g_hbmpBg);
	}
		SetTimer(hWnd,ID_TRANS_UP,TRANS_TINTERVAL,NULL);
		// �����˵�
		g_hMenu=GetMenu(hWnd);
		hSubMenu=GetSubMenu(g_hMenu,0);
		for(int i=1;i<9;i++)
			EnableMenuItem(hSubMenu,i,MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		SetMenuDefaultItem(hSubMenu,0,TRUE);

		// ��������λ��
		GetWindowRect(hWnd,&WndRect);
		SetWindowPos(hWnd,0,GetSystemMetrics(SM_CXSCREEN)/2-(WndRect.right-WndRect.left)/2,GetSystemMetrics(SM_CYSCREEN)/2-(WndRect.bottom-WndRect.top)/2,0,0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

		// ��ʼ������
		Init(hWnd,&g_HandleBallInst);
		g_HandleBallInst.SetMemDc(g_hCompatibleDC);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_EXIT:
			SetTimer(hWnd,ID_TRANS_DOWN,TRANS_TINTERVAL,NULL);
			break;
		case ID_INIT:	// ��ʼ��
			Init(hWnd,&g_HandleBallInst);
			break;
		case ID_START:	// ����
			StartOrStop(hWnd,&g_HandleBallInst);
			break;
		case ID_STOP:	// ֹͣ
			StartOrStop(hWnd,&g_HandleBallInst);
			break;
		case ID_PAUSE:	// ��ͣ
			StayOrMove(hWnd,&g_HandleBallInst);	
			break;
		case ID_CONTINUE: // ����
			StayOrMove(hWnd,&g_HandleBallInst);	
			break;
		case ID_FULLSCREEN:			// ȫ����ʾ
			ModifyMenu(GetSubMenu(g_hMenu,0),ID_FULLSCREEN,MF_BYCOMMAND | MF_STRING ,ID_NORMALSCREEN,L"�˳�ȫ��(&X)\tF5");
			// ���ز˵�
			SetMenu(hWnd,NULL);
			// ���ú�������ȫ��
			FullScreen(hWnd,&s_WndRt,&s_WndStyle,true);
			g_isFullScreen=true;
			break;
		case ID_NORMALSCREEN:		// �˳�ȫ��			
			ModifyMenu(GetSubMenu(g_hMenu,0),ID_NORMALSCREEN,MF_BYCOMMAND | MF_STRING ,ID_FULLSCREEN,L"ȫ����ʾ(&F)\tF5");
			// ���ú����˳�ȫ��
			FullScreen(hWnd,&s_WndRt,&s_WndStyle,false);
			// ��ʾ�˵�
			SetMenu(hWnd,g_hMenu);

			g_isFullScreen=false;
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ���������ͼ����...
		if(g_isInit)
		{
			// ���ڳ�ʼ��֮������ʾ����ͼƬ
			GetClientRect(hWnd,&ClientRect);
			SelectObject(g_hdcBk, g_hbmpBg);
			BitBlt(hdc,(ClientRect.right+ClientRect.left)/2-g_BgbmpInfo.bmWidth/2,(ClientRect.bottom+ClientRect.top)/2-g_BgbmpInfo.bmHeight/2,g_BgbmpInfo.bmWidth,g_BgbmpInfo.bmHeight,g_hdcBk,0,0,SRCCOPY);
		}
		if(g_canShowBall)
			g_HandleBallInst.Show(0,false);
		if(!g_isInit && !g_isStart )
		{
			// �ǳ�ʼ��֮����ִ����ֹͣ����
			GetClientRect(hWnd,&ClientRect);
			SelectObject(g_hdcBk, g_hbmpDream);
			BitBlt(hdc,(ClientRect.right+ClientRect.left)/2-g_DreamBmpInfo.bmWidth/2,(ClientRect.bottom+ClientRect.top)/2-g_DreamBmpInfo.bmHeight/2,g_DreamBmpInfo.bmWidth,g_DreamBmpInfo.bmHeight,g_hdcBk,0,0,SRCCOPY);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_RBUTTONDOWN:
		// ������ݲ˵�
		POINT pt;
		pt.x=MAKEPOINTS(lParam).x;
		pt.y=MAKEPOINTS(lParam).y;
		ClientToScreen(hWnd,&pt);
		TrackPopupMenu(GetSubMenu(g_hMenu,0),
			TPM_TOPALIGN | TPM_LEFTALIGN,									// ���λ
			pt.x,pt.y,0,hWnd,0);
		break;
	case WM_TIMER:
		// ����timer����ͳһ����
		OnTimer(hWnd,wParam,lParam);
		break;
	case WM_CHAR:
		switch(wParam)
		{
		case 0x20:	// �ո�
			StayOrMove(hWnd,&g_HandleBallInst);	
			break;
		case 0x0D:  // �س�
			StartOrStop(hWnd,&g_HandleBallInst);
			break;
		case 0x1B:
			Init(hWnd,&g_HandleBallInst);
			break;
		}
		break;
	case WM_KEYDOWN:	
		// ��ӦF5���ܼ�
		if(wParam==VK_F5)
		{
			if(g_isFullScreen)
			{
				// ��ǰ����ȫ����Ӧ�˳�ȫ��
				SendMessage(hWnd,WM_COMMAND,(WPARAM)ID_NORMALSCREEN,(LPARAM)0);
				return 0;
			}
			else 
			{
				// ��ǰ����ȫ����Ӧ����ȫ��
				SendMessage(hWnd,WM_COMMAND,(WPARAM)ID_FULLSCREEN,(LPARAM)0);
				return 0;
			}
		}
		break;
	case WM_SIZE:
		OnSize(hWnd,wParam,lParam);
		break;
	case WM_SIZING:
		return OnSizing(hWnd,wParam,lParam,MIN_HEIGHT ,MIN_WIDTH);
		break;
	case WM_CLOSE:
		SetTimer(hWnd,ID_TRANS_DOWN,TRANS_TINTERVAL,NULL);
		DeleteObject(SelectObject(g_hCompatibleDC, NULL));
		ReleaseDC(hWnd,g_hdcBk);
		ReleaseDC(hWnd,g_hCompatibleDC);
		ReleaseDC(hWnd,g_hDc);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Init(HWND hWnd,HandleBall *pHandleBall)
{
	if(g_isTimerExist)
		KillTimer(hWnd,ID_BALL_MOVE);

	pHandleBall->init(hWnd,BALL_R,MOVE_LEN);
	InvalidateRect(hWnd,NULL,TRUE);
	g_isInit=true;
	g_canShowBall=false;
	g_isTimerExist=false;
	SetMenuItem(hWnd);

	SetTimer(hWnd,ID_AUTORUN,3000,NULL);
}

void StartOrStop(HWND hWnd,HandleBall *pHandleBall)
{
	if(g_isInit || (!g_isInit && !g_isStart))		// ִ���˳�ʼ�������߳�ʼ����������û��ִ��Start
	{	
		// ��ʼ
		// ����timer����canShowBall����Ϊtrue
		//		InvalidateRect(hWnd,NULL,TRUE);
		pHandleBall->init(hWnd,BALL_R,MOVE_LEN);

		SetTimer(hWnd,ID_BALL_MOVE,TIMER_INTERVAL,NULL);
		g_canShowBall=false;
		g_isStart=true;
		g_isTimerExist=true;
		g_isInit=false;
		g_isStay=false;
	}
	else if(!g_isInit && g_isStart)			// �ǳ�ʼ����ִ����Start
	{
		// ֹͣ
		KillTimer(hWnd,ID_BALL_MOVE);
		InvalidateRect(hWnd,NULL,TRUE);

		g_canShowBall=false;
		g_isStart=false;
		g_isTimerExist=false;
		g_isInit=false;
		g_isStay=false;
	}
	SetMenuItem(hWnd);
}

void StayOrMove(HWND hWnd,HandleBall *pHandleBall)
{
	if(!g_isInit && g_isStart && !g_isStay)		// �ǳ�ʼ����ִ����Start��û��ִ��Stay
	{
		KillTimer(hWnd,ID_BALL_MOVE);
		g_canShowBall=true;
		g_isStay=true;
		g_isTimerExist=false;
		g_isInit=false;
	}
	else if(!g_isInit && g_isStart && g_isStay)
	{
		SetTimer(hWnd,ID_BALL_MOVE,TIMER_INTERVAL,NULL);
		g_canShowBall=false;
		g_isStay=false;
		g_isTimerExist=true;
		g_isInit=false;
	}
	SetMenuItem(hWnd);
}

BOOL OnSizing(HWND hWnd ,WPARAM wParam ,LPARAM lParam,LONG MinHeight,LONG MinWidth)
{
	LONG NewWidth,NewHeight;
	RECT* pRect;
	// lParam is a Pointer point a RECT structure;
	// So we can get the Width and Height as follow
	if(wParam!=WMSZ_BOTTOM 
		&& wParam!=WMSZ_BOTTOMLEFT
		&& wParam!=WMSZ_BOTTOMRIGHT
		&& wParam!=WMSZ_LEFT
		&& wParam!=WMSZ_RIGHT
		&& wParam!=WMSZ_TOP
		&& wParam!=WMSZ_TOPLEFT
		&& wParam!=WMSZ_TOPRIGHT)
		return FALSE;

	NewWidth=((RECT*)lParam)->right-((RECT*)lParam)->left;
	NewHeight=((RECT*)lParam)->bottom-((RECT*)lParam)->top;

	pRect=(RECT*)lParam;

	switch(wParam)
	{
	case WMSZ_BOTTOM:
		if(NewHeight<MinHeight)
		{
			pRect->bottom=pRect->top+MinHeight;
			return TRUE;
		}
		break;
	case WMSZ_BOTTOMLEFT:
		if(NewWidth<MinWidth || NewHeight<MinHeight)
		{
			if(NewWidth<MinWidth)
				pRect->left=pRect->right-MinWidth;
			if(NewHeight<MinHeight)
				pRect->bottom=pRect->top+MinHeight;
			return TRUE;
		}
		break;
	case WMSZ_BOTTOMRIGHT:
		if(NewWidth<MinWidth || NewHeight < MinHeight)
		{
			if(NewWidth<MinWidth)
				pRect->right=pRect->left+MinWidth;
			if(NewHeight < MinHeight)
				pRect->bottom=pRect->top+MinHeight;
			return TRUE;
		}
		break;
	case WMSZ_LEFT:
		if(NewWidth < MinWidth)
		{
			pRect->left=pRect->right-MinWidth;
			return TRUE;
		}
		break;
	case WMSZ_RIGHT:
		if(NewWidth < MinWidth)
		{
			pRect->right=pRect->left+MinWidth;
			return TRUE;
		}
		break;
	case WMSZ_TOP:
		if(NewHeight < MinHeight)
		{
			pRect->top=pRect->bottom-MinHeight;
			return TRUE;
		}
		break;
	case WMSZ_TOPLEFT:
		if(NewWidth < MinWidth || NewHeight < MinHeight)
		{
			if(NewWidth < MinWidth)
				pRect->left=pRect->right-MinWidth;
			if(NewHeight < MinHeight)
				pRect->top=pRect->bottom-MinHeight;	
			return TRUE;
		}
		break;
	case WMSZ_TOPRIGHT:
		if(NewWidth < MinWidth || NewHeight < MinHeight)
		{
			if(NewWidth < MinWidth)
				pRect->right=pRect->left+MinWidth;
			if(NewHeight < MinHeight)
				pRect->top=pRect->bottom-MinHeight;
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
}

BOOL OnSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	switch(wParam)
	{
	case SIZE_RESTORED:		// ����С���ָ�
		SetLayeredWindowAttributes(hWnd,RGB(255,0,0),0,LWA_COLORKEY | LWA_ALPHA);
		SetTimer(hWnd,ID_TRANS_UP,TRANS_TINTERVAL,NULL);
		if(g_needContinue)
		{
			SendMessage(hWnd,WM_COMMAND,(WPARAM)ID_CONTINUE,(LPARAM)0);
			g_needContinue=false;
		}
		return true;
	case SIZE_MINIMIZED:
		if(!g_isInit && g_isStart && !g_isStay)
		{
			//���������У�������ͣ�������ñ�׼needContinue
			SendMessage(hWnd,WM_COMMAND,(WPARAM)ID_PAUSE,(LPARAM)0);
			g_needContinue=true;
		}
		return true;
	case SIZE_MAXIMIZED:
		SetLayeredWindowAttributes(hWnd,RGB(255,0,0),0,LWA_COLORKEY | LWA_ALPHA);
		SetTimer(hWnd,ID_TRANS_UP,TRANS_TINTERVAL,NULL);
		return true;

	}
	return false;
}

void SetMenuItem(HWND hWnd)
{
	HMENU hSubMenu;
	hSubMenu=GetSubMenu(g_hMenu,0);

	if(g_isInit)
	{
		// ���ڳ�ʼ��״̬
		EnableMenuItem(hSubMenu,ID_INIT,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);		// ��ʼ��-disabled

		EnableMenuItem(hSubMenu,ID_START,MF_BYCOMMAND | MF_ENABLED);					// ����-enabled
		EnableMenuItem(hSubMenu,ID_STOP,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);		// ֹͣ-disabled

		EnableMenuItem(hSubMenu,ID_PAUSE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);		// ��ͣ-disabled
		EnableMenuItem(hSubMenu,ID_CONTINUE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);	// ����-enabled
	}
	else if(!g_isInit && g_isStart && !g_isStay)
	{
		// ��������������״̬
		EnableMenuItem(hSubMenu,ID_INIT,MF_BYCOMMAND | MF_ENABLED);					

		EnableMenuItem(hSubMenu,ID_START,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);		
		EnableMenuItem(hSubMenu,ID_STOP,MF_BYCOMMAND | MF_ENABLED);						

		EnableMenuItem(hSubMenu,ID_PAUSE,MF_BYCOMMAND | MF_ENABLED);		
		EnableMenuItem(hSubMenu,ID_CONTINUE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);	
	}
	else if(!g_isInit && g_isStart && g_isStay)
	{
		// ������������ͣ״̬
		EnableMenuItem(hSubMenu,ID_INIT,MF_BYCOMMAND | MF_ENABLED);					

		EnableMenuItem(hSubMenu,ID_START,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);		
		EnableMenuItem(hSubMenu,ID_STOP,MF_BYCOMMAND | MF_ENABLED);						

		EnableMenuItem(hSubMenu,ID_PAUSE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);		
		EnableMenuItem(hSubMenu,ID_CONTINUE,MF_BYCOMMAND | MF_ENABLED);	
	}
	else if(!g_isInit && !g_isStart)
	{
		// ����ֹͣ״̬
		EnableMenuItem(hSubMenu,ID_INIT,MF_BYCOMMAND | MF_ENABLED);					// ��ʼ��-disabled

		EnableMenuItem(hSubMenu,ID_START,MF_BYCOMMAND | MF_ENABLED);					// ����-enabled
		EnableMenuItem(hSubMenu,ID_STOP,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);		// ֹͣ-disabled

		EnableMenuItem(hSubMenu,ID_PAUSE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);		// ��ͣ-disabled
		EnableMenuItem(hSubMenu,ID_CONTINUE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);	// ����-
	}
}

/*
��hWnd����������isEnterFull����������Ϊȫ�������˳�ȫ��
isEnterFull=true;		// ��Ϊȫ��
isEnterFull=false;		// �˳�ȫ��

��isEnterFullΪ��ʱ��pRect�����������ڴ�С��pStyle���������Ĵ�����ʽ����Ϊ�����������壬�����߱��뱣��˲�����
��isEnterFullΪ��ʱ��pRect���ڴ����������ڴ�С��pStyle���������Ĵ�����ʽ��
*/
bool FullScreen(HWND hWnd ,RECT *pRect,LONG *pStyle,bool isEnterFull)
{
	LONG tmpStyle;
	int sx,sy;						// ȫ����͸�


	if(isEnterFull)					// ����ȫ��
	{
		// ��������
		GetWindowRect(hWnd,pRect);
		*pStyle=GetWindowLong(hWnd,GWL_STYLE);

		tmpStyle=*pStyle;
		tmpStyle&=~WS_BORDER;		// �߿�
		tmpStyle&=~WS_CAPTION;		// ������
		tmpStyle&=~WS_SIZEBOX;		// ��������

		sx=GetSystemMetrics(SM_CXSCREEN);
		sy=GetSystemMetrics(SM_CYSCREEN);

		SetWindowLong(hWnd,GWL_STYLE,tmpStyle);
		SetWindowPos(hWnd,HWND_TOPMOST,0,0,sx,sy,SWP_SHOWWINDOW);

		return true;
	}
	else if(!isEnterFull)			// �˳�ȫ��
	{
		SetWindowLong(hWnd,GWL_STYLE,*pStyle);
		SetWindowPos(hWnd,HWND_NOTOPMOST,
			pRect->left,	
			pRect->top,
			pRect->right-pRect->left,
			pRect->bottom-pRect->top,
			SWP_SHOWWINDOW);
	}
	return false;
}
// 
void OnTimer(HWND hWnd,WPARAM wParam ,LPARAM lParam)
{
	static bool isFirst=true;
	if(isFirst)
	{
		g_TransUp=0;
		g_TransDown=255;
		isFirst=false;
	}
	switch(wParam)
	{
	case ID_BALL_MOVE:			// ���˶�
	{
		HDC hdc = GetDC(hWnd);
		RECT rtClient = { 0 };
		GetClientRect(GetDesktopWindow(), &rtClient);
		DeleteObject(SelectObject(g_hCompatibleDC, CreateCompatibleBitmap(hdc, rtClient.right, rtClient.bottom)));
		HBRUSH br = CreateSolidBrush(0x808080);
		FillRect(g_hCompatibleDC, &rtClient, br);
		DeleteObject(br);
		g_HandleBallInst.Move(10, false);
		g_HandleBallInst.Show(0, false);
		BitBlt(hdc, 0, 0, rtClient.right, rtClient.bottom, g_hCompatibleDC, 0, 0, SRCCOPY);
		ReleaseDC(hWnd, hdc);
	}
		break;
	case ID_TRANS_UP:			// ����
		if(g_TransUp<=255)
		{
			SetLayeredWindowAttributes(hWnd,RGB(255,0,0),g_TransUp,LWA_COLORKEY | LWA_ALPHA);
			g_TransUp+=3;
		}
		else 
		{
			KillTimer(hWnd,ID_TRANS_UP);
			isFirst=true;
		}

		break;	
	case ID_TRANS_DOWN:			// ����
		if(g_TransDown>=0)
		{
			SetLayeredWindowAttributes(hWnd,RGB(255,0,0),g_TransDown,LWA_COLORKEY | LWA_ALPHA);
			g_TransDown-=3;
		}
		else
		{
			KillTimer(hWnd,ID_TRANS_DOWN);
			PostQuitMessage(0);
		}
		break;
	case ID_AUTORUN:		// �Զ�����
		// �����Զ�������Ϣ������ʱ��ؼ�
		SendMessage(hWnd,WM_COMMAND,(WPARAM)ID_START,(LPARAM)0);	
		KillTimer(hWnd,ID_AUTORUN);
		break;
	}
}

/*
�Ƿ��д˳����ʵ���������У�
����
���ܹ��õ��䴰����������Ϣ���伤��
������ر�
������
�򷵻�false
*/ 
bool isExist()
{
	HANDLE hEvent;
	HWND hWnd;
	if((hEvent=CreateEvent(NULL,TRUE,FALSE,szEvent))		// ����һ���˹����� �� ��ʼΪ���źŵ��¼�
		&& GetLastError()==ERROR_ALREADY_EXISTS)
	{
		// ����˳�����ʵ��������ͼ�õ��䴰�ھ��
		if(hWnd=FindWindow(szWindowClass,szTitle))
		{
			SendNotifyMessage(hWnd,UM_ACTIVEEXIST,(WPARAM)0,(LPARAM)0);
			return true;
		}
		else 
		{
			// �õ�������ʧ�ܣ�����ر�
			SetEvent(hEvent);
			return false;
		}
	}
	CreateThread(NULL,0,WaitForExit,(LPVOID)hEvent,0,NULL);
	return false;
}

/*
���߳�ֻ��һֱ�ȴ�
ֱ��lpParameter����ʾ���¼����ź�
�򽫵�ǰ���̹ر�
*/
DWORD WINAPI WaitForExit(LPVOID lpParameter)
{
	WaitForSingleObject((HANDLE)lpParameter,INFINITE);
	TerminateProcess(GetCurrentProcess(),0);
	return 0;
}

bool LoadAndWriteMIDFile(HMODULE hModule,LPWSTR lpFileName)
{
	HRSRC hRsrc;
	HGLOBAL hGlobal;
	LPVOID lpMidAddr=0;			// ����mid��Դ���ڴ�λ��
	HANDLE hFile;
	DWORD dwRet;

	// ������Դ���õ��ڴ��ַ
	hRsrc=FindResource(hModule,MAKEINTRESOURCE(IDR_MID),L"MID");
	if(!hRsrc)
		return false;
	ULONG FileLen = ::SizeofResource(NULL,hRsrc);  // Data size/length  
	hGlobal=LoadResource(hModule,hRsrc);
	if(!hGlobal)
		return false;

	lpMidAddr=LockResource(hGlobal);
	if(!lpMidAddr)
		return false;

	hFile=CreateFile(lpFileName,GENERIC_READ | GENERIC_WRITE ,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!hFile)
		return false;
	bool bRet = false;
	WriteFile(hFile,lpMidAddr,FileLen,&dwRet,NULL);
	if( dwRet == FileLen)
		bRet = true;
	CloseHandle(hFile);
	return bRet;
}


int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd) 
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MOVEBALL, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_EVENT, szEvent,MAX_LOADSTRING);
	LoadString(hInstance, IDS_ACTIVEEXIST,szActiveExist,MAX_LOADSTRING);
	LoadString(hInstance, IDS_BGMUSIC,szBgMusic,MAX_LOADSTRING);

	// ע���Զ�����Ϣ
	UM_ACTIVEEXIST=RegisterWindowMessage(szActiveExist);

	// ����Ƿ�������ʵ����������
	if(isExist())
		return 0;	// ��������򷵻�

	// ׼����������
	LoadAndWriteMIDFile(hInstance,szBgMusic);

	MyRegisterClass(hInstance);

	// ����λͼ
	if(g_hbmpBg=LoadBitmap(hInstance,MAKEINTRESOURCEW(IDB_BG)))
	{
		GetObject(g_hbmpBg,sizeof(g_BgbmpInfo),&g_BgbmpInfo);
	}

	if(g_hbmpDream=LoadBitmap(hInstance,MAKEINTRESOURCEW(IDB_DREAM)))
	{
		GetObject(g_hbmpDream,sizeof(g_DreamBmpInfo),&g_DreamBmpInfo);
	}

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nShowCmd))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOVEBALL));

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	if(g_hbmpBg && DeleteObject(g_hbmpBg))
		g_hbmpBg=NULL;
	if(g_hbmpDream && DeleteObject(g_hbmpDream))
		g_hbmpDream=NULL;

	return (int) msg.wParam;
}


