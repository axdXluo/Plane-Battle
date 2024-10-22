#include <graphics.h>//引用图形库头文件
#include <iostream>
#include <ctime>
#include <windows.h>  
using namespace std;

const int WIDTH = 400;  //画面的宽
const int LENGTH = 650;  //画面的长
const double Init_Plane_x_label = (double)WIDTH / 2.0 - (double)WIDTH / 8.0;   //我方飞机的初始x坐标
const double Init_Plane_y_label = (double)LENGTH - (double)LENGTH / 6.0;   //我方飞机的初始y坐标
const double Plane_speed = 5;				//我方飞机的移动速度
const double Bullet_speed = 2;				//我方子弹的移动速度
const double Bullet_interval_ms = 150;		//我方子弹的发射间隔，单位ms
const int Bullet_Num = 50;					//我方子弹的数量

const double Enemy_Speed1 = 0.8;			//敌方飞机1的移动速度
const double Enemy_Speed2 = 0.3;			//敌方飞机2的移动速度
const double Enemy_Speed3 = 0.1;			//敌方飞机3的移动速度

const int Enemy_Num1 = 15;   //敌方飞机1的数量
const int Enemy_Num2 = 5;	 //敌方飞机2的数量
const int Enemy_Num3 = 3;	 //敌方飞机3的数量

const int Plane_HP = 3;		//我方飞机血量
const int Enemy_HP1 = 5;	//敌方飞机1血量
const int Enemy_HP2 = 10;	//敌方飞机2血量
const int Enemy_HP3 = 20;	//敌方飞机3血量

int Enemy_Recent_Num1 = Enemy_Num1;		//敌方飞机1当前数量
int Enemy_Recent_Num2 = Enemy_Num2;		//敌方飞机2当前数量
int Enemy_Recent_Num3 = Enemy_Num3;		//敌方飞机3当前数量

const double Enemy_interval_ms1 = 1000;	//敌方飞机1产生间隔，单位ms
const double Enemy_interval_ms2 = 3000;	//敌方飞机2产生间隔，单位ms
const double Enemy_interval_ms3 = 5000;	//敌方飞机2产生间隔，单位ms

//图片数据
IMAGE Background;			//游戏背景图
IMAGE Plane[2];				//我方飞机图
IMAGE Bullet[2];			//我方子弹图
IMAGE Enemy[3][2];			//敌方飞机图
IMAGE Enemy_Dowm[3][6];		//敌方飞机炸毁图

//飞机数据
struct Plane
{
	double x_label;	//飞机x轴坐标数据
	double y_label;	//飞机y轴坐标数据
	double speed;	//飞机飞行速度
	int hp;			//飞机血量
	int width;		//飞机宽度
	int height;		//飞机高度
	int type;		//飞机类型 0 我方飞机  1 敌方飞机1  2 敌方飞机2   3  敌方飞机3
	bool live;      //判断是否存活
	bool broken;	//判断是否被摧毁过
};

struct Plane myPlane = { Init_Plane_x_label, Init_Plane_y_label, Plane_speed, Plane_HP, 102, 126, 0, true,false };
struct Plane EnemyPlane1[Enemy_Num1];
struct Plane EnemyPlane2[Enemy_Num2];
struct Plane EnemyPlane3[Enemy_Num3];


//我方子弹数据
struct Bullet_Index
{
	double x_label;						//我方子弹x轴坐标数据
	double y_label;						//我方子弹y轴坐标数据
	double speed = Bullet_speed;		//我方子弹移动距离
	bool live = false;					//我方子弹存活标志
};
struct Bullet_Index bullet_index[Bullet_Num];


