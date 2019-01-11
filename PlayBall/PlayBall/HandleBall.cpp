#include "Handleball.h"
#include "ball.h"
#include <math.h>



#define COL_WHITE (RGB(255,255,255))
#define COL_BLUE (RGB(0,0,255))
#define COL_GREEN (RGB(0,255,0))


#define PI	3.1415926		// pi����
#define AngToRad(x)	(((float)(x))/180*PI)			// �Ƕ���ת��Ϊ������
#define RadToAng(x) (double(x)/PI*180)			    // ������ת��Ϊ�Ƕ���
#define DISTANCE(pt1,pt2) sqrt((float)(pt1.x-pt2.x)*(pt1.x-pt2.x)+(pt1.y-pt2.y)*(pt1.y-pt2.y))	   // ��������֮��ľ��� 

HandleBall::HandleBall()
{
	m_MemDc = 0;
}

/*==================HandleBall���Ա������ʵ��==================*/
/*********************���г�Ա����ʵ�ֲ���***********************/
bool HandleBall::init(HWND hWnd,int ball_r,int MoveLen)
{
	int i;
	POINT pt;							// ��ʱ�����������λ������
	RECT rt;							// ���洰���������

	SYSTEMTIME stime;
	GetSystemTime(&stime);
	WORD wCount=stime.wMilliseconds%100;
	while(wCount--)
		rand();				// ��ϵͳʱ���΢����Ϊ���ӳ�ʼ��rand�������Ӷ���֤ÿһ�εõ��Ķ�����һ���Ĳ���

	this->m_hWnd=hWnd;
	this->m_ball_r=ball_r;
	this->m_MoveLen=MoveLen;

	// �õ������С
	GetClientRect(hWnd,&rt);

	for(i=0;i<BALLNUMBER;i++)
	{
		// ������i�Լ�����Ӧ���ƶ�����Ƕ�
		this->m_pBalls[i]=new ball();
		this->m_BallAngles[i]=rand()%360;

		// ��ʼ����i��
		pt.x=(LONG)((abs(rand())%(int)(rt.right-2*ball_r))+ball_r);
		pt.y=(LONG)((abs(rand())%(int)(rt.bottom-2*ball_r))+ball_r);

		this->m_pBalls[i]->init(pt,ball_r,RGB(rand()%255,rand()%255,rand()%255));
		this->m_pBalls[i]->SetId(i);					// ���ô���Ψһ��ʶ
	}
	return true;
}

HandleBall::~HandleBall()
{
	for(int i=0;i<BALLNUMBER;i++)
		delete this->m_pBalls[i];
}

void HandleBall::SetMemDc(HDC hdc)
{
	m_MemDc = hdc;
}

void HandleBall::Show(COLORREF col,bool isErase)
{
	int i;
	if(false==isErase)
	{	for(i=0;i<BALLNUMBER;i++)
	this->m_pBalls[i]->draw(m_MemDc, 0, false);
	}
	else
	{
		for(i=0;i<BALLNUMBER;i++)
			this->m_pBalls[i]->draw(m_MemDc, col, true);
	}
}

void HandleBall::Move(int mLen, bool UseDefault)
{
	if(true==UseDefault)
		mLen=this->m_MoveLen;
	for(int i=0;i<BALLNUMBER;i++)
		this->MoveOneBall(this->m_pBalls[i],this->m_BallAngles[i],mLen);
}

/*********************˽�г�Ա����ʵ�ֲ���***********************/

/*
*******************�ƶ���**********************
��pBallָ������������ƶ���
Nx=Ox+len*cos(Angle)
Ny=Oy+len*sin(Angle)
����(Ox,Oy)Ϊ�ƶ�ǰ��λ�ã���(Nx,Ny)Ϊ�ƶ����λ��
*/
void HandleBall::MoveOneBall(ball *pBall,int Angle,int len)
{
	POINT OldPos,NewPos,tmpPos;
	RECT rt = {0};
	int tempAngle = 0;
	OldPos=pBall->GetPos();

	NewPos.x=OldPos.x+len*cos((float)AngToRad(Angle));
	NewPos.y=OldPos.y+len*sin((float)AngToRad(Angle));

	// �������ı���ײ���
	GetClientRect(this->m_hWnd,&rt);
	if(this->isHitWall(pBall,Angle,len,rt,&tempAngle,&tmpPos))
	{
		this->m_BallAngles[pBall->GetId()]=tempAngle;		// �ı�����˶�����

		NewPos.x=OldPos.x+len*cos((float)AngToRad(tempAngle));
		NewPos.y=OldPos.y+len*sin((float)AngToRad(tempAngle));

		pBall->SetPos(NewPos);

	}
	else
		pBall->SetPos(NewPos);

	//	������֮����ײ����
	int aActive,aPassive;
	for(int i=0;i<BALLNUMBER;i++)
	{
		// ɨ��ȫ������pBall���򣬲������ж�
		if(i!=pBall->GetId() && this->isTwoBallHit(pBall,this->m_pBalls[i],&aActive,&aPassive))
		{
			this->m_BallAngles[pBall->GetId()]=aActive;
			this->m_BallAngles[i]=aPassive;
		}
	}
}

