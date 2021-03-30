#ifndef __MATH_VEC3_H__
#define __MATH_VEC3_H__

#include <cmath>
#include "rtweekend.h"

namespace Math
{
	using std::sqrt;

	class Vec3
	{
	public:
		Vec3() : _e{ 0.0, 0.0, 0.0 } {}
		Vec3(double in_e0, double in_e1, double in_e2) : _e{ in_e0, in_e1, in_e2 } {}

		// ランダムの値のベクトル取得
		inline static Vec3 Random()
		{
			return Vec3(RandomDouble(), RandomDouble(), RandomDouble());
		}

		// ランダムの値のベクトル取得
		inline static Vec3 Random(const double in_min, const double in_max)
		{
			return Vec3(
				RandomDouble(in_min, in_max),
				RandomDouble(in_min, in_max),
				RandomDouble(in_min, in_max));
		}

		inline void Set(const double in_e0, const double in_e1, const double in_e2)
		{
			this->_e[0] = in_e0;
			this->_e[1] = in_e1;
			this->_e[2] = in_e2;
		}

		inline void Set(const Vec3& in_r_copy)
		{
			this->_e[0] = in_r_copy._e[0];
			this->_e[1] = in_r_copy._e[1];
			this->_e[2] = in_r_copy._e[2];
		}

		// ベクトルのxyz要素
		inline double x() const { return this->_e[0]; }
		inline double y() const { return this->_e[1]; }
		inline double z() const { return this->_e[2]; }

		// ベクトルの符号反転
		Vec3 operator-() const { return Vec3(-this->_e[0], -this->_e[1], -this->_e[2]); }

		// 要素を配列インデックスで取得
		double operator[](int i) const { return this->_e[i]; }
		double& operator[](int i) { return this->_e[i]; }

		// ベクトル同士の加算して代入
		Vec3& operator+=(const Vec3& in_v)
		{
			this->_e[0] += in_v._e[0];
			this->_e[1] += in_v._e[1];
			this->_e[2] += in_v._e[2];

			return *this;
		}

		// ベクトルのスケールして代入
		Vec3& operator*=(const double in_s)
		{
			this->_e[0] *= in_s;
			this->_e[1] *= in_s;
			this->_e[2] *= in_s;

			return *this;
		}

		// ベクトルを割り算して代入
		Vec3& operator/=(const double in_t)
		{
			return *this *= (1.0 / in_t);
		}

		/// <summary>
		/// ベクトルの長さ.
		/// </summary>
		/// <returns></returns>
		double Length() const
		{
			return sqrtl(this->LengthSquared());
		}

		/// <summary>
		/// ベクトルの長さの2乗.
		/// </summary>
		/// <returns></returns>
		double LengthSquared() const
		{
			return
				this->_e[0] * this->_e[0] +
				this->_e[1] * this->_e[1] +
				this->_e[2] * this->_e[2];
		}

	public:
		double _e[3];
	};
}

// vec3の型のエイリアス
using Point3 = Math::Vec3;
using Color = Math::Vec3;

// vec3の有用処理一覧

// ベクトル同士を加算したベクトルを出力
inline Math::Vec3 operator+(const Math::Vec3& u, const Math::Vec3& v)
{
	return Math::Vec3(u._e[0] + v._e[0], u._e[1] + v._e[1], u._e[2] + v._e[2]);
}

// ベクトル同士を減算したベクトルを出力
inline Math::Vec3 operator-(const Math::Vec3& u, const Math::Vec3& v)
{
	return Math::Vec3(u._e[0] - v._e[0], u._e[1] - v._e[1], u._e[2] - v._e[2]);
}

// ベクトル同士の掛け算したベクトルを出力
inline Math::Vec3 operator*(const Math::Vec3& u, const Math::Vec3& v)
{
	return Math::Vec3(u._e[0] * v._e[0], u._e[1] * v._e[1], u._e[2] * v._e[2]);
}

// ベクトルにスケールを与えたベクトルを出力
inline Math::Vec3 operator*(const double s, const Math::Vec3& v)
{
	return Math::Vec3(v._e[0] * s, v._e[1] * s, v._e[2] * s);
}

inline Math::Vec3 operator*(const Math::Vec3& v, const double s)
{
	return s * v;
}

// ベクトルに割り算をしたベクトルを出力
inline Math::Vec3 operator/(Math::Vec3& v, const double s)
{
	return (1.0 / s) * v;
}

inline Math::Vec3 operator/(const Math::Vec3& v, const double s)
{
	return (1.0 / s) * v;
}

// 内積計算
inline double Dot(const Math::Vec3& u, const Math::Vec3& v)
{
	return
		u._e[0] * v._e[0] +
		u._e[1] * v._e[1] +
		u._e[2] * v._e[2];
}

// 外積計算
inline Math::Vec3 Cross(const Math::Vec3& u, const Math::Vec3& v)
{
	return Math::Vec3(
		u._e[1] * v._e[2] - u._e[2] * v._e[1],
		u._e[2] * v._e[0] - u._e[0] * v._e[2],
		u._e[0] * v._e[1] - u._e[1] * v._e[0]
	);
}

// 単位ベクトル計算
inline Math::Vec3 UnitVector3(Math::Vec3& v)
{
	return v / v.Length();
}

inline Math::Vec3 UnitVector3(const Math::Vec3& v)
{
	return v / v.Length();
}

// 長さが１のベクトルのランダムを取得
inline Math::Vec3 RandomInUnitSphere()
{
	Math::Vec3 p;
	// 無限ループはきつい
	// ランダムデータを収めたテーブルに入れ替えよう
	while (true)
	{
		p.Set(Math::Vec3::Random(-1.0, 1.0));
		if (p.LengthSquared() >= 1.0)
			continue;

		return p;
	}
}

#endif // __MATH_VEC3_H__