//读取图片
void InitImage()
{	
	loadimage(&Background, "./images/bk.png");   //背景图

	loadimage(&Plane[0], "./images/me1.png");    //我方飞机图
	loadimage(&Plane[1], "./images/me2.png");	 //我方飞机图

	loadimage(&Bullet[0], "./images/bullet1.png");   //子弹图
	loadimage(&Bullet[1], "./images/bullet2.png");   //子弹图

	loadimage(&Enemy[0][0], "./images/enemy1.png"); loadimage(&Enemy[0][1], "./images/enemy1.png");   //敌方飞机1图
	loadimage(&Enemy[1][0], "./images/enemy2.png"); loadimage(&Enemy[1][1], "./images/enemy2_hit.png");    //敌方飞机2图
	loadimage(&Enemy[2][0], "./images/enemy3_n1.png"); loadimage(&Enemy[2][1], "./images/enemy3_n2.png");  //敌方飞机3图

	//敌方类型1机型坠毁图
	loadimage(&Enemy_Dowm[0][0], "./images/enemy1_down1.png");
	loadimage(&Enemy_Dowm[0][1], "./images/enemy1_down2.png");
	loadimage(&Enemy_Dowm[0][2], "./images/enemy1_down3.png");
	loadimage(&Enemy_Dowm[0][3], "./images/enemy1_down4.png");

	//敌方类型2机型坠毁图
	loadimage(&Enemy_Dowm[1][0], "./images/enemy2_down1.png");
	loadimage(&Enemy_Dowm[1][1], "./images/enemy2_down2.png");
	loadimage(&Enemy_Dowm[1][2], "./images/enemy2_down3.png");
	loadimage(&Enemy_Dowm[1][3], "./images/enemy2_down4.png");

	//敌方类型3机型坠毁图
	loadimage(&Enemy_Dowm[2][0], "./images/enemy3_down1.png");
	loadimage(&Enemy_Dowm[2][1], "./images/enemy3_down2.png");
	loadimage(&Enemy_Dowm[2][2], "./images/enemy3_down3.png");
	loadimage(&Enemy_Dowm[2][3], "./images/enemy3_down4.png");
	loadimage(&Enemy_Dowm[2][4], "./images/enemy3_down5.png");
	loadimage(&Enemy_Dowm[2][5], "./images/enemy3_down6.png");
}

//单位为ms的定时器，需要使用两个定时器，否则子弹的产生和敌机的产生会相互影响
bool Timer(int ms,int id)
{
	static DWORD t[10];
	if (clock() - t[id] > ms)
	{
		t[id] = clock();
		return true;
	}
	return false;
}

