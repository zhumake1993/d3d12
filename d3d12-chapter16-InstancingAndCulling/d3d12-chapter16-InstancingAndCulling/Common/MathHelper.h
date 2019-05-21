#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <cstdint>

class MathHelper
{
public:
	//返回[0, 1)内的随机浮点数
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	//返回[a, b)内的随机浮点数
	static float RandF(float a, float b)
	{
		return a + RandF() * (b - a);
	}

	//返回[a, b]内的随机整数
	static int Rand(int a, int b)
	{
		return a + rand() % ((b - a) + 1);
	}

	template<typename T>
	static T Min(const T & a, const T & b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T & a, const T & b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	static T Lerp(const T & a, const T & b, float t)
	{
		return a + (b - a) * t;
	}

	template<typename T>
	static T Clamp(const T & x, const T & low, const T & high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	//计算点(x,y)对应的极坐标的极角，范围[0, 2*PI)
	static float AngleFromXY(float x, float y);

	//将球坐标转换为笛卡尔坐标
	//向量与正y轴之间的夹角为phi
	//向量在xz平面的投影线与正x轴之间的夹角为theta
	static DirectX::XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
	{
		return DirectX::XMVectorSet(
			radius * sinf(phi) * cosf(theta),
			radius * cosf(phi),
			radius * sinf(phi) * sinf(theta),
			1.0f);
	}

	//计算逆转置矩阵
	static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		DirectX::XMMATRIX A = M;
		A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
		return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}

	static DirectX::XMFLOAT4X4 Identity4x4()
	{
		static DirectX::XMFLOAT4X4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return I;
	}

	static DirectX::XMVECTOR RandUnitVec3();
	static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);

	static const float Infinity;
	static const float Pi;


};