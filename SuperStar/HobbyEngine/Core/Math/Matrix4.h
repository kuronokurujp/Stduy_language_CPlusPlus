#ifndef __MATH_MATRIX4_H__
#define __MATH_MATRIX4_H__

#include <string.h>

#include "Math/vector3.h"
#include "Math/vector4.h"

#include "Math/matrix3.h"

namespace Math
{
	/// <summary>
	/// 4x4行列のクラス
	/// 行ベクトル前提で各計算処理をしている
	/// 列ベクトルにするで扱いたい場合は転置する必要がある
	/// クラスだが扱いは構造体とするので仮想関数の定義はしてはいけない
	/// </summary>
	class Matrix4
	{
	public:
		float mat[4][4];

		Matrix4()
		{
			*this = Matrix4::Identity;
		}

		explicit Matrix4(const float in_mat[4][4])
		{
			memcpy(this->mat, in_mat, 4 * 4 * sizeof(float));
		}

		// Floatのポインタとして取得
		const float* GetAsFloatPtr() const
		{
			return reinterpret_cast<const float*>(&this->mat[0][0]);
		}

		void Mul(const Matrix4& in_rSrcMat)
		{
			// 行列同士の計算で行側と列側でそれぞれVector4クラスに格納
			// Vector4クラス同士の掛け算で行列の各行を作成
			auto row01 = Vector4(this->mat[0][0], this->mat[0][1], this->mat[0][2], this->mat[0][3]);
			auto row02 = Vector4(this->mat[1][0], this->mat[1][1], this->mat[1][2], this->mat[1][3]);
			auto row03 = Vector4(this->mat[2][0], this->mat[2][1], this->mat[2][2], this->mat[2][3]);
			auto row04 = Vector4(this->mat[3][0], this->mat[3][1], this->mat[3][2], this->mat[3][3]);

			auto col01 = Vector4(in_rSrcMat.mat[0][0], in_rSrcMat.mat[1][0], in_rSrcMat.mat[2][0], in_rSrcMat.mat[3][0]);
			auto col02 = Vector4(in_rSrcMat.mat[0][1], in_rSrcMat.mat[1][1], in_rSrcMat.mat[2][1], in_rSrcMat.mat[3][1]);
			auto col03 = Vector4(in_rSrcMat.mat[0][2], in_rSrcMat.mat[1][2], in_rSrcMat.mat[2][2], in_rSrcMat.mat[3][2]);
			auto col04 = Vector4(in_rSrcMat.mat[0][3], in_rSrcMat.mat[1][3], in_rSrcMat.mat[2][3], in_rSrcMat.mat[3][3]);

			auto newRow01 = Vector4(row01.AddMul(col01), row01.AddMul(col02), row01.AddMul(col03), row01.AddMul(col04));
			auto newRow02 = Vector4(row02.AddMul(col01), row02.AddMul(col02), row02.AddMul(col03), row02.AddMul(col04));
			auto newRow03 = Vector4(row03.AddMul(col01), row03.AddMul(col02), row03.AddMul(col03), row03.AddMul(col04));
			auto newRow04 = Vector4(row04.AddMul(col01), row04.AddMul(col02), row04.AddMul(col03), row04.AddMul(col04));

			float temp[4][4] =
			{
				{ newRow01.x, newRow01.y, newRow01.z, newRow01.w },
				{ newRow02.x, newRow02.y, newRow02.z, newRow02.w },
				{ newRow03.x, newRow03.y, newRow03.z, newRow03.w },
				{ newRow04.x, newRow04.y, newRow04.z, newRow04.w },
			};

			memcpy(this->mat, temp, sizeof(float) * 4 * 4);
		}

		// 平行移動ベクトル取得
		void OutputTranslation(Vector3* out_pVec);

		/// <summary>
		/// 逆行列に変換したのを出力
		/// </summary>
		void OutputInvert(Matrix4* out_pMat);

		/// <summary>
		/// 行列式を取得.
		/// </summary>
		/// <returns></returns>
		float GetDet();

		/// <summary>
		/// 余因子行列を出力.
		/// </summary>
		/// <param name="out_pMat">The out p mat.</param>
		void OutputYoinshi(Matrix4* out_pMat);

		/// <summary>
		/// 行と列を指定した3x3の余因子行列を出力.
		/// </summary>
		/// <param name="in_row">The in row.</param>
		/// <param name="in_col">The in col.</param>
		/// <param name="out_pMat">The out p mat.</param>
		void OutputYoinshi3x3(int in_row, int in_col, Matrix3* out_pMat);

		/// <summary>
		/// 転置行列に変換.
		/// </summary>
		void ChangeTentsu();

		/// <summary>
		/// 各要素にスケール値を掛ける.
		/// </summary>
		/// <param name="in_scaler">The in scaler.</param>
		void Scale(float);

		/// <summary>
		/// Creates the scale matrix4.
		/// </summary>
		/// <param name="in_x">The in x.</param>
		/// <param name="in_y">The in y.</param>
		/// <param name="in_z">The in z.</param>
		/// <returns></returns>
		static Matrix4 CreateScale(
			const float in_x,
			const float in_y,
			const float in_z)
		{
			float temp[4][4] =
			{
				{ in_x, 0.0f, 0.0f, 0.0f },
				{ 0.0f, in_y, 0.0f, 0.0f },
				{ 0.0f, 0.0f, in_z, 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f },
			};

			return Matrix4(temp);
		}

		/// <summary>
		/// Creates the scale.
		/// </summary>
		/// <param name="in_rScale">The in r scale.</param>
		/// <returns></returns>
		static Matrix4 CreateScale(const Math::Vector3& in_rScale)
		{
			return CreateScale(in_rScale.x, in_rScale.y, in_rScale.z);
		}

		/// <summary>
		/// Creates the scale.
		/// </summary>
		/// <param name="in_scale">The in scale.</param>
		/// <returns></returns>
		static Matrix4 CreateScale(const float in_scale)
		{
			return CreateScale(in_scale, in_scale, in_scale);
		}

		/// <summary>
		/// Creates the rotation z.
		/// 回転する単位はラジアンなので注意です。
		/// 1ラジアン = 2pi / 360
		/// なぜ?
		///   2pi = 360度なのでこれを360分割すれば1ラジアンになるから
		/// </summary>
		/// <returns></returns>
		static Matrix4 CreateRotationZ(const float in_radian);

		/// <summary>
		/// Creates the translation.
		/// </summary>
		/// <param name="in_rPos">The in r position.</param>
		/// <returns></returns>
		static Matrix4 CreateTranslation(const Vector3& in_rPos);

		// 縦横のサイズを元に単位正方形の座標系に行列変換する行列作成
		static Matrix4 CreateSimpleViewProj(const float in_width, const float in_height);

		// カメラ情報からビュー行列生成
		static Matrix4 CreateLookAt(const Vector3& in_rEye, const Vector3& in_rTarget, const Vector3& in_rUp);

		// 正射影行列作成
		static Matrix4 CreateOrtho(const float in_width, const float in_height, const float in_near, const float in_far);

		// 透視射影行列作成
		static Matrix4 CreatePerspectiveFOV(const float in_fov, const float in_width, const float in_height, const float in_near, const float in_far);

		// 単位行列定義
		static const Matrix4 Identity;
	};
}

#endif // __MATH_MATRIX4_H__