int GetRand(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

//加载图片
void PutImage()
{
	InitImage();
	putimage(0, 0, &Background);   //背景图片位置
	//地图边界处理，让飞机始终位于窗口内
	if (myPlane.y_label < 0)   myPlane.y_label = 0;
	if (myPlane.y_label > LENGTH - LENGTH/7) myPlane.y_label = (double)LENGTH - (double)LENGTH / 7.0;
	if (myPlane.x_label < 0)   myPlane.x_label = 0;
	if (myPlane.x_label > WIDTH- WIDTH/4) myPlane.x_label = (double)WIDTH- (double)WIDTH / 4.0;

	//初始化飞机位置
	putimage(myPlane.x_label, myPlane.y_label, &Plane[0], NOTSRCERASE);
	putimage(myPlane.x_label, myPlane.y_label, &Plane[1], NOTSRCERASE);
}

//按下空格后将live置为true，创建子弹
void Creat_Bullet()
{
	for (int i = 0; i < Bullet_Num; i++)
	{
		if (bullet_index[i].live != true)
		{
			bullet_index[i].live = true;
			bullet_index[i].x_label = myPlane.x_label + 50;
			bullet_index[i].y_label = myPlane.y_label - 5;
			break;
		}
	}
}

//每产生一个子弹，就控制这个子弹的飞行
void Bullet_Control()
{
	for (int i = 0; i < Bullet_Num; i++)
	{
		if (bullet_index[i].live == true)   //利用子弹是否存活来判断是否绘制子弹
		{
			putimage(bullet_index[i].x_label, bullet_index[i].y_label, &Bullet[0], NOTSRCERASE);
			putimage(bullet_index[i].x_label, bullet_index[i].y_label, &Bullet[1]);
			bullet_index[i].y_label -= bullet_index[i].speed;
			if (bullet_index[i].y_label < 0)bullet_index[i].live = false;   //子弹出界时子弹的生命周期结束
		}
	}
}

//初始化敌方机型数据
void Init_Enemy()
{
	for (int i = 0; i < Enemy_Num1; i++)
		EnemyPlane1[i] = {0, 0, Enemy_Speed1, Enemy_HP1, 57, 43, 1, false,false };
	for (int i = 0; i < Enemy_Num2; i++)
		EnemyPlane2[i] = { 0, 0, Enemy_Speed2, Enemy_HP2, 69, 99, 2, false ,false };
	for (int i = 0; i < Enemy_Num3; i++)
		EnemyPlane3[i] = { 0, 0, Enemy_Speed3, Enemy_HP3, 165, 261, 3, false ,false };
}

//生成敌方飞机1
void Creat_Enemy1()
{
	cout << "Enemy_Num1 - Enemy_Recent_Num1:" << Enemy_Num1 - Enemy_Recent_Num1 << endl;
	//生成类型1敌机
	for (int i = Enemy_Num1 - Enemy_Recent_Num1; i < Enemy_Num1; i++)
	{
		if (EnemyPlane1[i].live != true && EnemyPlane1[i].broken == false)
		{
			EnemyPlane1[i].live = true;
			EnemyPlane1[i].x_label = GetRand(0, WIDTH - 57);
			EnemyPlane1[i].y_label = 0;
			break;
		}
	}
}

//生成敌方飞机2
void Creat_Enemy2()
{
	//生成类型2敌机
	for (int i = Enemy_Num2 - Enemy_Recent_Num2; i < Enemy_Num2 ; i++)
	{
		if (EnemyPlane2[i].live != true && EnemyPlane2[i].broken == false)
		{
			EnemyPlane2[i].live = true;
			EnemyPlane2[i].x_label = GetRand(0, WIDTH - 57);
			EnemyPlane2[i].y_label = 0;
			break;
		}
	}
}

//生成敌方飞机3
void Creat_Enemy3()
{
	//生成类型1敌机
	for (int i = Enemy_Num3 - Enemy_Recent_Num3; i < Enemy_Num3; i++)
	{
		if (EnemyPlane3[i].live != true && EnemyPlane3[i].broken == false)
		{
			EnemyPlane3[i].live = true;
			EnemyPlane3[i].x_label = GetRand(0, WIDTH - 57);
			EnemyPlane3[i].y_label = 0;
			break;
		}
	}
}

//敌方飞机的飞行控制
void Enemy_Control()
{
	//制造敌机类型1
	if (Enemy_Recent_Num1 > 0)
	{
		for (int i = 0; i < Enemy_Num1; i++)
		{
			if (EnemyPlane1[i].live == true)
			{
				switch (EnemyPlane1[i].hp)
				{
				case 5:
					putimage(EnemyPlane1[i].x_label, EnemyPlane1[i].y_label, &Enemy[0][0], NOTSRCERASE);
					putimage(EnemyPlane1[i].x_label, EnemyPlane1[i].y_label, &Enemy[0][0], NOTSRCERASE);
					break;
				case 4:
					putimage(EnemyPlane1[i].x_label, EnemyPlane1[i].y_label, &Enemy_Dowm[0][0], NOTSRCERASE);
					putimage(EnemyPlane1[i].x_label, EnemyPlane1[i].y_label, &Enemy_Dowm[0][0], NOTSRCERASE);
					break;
				case 3:
					putimage(EnemyPlane1[i].x_label, EnemyPlane1[i].y_label, &Enemy_Dowm[0][1], NOTSRCERASE);
					putimage(EnemyPlane1[i].x_label, EnemyPlane1[i].y_label, &Enemy_Dowm[0][1], NOTSRCERASE);
					break;
				case 2:
					putimage(EnemyPlane1[i].x_label, EnemyPlane1[i].y_label, &Enemy_Dowm[0][2], NOTSRCERASE);
					putimage(EnemyPlane1[i].x_label, EnemyPlane1[i].y_label, &Enemy_Dowm[0][2], NOTSRCERASE);
					break;
				case 1:
					putimage(EnemyPlane1[i].x_label, EnemyPlane1[i].y_label, &Enemy_Dowm[0][3], NOTSRCERASE);
					putimage(EnemyPlane1[i].x_label, EnemyPlane1[i].y_label, &Enemy_Dowm[0][3], NOTSRCERASE);
					break;
				default:
					break;
				}
				EnemyPlane1[i].y_label += EnemyPlane1[i].speed;
				if (EnemyPlane1[i].y_label >= LENGTH)
				{
					cout << "越界:"<< i << endl;
					EnemyPlane1[i].live = false;
					Enemy_Recent_Num1--;
				}
			}
		}
	}
	else if (Enemy_Recent_Num2 > 0)   //当敌机类型1当前数量为0后，制造敌机类型2
	{
		for (int i = 0; i < Enemy_Num2; i++)
		{
			if (EnemyPlane2[i].live == true)
			{
				if (EnemyPlane2[i].hp > 7 && EnemyPlane2[i].hp <= 10)
				{
					putimage(EnemyPlane2[i].x_label, EnemyPlane2[i].y_label, &Enemy[1][0], NOTSRCERASE);
					putimage(EnemyPlane2[i].x_label, EnemyPlane2[i].y_label, &Enemy[1][1], NOTSRCERASE);
				}
				else if (EnemyPlane2[i].hp > 5 && EnemyPlane2[i].hp <= 7)
				{
					putimage(EnemyPlane2[i].x_label, EnemyPlane2[i].y_label, &Enemy_Dowm[1][0], NOTSRCERASE);
					putimage(EnemyPlane2[i].x_label, EnemyPlane2[i].y_label, &Enemy_Dowm[1][0], NOTSRCERASE);
				}
				else if (EnemyPlane2[i].hp > 3 && EnemyPlane2[i].hp <= 5)
				{
					putimage(EnemyPlane2[i].x_label, EnemyPlane2[i].y_label, &Enemy_Dowm[1][1], NOTSRCERASE);
					putimage(EnemyPlane2[i].x_label, EnemyPlane2[i].y_label, &Enemy_Dowm[1][1], NOTSRCERASE);
				}
				else if (EnemyPlane2[i].hp > 1 && EnemyPlane2[i].hp <= 3)
				{
					putimage(EnemyPlane2[i].x_label, EnemyPlane2[i].y_label, &Enemy_Dowm[1][2], NOTSRCERASE);
					putimage(EnemyPlane2[i].x_label, EnemyPlane2[i].y_label, &Enemy_Dowm[1][2], NOTSRCERASE);
				}
				else
				{
					putimage(EnemyPlane2[i].x_label, EnemyPlane2[i].y_label, &Enemy_Dowm[1][3], NOTSRCERASE);
					putimage(EnemyPlane2[i].x_label, EnemyPlane2[i].y_label, &Enemy_Dowm[1][3], NOTSRCERASE);
				}
				EnemyPlane2[i].y_label += EnemyPlane2[i].speed;
				if (EnemyPlane2[i].y_label > LENGTH)
				{
					EnemyPlane2[i].live = false;
					Enemy_Recent_Num2--;
				}
			}
		}
	}
	else if (Enemy_Recent_Num3 > 0)
	{
		for (int i = 0; i < Enemy_Num3; i++)
		{
			if (EnemyPlane3[i].live == true)
			{
				if (EnemyPlane3[i].hp > 15 && EnemyPlane3[i].hp <= 20)
				{
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy[2][0], NOTSRCERASE);
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy[2][1], NOTSRCERASE);
				}
				else if (EnemyPlane3[i].hp > 12 && EnemyPlane3[i].hp <= 15)
				{
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy_Dowm[2][0], NOTSRCERASE);
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy_Dowm[2][0], NOTSRCERASE);
				}
				else if (EnemyPlane3[i].hp > 9 && EnemyPlane3[i].hp <= 12)
				{
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy_Dowm[2][1], NOTSRCERASE);
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy_Dowm[2][1], NOTSRCERASE);
				}
				else if (EnemyPlane3[i].hp > 6 && EnemyPlane3[i].hp <= 9)
				{
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy_Dowm[2][2], NOTSRCERASE);
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy_Dowm[2][2], NOTSRCERASE);
				}
				else if(EnemyPlane3[i].hp > 3 && EnemyPlane3[i].hp <= 6)
				{
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy_Dowm[2][3], NOTSRCERASE);
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy_Dowm[2][3], NOTSRCERASE);
				}
				else if (EnemyPlane3[i].hp > 1 && EnemyPlane3[i].hp <= 3)
				{
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy_Dowm[2][4], NOTSRCERASE);
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy_Dowm[2][4], NOTSRCERASE);
				}
				else
				{
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy_Dowm[2][5], NOTSRCERASE);
					putimage(EnemyPlane3[i].x_label, EnemyPlane3[i].y_label, &Enemy_Dowm[2][5], NOTSRCERASE);
				}
				EnemyPlane3[i].y_label += EnemyPlane3[i].speed;
				if (EnemyPlane3[i].y_label > LENGTH)
				{
					EnemyPlane3[i].live = false;
					Enemy_Recent_Num3--;
				}
			}
		}
	}
}

