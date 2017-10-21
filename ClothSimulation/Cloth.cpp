//=====================================
//����ҁF���X�ؗ��M
//���t�F2016/4/13
//=====================================
//include
#include "Cloth.h"
#include "DDSLoader.h"

//global
const int numGrid = 20;			//�O���b�h��
const double gridMass = 1.0f;	//����
const double gravity = 9.8;		//�d�͉����x
const double windForce = 25.0f; //����
const double damping = 0.25;	//�_���p�萔
const double drag = 0.1;		//�S���W��
const double k = 500.0;			//�\���΂˂̒萔
const double kd = 250.0;		//����f�΂˂̒萔
const double hingeK = 0.01;		//�q���W(���Ԃ΂�)�萔
const double spring_length = 3.0;	//�\���΂˂̑傫��
const double d_spring_length = spring_length * sqrt(2.0);	//����f�΂˂̑傫��
const double dt = 0.01;			//��������
const double repForce = 0.05;	//���_���m�̐˗�
const double mu = 0.5;			//�����C�W��
const double e = 0.3;			//�����W��
const double PI = 3.14159265358979323846264338327;

//=====================================
//�R���X�g���N�^
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
//�f�X�g���N�^
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
//������
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
	
	//�΂˂ɐڑ����鍶�E�̎��_�ԍ�
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
	//�΂˂ɐڑ�����㉺�̎��_�ԍ�
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

	//����f�΂˂ɐڑ����鎿�_�ԍ�
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

	//�q���W�΂˂ɐڑ����鎿�_�ԍ�
	//�l�ӂ̂΂˂̐ڑ�
	//���ӂ̎��_��ڑ�
	for (int j = 1; j < numGrid - 1; j++)
	{
		s_hinge[j][0].pairNum = 1;
		s_hinge[j][0].gridRowNumber[0] = j - 1;	s_hinge[j][0].gridColumnNumber[0] = 0;		
		s_hinge[j][0].gridRowNumber[1] = j + 1;	s_hinge[j][0].gridColumnNumber[1] = 0;
	}
	//���ӂ̎��_��ڑ�
	for (int i = 1; i < numGrid - 1; i++)
	{
		s_hinge[numGrid - 1][i].pairNum = 1;
		s_hinge[numGrid - 1][i].gridRowNumber[0] = numGrid - 1;	s_hinge[numGrid - 1][i].gridColumnNumber[0] = i - 1;
		s_hinge[numGrid - 1][i].gridRowNumber[1] = numGrid - 1;	s_hinge[numGrid - 1][i].gridColumnNumber[1] = i + 1;	
	}
	//�E�ӂ̎��_��ڑ�
	for (int j = 1; j < numGrid - 1; j++)
	{
		s_hinge[j][numGrid - 1].pairNum = 1;
		s_hinge[j][numGrid - 1].gridRowNumber[0] = j - 1; s_hinge[j][numGrid - 1].gridColumnNumber[0] = numGrid - 1;
		s_hinge[j][numGrid - 1].gridRowNumber[1] = j + 1; s_hinge[j][numGrid - 1].gridColumnNumber[1] = numGrid - 1;
	}
	//��ӂ̎��_��ڑ�
	for (int i = 1; i < numGrid - 1; i++)
	{
		s_hinge[0][i].pairNum = 1;
		s_hinge[0][i].gridRowNumber[0] = 0;	s_hinge[0][i].gridColumnNumber[0] = i - 1;
		s_hinge[0][i].gridRowNumber[1] = 0;	s_hinge[0][i].gridColumnNumber[1] = i + 1;
	}
	
	//�l�ӓ��̂΂˂̐ڑ�
	//�㉺���E�̂΂˂̐ڑ�
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
//�e�N�X�`���̃��[�h
//*fileName:�ǂݍ��ރt�@�C��
//=====================================
void Cloth::LoadTexture(const char *fileName)
{
	texture = new DDSImage;
	texture->Load(fileName);
}