/*
**********************�����㷨*******************
1����aRef(�༴�赲����)Ϊx��������������ϵ
2��(360-aRef)��Ϊ�������ᰴ˳ʱ����ת���׼�������غ�����Ҫ����С�Ƕ�
*/
int HandleBall::Reflect(int aSrc, int aRef)
{
	int aResult;					// 
	int n_aResult;					// ����������aResult�Ƕ�
	int offset=(360-aRef)%360;		// ��������֮���ƫ�������μ����϶���
	int n_aSrc=(aSrc+offset)%360;	// ����������aSrc�Ƕ�

	// �൱�򲻴��ڷ���Ŀ��ܣ�����-1�������
	if(aSrc==aRef)
		return -1;

	n_aResult=360-n_aSrc;

	// ���������׼����ת��	
	aResult=abs(n_aResult-offset);	
	return aResult;
}

bool HandleBall::isHitWall(ball *pBall,int bdAngle, int len, RECT rt, int *Angle, POINT *Pos)
{
	POINT newPos;		// �������ƶ������λ��
	newPos.x=pBall->GetPos().x+len*cos((float)AngToRad(bdAngle));
	newPos.y=pBall->GetPos().y+len*sin((float)AngToRad(bdAngle));

	// pos��ʼ�������ش˱�����Ч
	Pos->x=-1;
	Pos->y=-1;

	// ���ȿ������ĸ�����ײ�����
	if(newPos.y<=rt.top && newPos.x<=rt.left)
	{
		// ���Ͻ�
		*Angle=this->Reflect(bdAngle,135);
		return true;

	}
	if(newPos.y<=rt.top && newPos.x>=rt.right)
	{
		// ���Ͻ�
		*Angle=this->Reflect(bdAngle,225);
		return true;
	}
	if(newPos.y>=rt.bottom && newPos.x<=rt.left)
	{
		// ���½�
		*Angle=this->Reflect(bdAngle,45);
		return true;
	}
	if(newPos.y>=rt.bottom && newPos.x>=rt.right)
	{
		// ���½�
		*Angle=this->Reflect(bdAngle,315);
		return true;
	}
	// �����ı���ײ�����
	if(newPos.y<=rt.top)
	{
		// ��
		*Angle=this->Reflect(bdAngle,0);
		return true;

	}
	if(newPos.y>=rt.bottom)
	{
		// ��
		*Angle=this->Reflect(bdAngle,0);
		return true;
	}
	if(newPos.x<=rt.left)
	{
		// ��
		*Angle=this->Reflect(bdAngle,90);
		return true;
	}
	if(newPos.x>=rt.right)
	{
		// ��
		*Angle=this->Reflect(bdAngle,270);
		return true;
	}
	return false;
}

/*************************������ƽ�����㷨****************************
1�������Ƕ�ͬ�򣨼���ľ���ֵС��180����
aResult=(a1+a2)/2
2�������Ƕȷ���
aResult=((a1+a2)/2+180)%360
*/
int HandleBall::Bisector(int a1, int a2)
{
	int Result;
	abs(a1-a2)>180? Result=(((a1+a2)/2+180)%360):Result=((a1+a2)/2);
	return Result;
}

/***********************************************************
������Ȼ�����Զ���������ᣬ������ᵽ�����޶������£�
�����·�˳ʱ������Ϊһ���������������ޡ�
*/
int HandleBall::DirectTwoPoint(POINT pt_src,POINT pt_dest)
{
	float temp;
	//	�������޿������
	if(pt_dest.x>pt_src.x && pt_dest.y>=pt_src.y)
	{
		// ��һ���ޣ�����0��
		temp=atan((float)((float)(pt_dest.y-pt_src.y)/(float)(pt_dest.x-pt_src.x)));

		return (int)RadToAng(temp);
	}
	if(pt_dest.x<=pt_src.x && pt_dest.y>pt_src.y)
	{
		// �ڶ����ޣ�����90��
		temp=atan((float)((float)(pt_src.x-pt_dest.x)/(float)(pt_dest.y-pt_src.y)));
		return (int)RadToAng(temp)+90;
	}
	if(pt_dest.x<pt_src.x && pt_dest.y<=pt_src.y)
	{
		// �������ޣ�����180��
		temp=(atan((float)((float)(pt_src.y-pt_dest.y)/(float)(pt_src.x-pt_dest.x))));
		return (int)RadToAng(temp)+180;
	}
	if(pt_dest.x>=pt_src.x && pt_dest.y<pt_src.y)
	{
		// �������ޣ�����270��
		temp=(atan((float)((float)(pt_dest.x-pt_src.x)/(float)(pt_src.y-pt_dest.y))));
		return (int)RadToAng(temp)+270;
	}
	return -1;
}

bool HandleBall::isTwoBallHit(ball *pActive, ball *pPassive, int *aActive, int *aPassive)
{
	int RefDirect;		// ������ײʱ��������ķ��䷽��
	int aPerDirect;		// ��������ǰ����
	int pPerDirect;		// ��������ǰ����

	// ��������֮��ľ���С������뾶֮��ʱ����ײ
	if(DISTANCE(pActive->GetPos(),pPassive->GetPos())<=(pActive->GetR()+pPassive->GetR()))
	{	
		aPerDirect=this->m_BallAngles[pActive->GetId()];
		pPerDirect=this->m_BallAngles[pPassive->GetId()];

		RefDirect=this->DirectTwoPoint(pPassive->GetPos(),pActive->GetPos());

		// �����򷵻ط������������߷������Լ�ԭ������������
		*aActive=this->Bisector(RefDirect,aPerDirect);

		// �����򷵻ط����������������Լ�ԭ������������
		*aPassive=this->Bisector(aPerDirect,pPerDirect);
		return true;

	}
	else
		return false;
}