//=====================================
//制作者：佐々木隆貴
//日付：2016/4/13
//=====================================
//include
#include "Cloth.h"
#include "DDSLoader.h"

//global
const int numGrid = 20;			//グリッド数
const double gridMass = 1.0f;	//質量
const double gravity = 9.8;		//重力加速度
const double windForce = 25.0f; //風力
const double damping = 0.25;	//ダンパ定数
const double drag = 0.1;		//粘性係数
const double k = 500.0;			//構造ばねの定数
const double kd = 250.0;		//せん断ばねの定数
const double hingeK = 0.01;		//ヒンジ(蝶番ばね)定数
const double spring_length = 3.0;	//構造ばねの大きさ
const double d_spring_length = spring_length * sqrt(2.0);	//せん断ばねの大きさ
const double dt = 0.01;			//微小時間
const double repForce = 0.05;	//質点同士の斥力
const double mu = 0.5;			//動摩擦係数
const double e = 0.3;			//反発係数
const double PI = 3.14159265358979323846264338327;

//=====================================
//コンストラクタ
//=====================================
Cloth::Cloth() : counter(0)
{
	grids.resize(numGrid);	
	s_hinge.resize(numGrid);
	for (int i = 0; i < numGrid; i++)
	{
		grids[i].resize(numGrid);
		s_hinge[i].resize(numGrid);
	}
}

//=====================================
//デストラクタ
//=====================================
Cloth::~Cloth()
{
	delete texture;
	delete s_struct;
	delete s_shear;
	texture = 0;
	s_struct = 0;
	s_shear = 0;
}

//=====================================
//初期化
//=====================================
void Cloth::Initialize()
{
	int counter = 0;

	for (int j = 0; j < numGrid; j++)
	{
		for (int i = 0; i < numGrid; i++)
		{
			grids[j][i].mass = gridMass;
			grids[j][i].position.x = ((double)i - (double)(numGrid - 1.0) / 2.0) * spring_length;
			grids[j][i].position.y = (double)j * spring_length;
			grids[j][i].position.z = 0.0;
		}
	}
	
	//ばねに接続する左右の質点番号
	s_struct = new Spring_Struct[numGrid * numGrid * 2];
	counter = 0;
	for (int j = 0; j < numGrid; j++)
	{
		for (int i = 0; i < numGrid - 1; i++)
		{
			s_struct[counter].gridRowNumber1 = j; s_struct[counter].gridColumnNumber1 = i;
			s_struct[counter].gridRowNumber2 = j; s_struct[counter].gridColumnNumber2 = i + 1;
			
			counter++;
		}
	}
	//ばねに接続する上下の質点番号
	for (int j = 0; j < numGrid - 1; j++)
	{
		for (int i = 0; i < numGrid; i++)
		{
			s_struct[counter].gridRowNumber1 = j;     s_struct[counter].gridColumnNumber1 = i;
			s_struct[counter].gridRowNumber2 = j + 1; s_struct[counter].gridColumnNumber2 = i;
			
			counter++;
		}
	}
	s_structSize = counter;

	//せん断ばねに接続する質点番号
	s_shear = new Spring_Shear[numGrid * numGrid * 2];
	counter = 0;
	for (int j = 0; j < numGrid - 1; j++)
	{
		for (int i = 0; i < numGrid - 1; i++)
		{
			s_shear[counter].gridRowNumber1 = j;	 s_shear[counter].gridColumnNumber1 = i;
			s_shear[counter].gridRowNumber2 = j + 1; s_shear[counter].gridColumnNumber2 = i + 1;
			
			counter++;
		}
	}
	for (int j = 0; j < numGrid - 1; j++)
	{
		for (int i = 0; i < numGrid - 1; i++)
		{
			s_shear[counter].gridRowNumber1 = j + 1; s_shear[counter].gridColumnNumber1 = i;		
			s_shear[counter].gridRowNumber2 = j + 1; s_shear[counter].gridColumnNumber2 = i;
			counter++;
		}
	}
	s_shearSize = counter;

	//ヒンジばねに接続する質点番号
	//四辺のばねの接続
	//左辺の質点を接続
	for (int j = 1; j < numGrid - 1; j++)
	{
		s_hinge[j][0].pairNum = 1;
		s_hinge[j][0].gridRowNumber[0] = j - 1;	s_hinge[j][0].gridColumnNumber[0] = 0;		
		s_hinge[j][0].gridRowNumber[1] = j + 1;	s_hinge[j][0].gridColumnNumber[1] = 0;
	}
	//下辺の質点を接続
	for (int i = 1; i < numGrid - 1; i++)
	{
		s_hinge[numGrid - 1][i].pairNum = 1;
		s_hinge[numGrid - 1][i].gridRowNumber[0] = numGrid - 1;	s_hinge[numGrid - 1][i].gridColumnNumber[0] = i - 1;
		s_hinge[numGrid - 1][i].gridRowNumber[1] = numGrid - 1;	s_hinge[numGrid - 1][i].gridColumnNumber[1] = i + 1;	
	}
	//右辺の質点を接続
	for (int j = 1; j < numGrid - 1; j++)
	{
		s_hinge[j][numGrid - 1].pairNum = 1;
		s_hinge[j][numGrid - 1].gridRowNumber[0] = j - 1; s_hinge[j][numGrid - 1].gridColumnNumber[0] = numGrid - 1;
		s_hinge[j][numGrid - 1].gridRowNumber[1] = j + 1; s_hinge[j][numGrid - 1].gridColumnNumber[1] = numGrid - 1;
	}
	//上辺の質点を接続
	for (int i = 1; i < numGrid - 1; i++)
	{
		s_hinge[0][i].pairNum = 1;
		s_hinge[0][i].gridRowNumber[0] = 0;	s_hinge[0][i].gridColumnNumber[0] = i - 1;
		s_hinge[0][i].gridRowNumber[1] = 0;	s_hinge[0][i].gridColumnNumber[1] = i + 1;
	}
	
	//四辺内のばねの接続
	//上下左右のばねの接続
	for (int j = 1; j < numGrid - 1; j++)
	{
		for (int i = 1; i < numGrid - 1; i++)
		{
			s_hinge[j][i].pairNum = 2;
			s_hinge[j][i].gridRowNumber[0] = j - 1;	s_hinge[j][i].gridColumnNumber[0] = i;
			s_hinge[j][i].gridRowNumber[1] = j + 1;	s_hinge[j][i].gridColumnNumber[1] = i;
			s_hinge[j][i].gridRowNumber[2] = j;		s_hinge[j][i].gridColumnNumber[2] = i - 1;
			s_hinge[j][i].gridRowNumber[3] = j;		s_hinge[j][i].gridColumnNumber[3] = i + 1;
		}
	}
}

