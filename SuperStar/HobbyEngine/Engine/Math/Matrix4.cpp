#include "Matrix4.h"

#include "Math.h"

namespace Core::Math
{
    // 行ベクトル基準
    // つまりDirectXで扱っている行列形式にしている
    // なのでシェーダーは左から右へ行列掛け算出来る

    // 単位行列の値
    static const Float32 faaIdent[4][4] = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    };

    const Matrix4 Matrix4::Identity = Matrix4(faaIdent);

    // 平行移動ベクトル取得
    void Matrix4::OutputTranslation(Vector3* out)
    {
        out->_fX = this->_faaMat[3][0];
        out->_fY = this->_faaMat[3][1];
        out->_fZ = this->_faaMat[3][2];
    }

    /// <summary>
    /// 逆行列に変換したのを出力
    /// </summary>
    void Matrix4::OutputInvert(Matrix4* out)
    {
        // 行列式を作る
        const Float32 fDet = this->GetDet();
        if (fDet == 0.0) return;

        // 余因子行列を作る
        this->OutputYoinshi(out);

        // 転置行列にする
        out->ChangeTentsu();

        // それを行列式で割る
        out->Scale(1.0f / fDet);
    }

    /// <summary>
    /// 行列式を取得.
    /// </summary>
    /// <returns></returns>
    const Float32 Matrix4::GetDet()
    {
        //	行と列を指定した余因子行列を求める(3x3)行列
        Matrix3 m11;
        this->OutputYoinshi3x3(&m11, 0, 0);

        Matrix3 m12;
        this->OutputYoinshi3x3(&m12, 0, 1);

        Matrix3 m13;
        this->OutputYoinshi3x3(&m13, 0, 2);

        Matrix3 m14;
        this->OutputYoinshi3x3(&m14, 0, 3);

        //  余因子行列から行列式を求める
        const Float32 a = this->_faaMat[0][0] * m11.GetDet();
        const Float32 b = this->_faaMat[0][1] * m12.GetDet();
        const Float32 c = this->_faaMat[0][2] * m13.GetDet();
        const Float32 d = this->_faaMat[0][3] * m14.GetDet();

        return (a - b + c - d);
    }

    /// <summary>
    /// 余因子行列を出力.
    /// </summary>
    /// <param name="out_pMat">The out p mat.</param>
    void Matrix4::OutputYoinshi(Matrix4* out)
    {
        Matrix3 tmpMat3;
        Float32 s = 1;
        for (Uint32 i = 0; i < 4; ++i)
        {
            for (Uint32 j = 0; j < 4; ++j)
            {
                this->OutputYoinshi3x3(&tmpMat3, i, j);
                out->_faaMat[i][j] = s * tmpMat3.GetDet();
                s *= -1;
            }
        }
    }

    /// <summary>
    /// 行と列を指定した3x3の余因子行列を出力.
    /// </summary>
    void Matrix4::OutputYoinshi3x3(Matrix3* out, Sint32 in_iRow, Sint32 in_iCol)
    {
        Sint32 row = 0;
        for (Sint32 i = 0; i < 4; ++i)
        {
            if (in_iRow == i) continue;

            Sint32 col = 0;
            for (Sint32 j = 0; j < 4; ++j)
            {
                if (in_iCol == j) continue;

                out->_faaMat[row][col] = this->_faaMat[i][j];
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
        ::memcpy(&tmp, &this->_faaMat[0][0], sizeof(Float32) * 4 * 4);

        for (Uint32 i = 0; i < 4; ++i)
        {
            for (Uint32 j = 0; j < 4; ++j)
            {
                this->_faaMat[i][j] = tmp._faaMat[j][i];
            }
        }
    }

    /// <summary>
    /// 各要素にスケール値を掛ける.
    /// </summary>
    void Matrix4::Scale(const Float32 in_fScale)
    {
        for (Uint32 i = 0; i < 4; ++i)
        {
            for (Uint32 j = 0; j < 4; ++j)
            {
                this->_faaMat[i][j] *= in_fScale;
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
    Matrix4 Matrix4::CreateRotationZ(const Float32 in_fRadian)
    {
        // cos(a) * x - sin(a) * y
        // sin(a) * x + cos(a) * y
        Float32 faaTtemp[4][4] = {
            {Cos(in_fRadian), Sin(in_fRadian), 0.0f, 0.0f},
            {-Sin(in_fRadian), Cos(in_fRadian), 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
        };

        return Matrix4(faaTtemp);
    }

    /// <summary>
    /// Creates the translation.
    /// </summary>
    /// <param name="in_rPos">The in r position.</param>
    /// <returns></returns>
    Matrix4 Matrix4::CreateTranslation(const Vector3& in_rPos)
    {
        Float32 faaTemp[4][4] = {
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {in_rPos._fX, in_rPos._fY, in_rPos._fZ, 1.0f},
        };

        return Matrix4(faaTemp);
    }

    // 縦横のサイズを元に単位正方形の座標系に行列変換する行列作成
    Matrix4 Matrix4::CreateSimpleViewProj(const Float32 in_fWidth, const Float32 in_fHeight)
    {
        Float32 faaTemp[4][4] = {
            {2.0f / in_fWidth, 0.0f, 0.0f, 0.0f},
            {0.0f, 2.0f / in_fHeight, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 1.0f},
        };

        return Matrix4(faaTemp);
    }

    // カメラ情報からビュー行列生成
    Matrix4 Matrix4::CreateLookAt(const Vector3& in_rEye, const Vector3& in_rTarget,
                                  const Vector3& in_rUp)
    {
        Vector3 zAxis;
        zAxis.SetSub(in_rTarget, in_rEye);
        zAxis.Normalize();

        Vector3 xAxis = Vector3::Cross(in_rUp, zAxis);
        xAxis.Normalize();

        Vector3 yAxis = Vector3::Cross(zAxis, xAxis);
        yAxis.Normalize();

        Vector3 trans;
        trans._fX = -Vector3::Dot(xAxis, in_rEye);
        trans._fY = -Vector3::Dot(yAxis, in_rEye);
        trans._fZ = -Vector3::Dot(zAxis, in_rEye);

        Float32 faaTemp[4][4] = {
            {xAxis._fX, yAxis._fX, zAxis._fX, 0.0f},
            {xAxis._fY, yAxis._fY, zAxis._fY, 0.0f},
            {xAxis._fZ, yAxis._fZ, zAxis._fZ, 0.0f},
            {trans._fX, trans._fY, trans._fZ, 1.0f},
        };

        return Matrix4(faaTemp);
    }

    // 正射影行列作成
    Matrix4 Matrix4::CreateOrtho(const Float32 in_fWidth, const Float32 in_fHeight,
                                 const Float32 in_fNear, const Float32 in_fFar)
    {
        const Float32 faaTemp[4][4] = {
            {2.0f / in_fWidth, 0.0f, 0.0f, 0.0f},
            {0.0f, 2.0f / in_fHeight, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f / (in_fFar - in_fNear), 0.0f},
            {0.0f, 0.0f, in_fNear / (in_fNear - in_fFar), 1.0f},
        };

        return Matrix4(faaTemp);
    }

    // 透視射影行列作成
    Matrix4 Matrix4::CreatePerspectiveFOV(const Float32 in_fFov, const Float32 in_fWidth,
                                          const Float32 in_fHeight, const Float32 in_fNear,
                                          const Float32 in_fFar)
    {
        HE_ASSERT(in_fWidth != 0.0f);
        HE_ASSERT(in_fHeight != 0.0f);
        HE_ASSERT(in_fFov != 0.0f);

        const Float32 yScale = Math::Cot(in_fFov / 2.0f);
        const Float32 xScale = yScale * in_fHeight / in_fWidth;

        const Float32 temp[4][4] = {
            {xScale, 0.0f, 0.0f, 0.0f},
            {0.0f, yScale, 0.0f, 0.0f},
            {0.0f, 0.0f, in_fFar / (in_fFar - in_fNear), 1.0f},
            {0.0f, 0.0f, -in_fNear * in_fFar / (in_fFar - in_fNear), 0.0f},
        };

        return Matrix4(temp);
    }
}  // namespace Core::Math