//判断敌方机型是否被子弹攻击
bool isCrash(const struct Plane& plane, const struct Bullet_Index& bullet)
{
	if (plane.x_label< bullet.x_label && plane.x_label + plane.width-7 > bullet.x_label
		&& plane.y_label< bullet.y_label && plane.y_label + plane.height - 10 > bullet.y_label)
		return true;
	return false;
}

//碰撞检测
void Crash_Detection()
{
	bool Crash_Flag = false;
	if (Enemy_Recent_Num1 > 0)
	{
		//检测子弹是否攻击到敌方机型1
		for (int i = 0; i < Enemy_Num1; i++)	//遍历所有live为true的敌方机型
		{
			if (EnemyPlane1[i].live == true)
			{
				for (int j = 0; j < Bullet_Num; j++)   //遍历屏幕上的子弹
				{
					if (bullet_index[j].live == true )
					{
						Crash_Flag = isCrash(EnemyPlane1[i], bullet_index[j]);
						if (Crash_Flag)
						{
							EnemyPlane1[i].hp--;   //检测到碰撞，则敌方机型血量-1
							bullet_index[j].live = false;   //子弹不具有穿透效果
							cout << "第 " << i << " 机型的hp为 " << EnemyPlane1[i].hp << endl;
							
							if (EnemyPlane1[i].hp <= 0)   //如果敌方机型空血，则live为false，敌方机型1总数-1
							{
								EnemyPlane1[i].live = false;
								EnemyPlane1[i].broken = true;
								Enemy_Recent_Num1--;
							}
							cout << "第 " << i << " 机型此时存活为 " << EnemyPlane1[i].live << endl;
							cout << "----" << endl;
							break;   //因为子弹不具有穿透效果，因此检测到碰撞之后就不用再继续下去了
						}
					}
				}
			}
		}
	}
	else if (Enemy_Recent_Num2 > 0)   //当敌方机型1当前数量为0后，检测子弹是否攻击到敌方机型2
	{
		for (int i = 0; i < Enemy_Num2; i++)	//遍历所有live为true的敌方机型
		{
			if (EnemyPlane2[i].live == true)
			{
				for (int j = 0; j < Bullet_Num; j++)   //遍历屏幕上的子弹
				{
					if (bullet_index[j].live == true)
					{
						Crash_Flag = isCrash(EnemyPlane2[i], bullet_index[j]);
						if (Crash_Flag)
						{
							EnemyPlane2[i].hp--;   //检测到碰撞，则敌方机型血量-1
							bullet_index[j].live = false;   //子弹不具有穿透效果
							if (EnemyPlane2[i].hp <= 0)   //如果敌方机型空血，则live为false，敌方机型1总数-1
							{
								EnemyPlane2[i].live = false;
								EnemyPlane2[i].broken = true;
								Enemy_Recent_Num2--;
							}
							break;   //因为子弹不具有穿透效果，因此检测到碰撞之后就不用再继续下去了
						}
					}
				}
			}
		}
	}
	else if (Enemy_Recent_Num3 > 0)
	{
		//检测子弹是否攻击到敌方机型3
		for (int i = 0; i < Enemy_Num3; i++)	//遍历所有live为true的敌方机型
		{
			if (EnemyPlane3[i].live == true)
			{
				for (int j = 0; j < Bullet_Num; j++)   //遍历屏幕上的子弹
				{
					if (bullet_index[j].live == true)
					{
						Crash_Flag = isCrash(EnemyPlane3[i], bullet_index[j]);
						if (Crash_Flag)
						{
							EnemyPlane3[i].hp--;   //检测到碰撞，则敌方机型血量-1
							bullet_index[j].live = false;   //子弹不具有穿透效果
							if (EnemyPlane3[i].hp <= 0)   //如果敌方机型空血，则live为false，敌方机型1总数-1
							{
								EnemyPlane3[i].live = false;
								EnemyPlane3[i].broken = true;
								Enemy_Recent_Num3--;
							}
							break;   //因为子弹不具有穿透效果，因此检测到碰撞之后就不用再继续下去了
						}
					}
				}
			}
		}
	}
}