//=====================================
//テクスチャのロード
//*fileName:読み込むファイル
//=====================================
void Cloth::LoadTexture(const char *fileName)
{
	texture = new DDSImage;
	texture->Load(fileName);
}

//=====================================
//力の更新
//=====================================
void Cloth::UpdateForce()
{
	//外力を求める
	for (int j = 0; j < numGrid; j++)
	{
		for (int i = 0; i < numGrid; i++)
		{
			//一番左上の点と一番左下の点は固定点のため除外
			if ((i == 0 && j == 0) || (i == 0 && j == (numGrid - 1)))
				continue;

			Grid &g = grids[j][i];
			
			//力をゼロにする
			g.force.Zero();

			//重力
			g.force.y -= g.mass * gravity;

			//風
			double r1 = counter / 75.0;
			double r2 = counter / 25.0;
			g.force.x += ((sin(r1) * sin(r2) * 0.5 + 0.5 + 0.5) * windForce);
			g.force.z += ((sin(r1) * sin(r2) * 0.5 + 0.5 + 0.5) * 0.0001);
		}
	}

	int row1, row2, col1, col2;	//行列の質点番号
	vector3 dir;	//質点#1から質点＃2へ向かう単位方向ベクトル
	vector3 dir1;	//hinge中心から質点#1へ向かう単位方向ベクトル
	vector3 dir2;	//hinge中心から質点#2へ向かう単位方向ベクトル
	vector3 vecForce;	//ばねの力
	vector3 relativeVelocity;	//相対速度
	vector3 dampingForce;	//相対速度に比例する力

	//構造ばねによる力
	for (int j = 0; j < s_structSize; j++)
	{
		//弾性力
		//s_struct[j]番目のばねに接続された質点番号
		row1 = s_struct[j].gridRowNumber1;	col1 = s_struct[j].gridColumnNumber1;
		row2 = s_struct[j].gridRowNumber2;	col2 = s_struct[j].gridColumnNumber2;

		Grid &g1 = grids[row1][col1], &g2 = grids[row2][col2];
		vector3 dirVec = g1.position - g2.position;
		dir = dirVec.UnitVector();	//#1から#2への単位ベクトル

		vecForce = dir * ((spring_length - dirVec.Length()) * k);	//正規化した方向ベクトルにばねの力を掛ける
		g1.force += vecForce;	//dirと同方向
		g2.force -= vecForce;	//反対方向

		//減衰力
		relativeVelocity = g1.velocity - g2.velocity;
		dampingForce = damping * (relativeVelocity * dir);	//相対速度に比例する減衰力
		g1.force -= dampingForce * dir;	//相対速度とは反対方向
		g2.force += dampingForce * dir;	//同方向
	}

	//せん断ばねによる力
	for (int j = 0; j < s_shearSize; j++)
	{
		//弾性力
		//s_shear[j]番目のばねに接続された質点番号
		row1 = s_shear[j].gridRowNumber1;	col1 = s_shear[j].gridColumnNumber1;
		row2 = s_shear[j].gridRowNumber2;	col2 = s_shear[j].gridColumnNumber2;

		Grid &g1 = grids[row1][col1], &g2 = grids[row2][col2];
		vector3 dirVec = g1.position - g2.position;
		dir = dirVec.UnitVector();	//#1から#2への単位ベクトル

		vecForce = dir * ((d_spring_length - dirVec.Length()) * kd);	//正規化した方向ベクトルにばねの力を掛ける
		g1.force += vecForce;	//dirと同方向
		g2.force -= vecForce;	//反対方向

		//減衰力
		relativeVelocity = g1.velocity - g2.velocity;
		dampingForce = damping * (relativeVelocity * dir);	//相対速度に比例する減衰力
		g1.force -= dampingForce * dir;	//相対速度とは反対方向
		g2.force += dampingForce * dir;	//同方向
	}

	//ヒンジ(蝶番)ばねを考慮
	for (int j = 0; j < numGrid; j++)
	{
		for (int i = 0; i < numGrid; i++)
		{
			for (int m = 0; m < s_hinge[j][i].pairNum; m++)
			{
				//四隅の質点を中心とした場合質点のペアができないため除外
				if (j == 0 && i == 0) continue;
				if (j == 0 && i == numGrid - 1) continue;
				if (j == numGrid - 1 && i == 0) continue;
				if (j == numGrid - 1 && i == numGrid - 1) continue;
				
				//質点s_hinge[j][i]を中心にペアとなる質点の番号
				row1 = s_hinge[j][i].gridRowNumber[2 * m];		col1 = s_hinge[j][i].gridColumnNumber[2 * m];
				row2 = s_hinge[j][i].gridRowNumber[2 * m + 1];	col2 = s_hinge[j][i].gridColumnNumber[2 * m + 1];

				//cg:中心グリッド、g1,g2:ペアとなるグリッド
				Grid &cg = grids[j][i], &g1 = grids[row1][col1], &g2 = grids[row2][col2];
				//hingeの中心から隣接質点へ向くベクトル方向
				vector3 dirVec1 = cg.position - g1.position, dirVec2 = cg.position - g2.position;
				dir1 = dirVec1.UnitVector();
				dir2 = dirVec2.UnitVector();

				//二つのベクトルのなす角
				double angle = GetAngle(dir1, dir2);
				//法線方向
				vector3 normal = Cross(dir1, dir2);

				//質点#1に作用する力
				dir1 = Cross(normal, dir1);
				vecForce = hingeK * (angle - PI) * dir1 / dirVec1.Length();	
				g1.force += vecForce;	//dir1と同方向
				cg.force -= vecForce;	//注目点では反対方向

				//質点#2に作用する力
				dir2 = Cross(normal, dir2);
				vecForce = hingeK * (angle - PI) * dir2 / dirVec2.Length();
				g2.force -= vecForce;	//dir2の反対方向
				cg.force += vecForce;	//注目点では反対方向
			}
		}
	}
}

