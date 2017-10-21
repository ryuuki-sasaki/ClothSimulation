//=====================================
//制作者：佐々木隆貴
//日付：2016/4/13
//=====================================
#ifndef _CLOTH_H_
#define _CLOTH_H_

//include
#include<iostream>
#include<vector>
#include<cmath>
#include <GL/glew.h>
#include <GL/glut.h>
#include "MyMath.h"

using namespace std;

//前方宣言
class DDSImage;

struct Grid
{
	double mass;			//質量
	vector3 position;		//位置
	vector3 force;			//力
	vector3 velocity;		//速度
	
	Grid()
	{
		position.Zero();
		force.Zero();
		velocity.Zero();
	}
};

//上下左右に伸びるばねを結ぶ質点の番号
struct Spring_Struct
{
	//質点番号
	int gridColumnNumber1 = 0;	//列番号
	int gridColumnNumber2 = 0;
	int gridRowNumber1 = 0;		//行番号
	int gridRowNumber2 = 0;
};

//斜めに伸びるばね(せん断ばね)を結ぶ質点の番号
struct Spring_Shear
{
	//質点番号
	int gridColumnNumber1 = 0;	//列番号
	int gridColumnNumber2 = 0;
	int gridRowNumber1 = 0;		//行番号
	int gridRowNumber2 = 0;
};

//蝶番ばねを結ぶ質点の番号
struct Spring_Hinge
{
	//質点番号
	int gridColumnNumber[4];	//列番号
	int gridRowNumber[4];		//行番号

	int pairNum = 0;	//接続質点のペア個数
};

//二次元配列で扱いたいからvector
typedef vector<vector<Grid>> Grids;
typedef vector<vector<Spring_Hinge>> Spring_Hinges;

class Cloth
{
public:
	Cloth();
	~Cloth();
	void Initialize();
	void LoadTexture(const char *fileName);
	void Update();
	void Render();
	const Grid& GetGrid(int x, int y) const { return grids[x][y]; }

private:
	void UpdateForce();
	void EularMethod();
	DDSImage* texture;

	Grids grids;
	Spring_Struct* s_struct;
	Spring_Shear* s_shear;
	Spring_Hinges s_hinge;

	int s_structSize;
	int s_shearSize;

	int counter;
};

#endif