//键盘控制
void Key_Command()
{
	//键盘WASD，上下左右箭头，控制飞机上下左右移动
	if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W'))
		myPlane.y_label -= myPlane.speed;
	if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
		myPlane.y_label += myPlane.speed;
	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))
		myPlane.x_label -= myPlane.speed;
	if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))
		myPlane.x_label += myPlane.speed;

	//键盘空格键控制子弹发射，每Bullet_interval_ms发射一次子弹
	if (GetAsyncKeyState(VK_SPACE) && Timer(Bullet_interval_ms,1))
		Creat_Bullet();		//创建子弹
	
	//控制子弹的飞行	
	Bullet_Control();

	if (Enemy_Recent_Num1 > 0)
	{
		if (Timer(Enemy_interval_ms1, 2))	//创建敌机1，每Enemy_interval_ms创建一台敌机1
			Creat_Enemy1();
	}
	else if (Enemy_Recent_Num2 > 0)
	{
		if (Timer(Enemy_interval_ms2, 3))	//敌方机型1当前数量为0后，开始创建敌方机型2
			Creat_Enemy2();
	}
	else if (Enemy_Recent_Num3 > 0)
	{
		if (Timer(Enemy_interval_ms3, 4))	//敌方机型1当前数量为0后，开始创建敌方机型3
			Creat_Enemy3();
	}

	//控制敌机飞行
	Enemy_Control();

	//进行碰撞检测
	Crash_Detection();
}

int main()
{
	initgraph(WIDTH, LENGTH);//创建窗口
	srand((unsigned int) time(NULL));   //随着时间变化的随机数种子
	BeginBatchDraw();//双缓冲机制，解决画面闪烁问题
	Init_Enemy();   //初始化敌机
	while (1)
	{
		PutImage();
		Key_Command();

		FlushBatchDraw();	//双缓冲机制，解决画面闪烁问题
	};

	return 0;
}




