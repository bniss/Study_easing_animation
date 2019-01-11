

//���������Ͳ���һЩ���Դﵽ������Ч��
/*
***************************����ϵ����****************************
1��x��������	������
2��y��������	������
3���Ƕ�������	����˳ʱ��
*/
#ifndef __HANDLE_BALL
#define __HANDLE_BALL

#include <Windows.h>

#define BALLNUMBER 30		// ������������Ŀ

class ball;

class HandleBall
{
private:
	ball *m_pBalls[BALLNUMBER];		// ָ�������ָ������
	int m_BallAngles[BALLNUMBER];		// ��Ӧ���ƶ���ƫ�ƽǶ�
	HWND m_hWnd;						// ���ھ��
	HDC  m_MemDc;


	int m_MoveLen;					// ���ƶ���λ����
	int m_ball_r;						// ����İ뾶

	

	//	˽�к�������
	int Reflect(int aSrc,int aRef);													// ����aSrc������aRef������֮��ķ��� 
	void MoveOneBall(ball* pBall,int Angle,int len);								// ���ո����ĽǶ�Angle,������λ��len�ƶ���������pBall;
	bool isHitWall(ball* pBall,int bdAngle,int len,RECT rt,int *Angle,POINT *Pos);	// ��pBall��ǰ�ƶ�len�����Ƿ���rt�ı���ײ�������ײ��Angle�д�ŵ��Ƿ���صķ���Pos�д�ŵ�����ǡ��ǽ��ײʱ��Բ��λ��
	int Bisector(int a1,int a2);													// ��a1��a2�������������кϳ�֮�󷵻�
	bool isTwoBallHit(ball* pActive,ball* pPassive,int *aActive,int *aPassive);		// ����������pActive�Ƿ��뱻����pPassive��ײ����ײ������ײ����ֱ�����aActive��aPassive֮��
	//	int DirectTwoPoint(POINT pt_src,POINT pt_dest);									// ������pt_srcΪ��㣬ָ��pt_dest�����߷���
public:		
	HandleBall();								//
	bool init(HWND hWnd,int ball_r,int MoveLen);		// ��ʼ��������ɳ�Ա�����ĳ�ʼ���Լ���Ӧ����Ĵ���
	~HandleBall();										// �������������
	void SetMemDc(HDC hdc);
	void Show(COLORREF col,bool isErase);				// ���isEraseΪ������col��ʾ���е��򣨿�������������,����col��Ч
	void Move(int mLen,bool UseDefault);				// �ƶ��˶������е������UseDefaultΪ�����ö����ڲ����ƶ�λ������Ϊ������mLen
	int DirectTwoPoint(POINT pt_src,POINT pt_dest);
};
#endif