//=====================================
//�͂̍X�V
//=====================================
void Cloth::UpdateForce()
{
	//�O�͂����߂�
	for (int j = 0; j < numGrid; j++)
	{
		for (int i = 0; i < numGrid; i++)
		{
			//��ԍ���̓_�ƈ�ԍ����̓_�͌Œ�_�̂��ߏ��O
			if ((i == 0 && j == 0) || (i == 0 && j == (numGrid - 1)))
				continue;

			Grid &g = grids[j][i];
			
			//�͂��[���ɂ���
			g.force.Zero();

			//�d��
			g.force.y -= g.mass * gravity;

			//��
			double r1 = counter / 75.0;
			double r2 = counter / 25.0;
			g.force.x += ((sin(r1) * sin(r2) * 0.5 + 0.5 + 0.5) * windForce);
			g.force.z += ((sin(r1) * sin(r2) * 0.5 + 0.5 + 0.5) * 0.0001);
		}
	}

	int row1, row2, col1, col2;	//�s��̎��_�ԍ�
	vector3 dir;	//���_#1���玿�_��2�֌������P�ʕ����x�N�g��
	vector3 dir1;	//hinge���S���玿�_#1�֌������P�ʕ����x�N�g��
	vector3 dir2;	//hinge���S���玿�_#2�֌������P�ʕ����x�N�g��
	vector3 vecForce;	//�΂˂̗�
	vector3 relativeVelocity;	//���Α��x
	vector3 dampingForce;	//���Α��x�ɔ�Ⴗ���

	//�\���΂˂ɂ���
	for (int j = 0; j < s_structSize; j++)
	{
		//�e����
		//s_struct[j]�Ԗڂ̂΂˂ɐڑ����ꂽ���_�ԍ�
		row1 = s_struct[j].gridRowNumber1;	col1 = s_struct[j].gridColumnNumber1;
		row2 = s_struct[j].gridRowNumber2;	col2 = s_struct[j].gridColumnNumber2;

		Grid &g1 = grids[row1][col1], &g2 = grids[row2][col2];
		vector3 dirVec = g1.position - g2.position;
		dir = dirVec.UnitVector();	//#1����#2�ւ̒P�ʃx�N�g��

		vecForce = dir * ((spring_length - dirVec.Length()) * k);	//���K�����������x�N�g���ɂ΂˂̗͂��|����
		g1.force += vecForce;	//dir�Ɠ�����
		g2.force -= vecForce;	//���Ε���

		//������
		relativeVelocity = g1.velocity - g2.velocity;
		dampingForce = damping * (relativeVelocity * dir);	//���Α��x�ɔ�Ⴗ�錸����
		g1.force -= dampingForce * dir;	//���Α��x�Ƃ͔��Ε���
		g2.force += dampingForce * dir;	//������
	}

	//����f�΂˂ɂ���
	for (int j = 0; j < s_shearSize; j++)
	{
		//�e����
		//s_shear[j]�Ԗڂ̂΂˂ɐڑ����ꂽ���_�ԍ�
		row1 = s_shear[j].gridRowNumber1;	col1 = s_shear[j].gridColumnNumber1;
		row2 = s_shear[j].gridRowNumber2;	col2 = s_shear[j].gridColumnNumber2;

		Grid &g1 = grids[row1][col1], &g2 = grids[row2][col2];
		vector3 dirVec = g1.position - g2.position;
		dir = dirVec.UnitVector();	//#1����#2�ւ̒P�ʃx�N�g��

		vecForce = dir * ((d_spring_length - dirVec.Length()) * kd);	//���K�����������x�N�g���ɂ΂˂̗͂��|����
		g1.force += vecForce;	//dir�Ɠ�����
		g2.force -= vecForce;	//���Ε���

		//������
		relativeVelocity = g1.velocity - g2.velocity;
		dampingForce = damping * (relativeVelocity * dir);	//���Α��x�ɔ�Ⴗ�錸����
		g1.force -= dampingForce * dir;	//���Α��x�Ƃ͔��Ε���
		g2.force += dampingForce * dir;	//������
	}

	//�q���W(����)�΂˂��l��
	for (int j = 0; j < numGrid; j++)
	{
		for (int i = 0; i < numGrid; i++)
		{
			for (int m = 0; m < s_hinge[j][i].pairNum; m++)
			{
				//�l���̎��_�𒆐S�Ƃ����ꍇ���_�̃y�A���ł��Ȃ����ߏ��O
				if (j == 0 && i == 0) continue;
				if (j == 0 && i == numGrid - 1) continue;
				if (j == numGrid - 1 && i == 0) continue;
				if (j == numGrid - 1 && i == numGrid - 1) continue;
				
				//���_s_hinge[j][i]�𒆐S�Ƀy�A�ƂȂ鎿�_�̔ԍ�
				row1 = s_hinge[j][i].gridRowNumber[2 * m];		col1 = s_hinge[j][i].gridColumnNumber[2 * m];
				row2 = s_hinge[j][i].gridRowNumber[2 * m + 1];	col2 = s_hinge[j][i].gridColumnNumber[2 * m + 1];

				//cg:���S�O���b�h�Ag1,g2:�y�A�ƂȂ�O���b�h
				Grid &cg = grids[j][i], &g1 = grids[row1][col1], &g2 = grids[row2][col2];
				//hinge�̒��S����אڎ��_�֌����x�N�g������
				vector3 dirVec1 = cg.position - g1.position, dirVec2 = cg.position - g2.position;
				dir1 = dirVec1.UnitVector();
				dir2 = dirVec2.UnitVector();

				//��̃x�N�g���̂Ȃ��p
				double angle = GetAngle(dir1, dir2);
				//�@������
				vector3 normal = Cross(dir1, dir2);

				//���_#1�ɍ�p�����
				dir1 = Cross(normal, dir1);
				vecForce = hingeK * (angle - PI) * dir1 / dirVec1.Length();	
				g1.force += vecForce;	//dir1�Ɠ�����
				cg.force -= vecForce;	//���ړ_�ł͔��Ε���

				//���_#2�ɍ�p�����
				dir2 = Cross(normal, dir2);
				vecForce = hingeK * (angle - PI) * dir2 / dirVec2.Length();
				g2.force -= vecForce;	//dir2�̔��Ε���
				cg.force += vecForce;	//���ړ_�ł͔��Ε���
			}
		}
	}
}