//=====================================
//オイラー法で計算
//=====================================
void Cloth::EularMethod()
{
	//力の更新
	UpdateForce();

	for (int j = 0; j < numGrid; j++)
	{
		for (int i = 0; i < numGrid; i++)
		{
			//一番左上の点と一番右上の点は固定点のため除外
			if ((i == 0 && j == 0) || (i == 0 && j == (numGrid - 1)))
				continue; 
			//空気粘性抵抗（全ての質点に一様と仮定)
			grids[i][j].force -= drag * grids[i][j].velocity;
			//現在の速度を求める(力/質量 = 加速度)
			grids[j][i].velocity += (grids[j][i].force / grids[j][i].mass) * dt;
			//現在の位置を求める	
			grids[j][i].position += grids[j][i].velocity * dt;	
		}
	}
}

//=====================================
//更新
//=====================================
void Cloth::Update()
{
	EularMethod();
	counter++;
}

//=====================================
//描画
//=====================================
void Cloth::Render()
{
	GLfloat clothColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };	//ポリゴンの色

	//テクスチャを有効化
	glEnable(GL_TEXTURE_2D);	
	//テクスチャをバインド
	glBindTexture(GL_TEXTURE_2D, texture->ID);	

	glColor4fv(clothColor);
	//四点を組にして四角形を描く
	glBegin(GL_QUADS);	

	for (int j = 0; j < numGrid-1; j++)
	{
		for (int i = 0; i < numGrid-1; i++)
		{	
			//テクスチャUV座標とポリゴンの頂点を対応させる
			glTexCoord2f((float)j / (float)(numGrid - 1), -(float)i / (float)(numGrid - 1));
			glVertex3dv(grids[i][j].position.v);

			glTexCoord2f((float)(j + 1) / (float)(numGrid - 1), -(float)i / (float)(numGrid - 1));
			glVertex3dv(grids[i][j + 1].position.v);

			glTexCoord2f((float)(j + 1) / (float)(numGrid - 1), -(float)(i + 1) / (float)(numGrid - 1));
			glVertex3dv(grids[i + 1][j + 1].position.v);

			glTexCoord2f((float)j / (float)(numGrid - 1), -(float)(i + 1) / (float)(numGrid - 1));
			glVertex3dv(grids[i + 1][j].position.v);
		}
	}

	glEnd();
	
	//テクスチャ無効化
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}