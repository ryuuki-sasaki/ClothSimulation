//=====================================
//����ҁF���X�ؗ��M
//���t�F2016/4/13
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

//�O���錾
class DDSImage;

struct Grid
{
	double mass;			//����
	vector3 position;		//�ʒu
	vector3 force;			//��
	vector3 velocity;		//���x
	
	Grid()
	{
		position.Zero();
		force.Zero();
		velocity.Zero();
	}
};

//�㉺���E�ɐL�т�΂˂����Ԏ��_�̔ԍ�
struct Spring_Struct
{
	//���_�ԍ�
	int gridColumnNumber1 = 0;	//��ԍ�
	int gridColumnNumber2 = 0;
	int gridRowNumber1 = 0;		//�s�ԍ�
	int gridRowNumber2 = 0;
};

//�΂߂ɐL�т�΂�(����f�΂�)�����Ԏ��_�̔ԍ�
struct Spring_Shear
{
	//���_�ԍ�
	int gridColumnNumber1 = 0;	//��ԍ�
	int gridColumnNumber2 = 0;
	int gridRowNumber1 = 0;		//�s�ԍ�
	int gridRowNumber2 = 0;
};

//���Ԃ΂˂����Ԏ��_�̔ԍ�
struct Spring_Hinge
{
	//���_�ԍ�
	int gridColumnNumber[4];	//��ԍ�
	int gridRowNumber[4];		//�s�ԍ�

	int pairNum = 0;	//�ڑ����_�̃y�A��
};

//�񎟌��z��ň�����������vector
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