//=====================================
//�I�C���[�@�Ōv�Z
//=====================================
void Cloth::EularMethod()
{
	//�͂̍X�V
	UpdateForce();

	for (int j = 0; j < numGrid; j++)
	{
		for (int i = 0; i < numGrid; i++)
		{
			//��ԍ���̓_�ƈ�ԉE��̓_�͌Œ�_�̂��ߏ��O
			if ((i == 0 && j == 0) || (i == 0 && j == (numGrid - 1)))
				continue; 
			//��C�S����R�i�S�Ă̎��_�Ɉ�l�Ɖ���)
			grids[i][j].force -= drag * grids[i][j].velocity;
			//���݂̑��x�����߂�(��/���� = �����x)
			grids[j][i].velocity += (grids[j][i].force / grids[j][i].mass) * dt;
			//���݂̈ʒu�����߂�	
			grids[j][i].position += grids[j][i].velocity * dt;	
		}
	}
}

//=====================================
//�X�V
//=====================================
void Cloth::Update()
{
	EularMethod();
	counter++;
}

//=====================================
//�`��
//=====================================
void Cloth::Render()
{
	GLfloat clothColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };	//�|���S���̐F

	//�e�N�X�`����L����
	glEnable(GL_TEXTURE_2D);	
	//�e�N�X�`�����o�C���h
	glBindTexture(GL_TEXTURE_2D, texture->ID);	

	glColor4fv(clothColor);
	//�l�_��g�ɂ��Ďl�p�`��`��
	glBegin(GL_QUADS);	

	for (int j = 0; j < numGrid-1; j++)
	{
		for (int i = 0; i < numGrid-1; i++)
		{	
			//�e�N�X�`��UV���W�ƃ|���S���̒��_��Ή�������
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
	
	//�e�N�X�`��������
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}