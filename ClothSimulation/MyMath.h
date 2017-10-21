//=====================================
//����ҁF���X�ؗ��M
//���t�F2016/3/25
//=====================================
#ifndef _MYMATH_H_
#define _MYMATH_H_
#include <iostream>

const double pp = 3.14159265358979323846264338327 / 180.0;

template <class T> class Vector3;

typedef Vector3<double>			vector3;
typedef Vector3<double>			vector3d;
typedef Vector3<float>			vector3f;
typedef Vector3<int>			vector3i;

template <class T>
class Vector3
{
public:
	//�����������L
	union{ struct{ T x, y, z; }; T v[3]; };

	Vector3<T>(T v_x = 0.0, T v_y = 0.0, T v_z = 0.0) : x(v_x), y(v_y), z(v_z){}
	Vector3<T>(const T *v) : x(v[0]), y(v[1]), z(v[2]) {}

	//�[���ɏ�����
	void Zero()
	{
		x = 0.0, y = 0.0, z = 0.0;
	}

	//�P�ʃx�N�g��
	const Vector3<T> UnitVector() const
	{
		if (Length() != 0.0)
			return (*this) / Length(); 
		else 
			return Vector3<T>(0.0, 0.0, 0.0);
	}

	//�x�N�g���̑傫��
	const T Length() const
	{
		return (T)sqrt(x*x + y*y + z*z);
	}

	void rotX(float angle)
	{
		float xx, yy, zz;

		xx = x; yy = y; zz = z;
		x = xx;
		y = yy * (float)cos(pp * angle) - zz * (float)sin(pp * angle);
		z = yy * (float)sin(pp * angle) + zz * (float)cos(pp * angle);
	}

	void rotY(float angle)
	{
		float xx, yy, zz;

		xx = x; yy = y; zz = z;
		x = xx * (float)cos(pp * angle) + zz * (float)sin(pp * angle);
		y = yy;
		z = -xx * (float)sin(pp * angle) + zz * (float)cos(pp * angle);
	}

	void rotZ(float angle)
	{
		float xx, yy, zz;

		xx = x; yy = y; zz = z;
		x = xx * (float)cos(pp * angle) - yy * (float)sin(pp * angle);
		y = xx * (float)sin(pp * angle) + yy * (float)cos(pp * angle);
		z = zz;
	}

	//=====================================
	//Vector3���m�̉��Z
	//=====================================
	const Vector3<T> operator + (const Vector3<T>& vec3) const
	{
		return Vector3<T>(x + vec3.x, y + vec3.y, z + vec3.z);
	}

	const Vector3<T>& operator += (const Vector3<T>& vec3)
	{
		x += vec3.x, y += vec3.y, z += vec3.z;
		return *this;
	}

	const Vector3<T> operator - (const Vector3<T>& vec3) const
	{
		return Vector3<T>(x - vec3.x, y - vec3.y, z - vec3.z);
	}

	const Vector3<T>& operator -= (const Vector3<T>& vec3)
	{
		x -= vec3.x, y -= vec3.y, z -= vec3.z;
		return *this;
	}

	const Vector3<T> operator * (const Vector3<T>& vec3) const
	{
		return Vector3<T> (x * vec3.x, y * vec3.y, z * vec3.z);
	}

	const Vector3<T>& operator *= (const Vector3<T>& vec3)
	{
		x *= vec3.x, y *= vec3.y, z *= vec3.z;
		return *this;
	}

	const int operator == (const Vector3<T>& vec3) const
	{
		return x == vec3.x && y == vec3.y && z == vec3.z;
	}

	const int operator != (const Vector3<T>& vec3) const
	{
		return x != vec3.x && y != vec3.y && z != vec3.z;
	}

	const Vector3<T>& operator = (const Vector3<T>& vec3)
	{
		x = vec3.x, y = vec3.y, z = vec3.z;
		return *this;
	}

	//=====================================
	//Vector3�Ƒg�ݍ��݌^�̉��Z
	//=====================================
	const Vector3<T> operator * (const T n) const
	{
		return Vector3<T>(n * x, n * y, n * z);
	}

	//�t�����h�̓N���X�����o�ł͂Ȃ�����const�֐��Ƃ��Ē�`�ł��Ȃ�
	friend const Vector3<T> operator * (const T n, const Vector3<T> &vec3) 
	{
		return vec3 * n;
	}

	const Vector3<T>& operator *= (const T n)
	{
		x *= n, y *= n, z *= n;
		return *this;
	}

	const Vector3<T> operator / (const T n) const
	{
		const T recip = 1 / n; 
		return Vector3<T>(recip*x, recip*y, recip*z);
	}

	const Vector3<T>& operator /= (const T n)
	{
		const T recip = 1 / n; x *= recip; y *= recip; z *= recip; 
		return *this;
	}
};

//�x�N�g������
template <class T> 
const T Dot(const Vector3<T>& vecA, const Vector3<T>& vecB)  
{
	return (vecA.x * vecB.x + vecA.y * vecB.y + vecA.z * vecB.z);
};

//�x�N�g���O��
template <class T> 
const Vector3<T> Cross(const Vector3<T>& vecA, const Vector3<T>& vecB)  
{
	T x = vecA.y * vecB.z - vecA.z * vecB.y;
	T y = vecA.z * vecB.x - vecA.x * vecB.z;
	T z = vecA.x * vecB.y - vecA.y * vecB.x;
	return Vector3<T>(x, y, z);
};

//�Q�̃x�N�g���̂Ȃ��p�x�Ƃ����߂�
template <class T> 
const T GetAngle(const Vector3<T>& vecA, const Vector3<T>& vecB) 
{
	double PI = 3.14159265358979323846264338327;
	//�x�N�g��A��B�̒������v�Z����
	double length_A = vecA.Length();
	double length_B = vecB.Length();

	//���ςƃx�N�g���������g����cos�Ƃ����߂�
	double cos_sita = Dot(vecA, vecB) / (length_A * length_B);
	
	double sita;
	//cos�Ƃ���Ƃ����߂�
	if (cos_sita >= 1.0) sita = 0.0;
	else if (cos_sita <= -1.0) sita = PI;
	else sita = acos(cos_sita);

	//���W�A���łȂ�0�`180�̊p�x�łق����ꍇ�̓R�����g�O��
	//sita = sita * 180.0 / PI;

	return sita;
};

template <class T>
const Vector3<T>& rotate(Vector3<T>& v, const Vector3<T>& elr)
{
	v.rotX(elr.x);
	v.rotY(elr.y);
	v.rotZ(elr.z);
	return v;
};

#endif // _MYMATH_H_