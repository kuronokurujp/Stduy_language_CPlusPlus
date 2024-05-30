#include "Math/matrix4.h"

#include "Math/math.h"

namespace Math
{
	// 行ベクトル基準
	// つまりDirectXで扱っている行列形式にしている
	// なのでシェーダーは左から右へ行列掛け算出来る

	// 単位行列の値
	static const float ident[4][4] =
	{
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f},
	};

	const Matrix4 Matrix4::Identity = Matrix4(ident);

	// 平行移動ベクトル取得
	void Matrix4::OutputTranslation(Vector3* out_pVec)
	{
		out_pVec->x = this->mat[3][0];
		out_pVec->y = this->mat[3][1];
		out_pVec->z = this->mat[3][2];
	}

	/// <summary>
	/// 逆行列に変換したのを出力
	/// </summary>
	void Matrix4::OutputInvert(Matrix4* out_pMat)
	{
		// 行列式を作る
		float det = this->GetDet();

		// 余因子行列を作る
		this->OutputYoinshi(out_pMat);

		// 転置行列にする
		out_pMat->ChangeTentsu();

		// それを行列式で割る
		out_pMat->Scale(1.0f / det);
	}

	/// <summary>
	/// 行列式を取得.
	/// </summary>
	/// <returns></returns>
	float Matrix4::GetDet()
	{
		//	行と列を指定した余因子行列を求める(3x3)行列
		Matrix3 m11;
		this->OutputYoinshi3x3(0, 0, &m11);

		Matrix3 m12;
		this->OutputYoinshi3x3(0, 1, &m12);

		Matrix3 m13;
		this->OutputYoinshi3x3(0, 2, &m13);

		Matrix3 m14;
		this->OutputYoinshi3x3(0, 3, &m14);

		//  余因子行列から行列式を求める
		float a = this->mat[0][0] * m11.GetDet();
		float b = this->mat[0][1] * m12.GetDet();
		float c = this->mat[0][2] * m13.GetDet();
		float d = this->mat[0][3] * m14.GetDet();

		return a - b + c - d;
	}

	/// <summary>
	/// 余因子行列を出力.
	/// </summary>
	/// <param name="out_pMat">The out p mat.</param>
	void Matrix4::OutputYoinshi(Matrix4* out_pMat)
	{
		Matrix3 tmpMat3;
		float s = 1;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				this->OutputYoinshi3x3(i, j, &tmpMat3);
				out_pMat->mat[i][j] = s * tmpMat3.GetDet();
				s *= -1;
			}
		}
	}

	/// <summary>
	/// 行と列を指定した3x3の余因子行列を出力.
	/// </summary>
	/// <param name="in_row">The in row.</param>
	/// <param name="in_col">The in col.</param>
	/// <param name="out_pMat">The out p mat.</param>
	void Matrix4::OutputYoinshi3x3(int in_row, int in_col, Matrix3* out_pMat)
	{
		int row = 0;
		for (int i = 0; i < 4; ++i)
		{
			if (in_row == i)
			{
				continue;
			}

			int col = 0;
			for (int j = 0; j < 4; ++j)
			{
				if (in_col == j)
				{
					continue;
				}

				out_pMat->mat[row][col] = this->mat[i][j];
				++col;
			}

			++row;
		}
	}

	/// <summary>
	/// 転置行列に変換.
	/// </summary>
	void Matrix4::ChangeTentsu()
	{
		Matrix4 tmp;
		memcpy(&tmp, &this->mat[0][0], sizeof(float) * 4 * 4);

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				this->mat[i][j] = tmp.mat[j][i];
			}
		}
	}

	/// <summary>
	/// 各要素にスケール値を掛ける.
	/// </summary>
	/// <param name="in_scaler">The in scaler.</param>
	void Matrix4::Scale(float in_scale)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				this->mat[i][j] *= in_scale;
			}
		}
	}

	/// <summary>
	/// Creates the rotation z.
	/// 回転する単位はラジアンなので注意です。
	/// 1ラジアン = 2pi / 360
	/// なぜ?
	///   2pi = 360度なのでこれを360分割すれば1ラジアンになるから
	/// </summary>
	/// <returns></returns>
	Matrix4 Matrix4::CreateRotationZ(const float in_radian)
	{
		// cos(a) * x - sin(a) * y
		// sin(a) * x + cos(a) * y
		float temp[4][4] =
		{
			{ Cos(in_radian), Sin(in_radian), 0.0f, 0.0f },
			{ -Sin(in_radian), Cos(in_radian), 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};

		return Matrix4(temp);
	}

	/// <summary>
	/// Creates the translation.
	/// </summary>
	/// <param name="in_rPos">The in r position.</param>
	/// <returns></returns>
	Matrix4 Matrix4::CreateTranslation(const Vector3& in_rPos)
	{
		float temp[4][4] =
		{
			{ 1.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ in_rPos.x, in_rPos.y, in_rPos.z, 1.0f },
		};

		return Matrix4(temp);
	}

	// 縦横のサイズを元に単位正方形の座標系に行列変換する行列作成
	Matrix4 Matrix4::CreateSimpleViewProj(const float in_width, const float in_height)
	{
		float temp[4][4] =
		{
			{ 2.0f / in_width, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 2.0f / in_height, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 1.0f },
		};

		return Matrix4(temp);
	}

	// カメラ情報からビュー行列生成
	Matrix4 Matrix4::CreateLookAt(const Vector3& in_rEye, const Vector3& in_rTarget, const Vector3& in_rUp)
	{
		Vector3 zAxis;
		zAxis.SetSub(in_rTarget, in_rEye);
		zAxis.Normalize();

		Vector3 xAxis = Vector3::Cross(in_rUp, zAxis);
		xAxis.Normalize();

		Vector3 yAxis = Vector3::Cross(zAxis, xAxis);
		yAxis.Normalize();

		Vector3 trans;
		trans.x = -Vector3::Dot(xAxis, in_rEye);
		trans.y = -Vector3::Dot(yAxis, in_rEye);
		trans.z = -Vector3::Dot(zAxis, in_rEye);

		float temp[4][4] =
		{
			{xAxis.x, yAxis.x, zAxis.x, 0.0f},
			{xAxis.y, yAxis.y, zAxis.y, 0.0f},
			{xAxis.z, yAxis.z, zAxis.z, 0.0f},
			{trans.x, trans.y, trans.z, 1.0f},
		};

		return Matrix4(temp);
	}

	// 正射影行列作成
	Matrix4 Matrix4::CreateOrtho(const float in_width, const float in_height, const float in_near, const float in_far)
	{
		float temp[4][4] =
		{
			{2.0f / in_width, 0.0f,             0.0f,                         0.0f},
			{0.0f,            2.0f / in_height, 0.0f,                         0.0f},
			{0.0f,            0.0f,             1.0f / (in_far - in_near),    0.0f},
			{0.0f,            0.0f,             in_near / (in_near - in_far), 1.0f},
		};

		return Matrix4(temp);
	}

	// 透視射影行列作成
	Matrix4 Matrix4::CreatePerspectiveFOV(const float in_fov, const float in_width, const float in_height, const float in_near, const float in_far)
	{
		float yScale = Math::Cot(in_fov / 2.0f);
		float xScale = yScale * in_height / in_width;

		float temp[4][4] =
		{
			{xScale, 0.0f,   0.0f,                                     0.0f},
			{0.0f,   yScale, 0.0f,                                     0.0f},
			{0.0f,   0.0f,   in_far / (in_far - in_near),              1.0f},
			{0.0f,   0.0f,   -in_near * in_far / (in_far - in_near),   0.0f},
		};

		return Matrix4(temp);
	}
}