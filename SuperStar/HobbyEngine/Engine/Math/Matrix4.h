#pragma once

#include "Engine/Core.h"
#include "Matrix3.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Core
{
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
            Float32 _faaMat[4][4];

            Matrix4() { *this = Matrix4::Identity; }

            /// <summary>
            /// TODO: コピーなので重い
            /// </summary>
            /// <param name="in_faaMat"></param>
            explicit Matrix4(const Float32 in_faaMat[4][4])
            {
                ::memcpy(this->_faaMat, in_faaMat, 4 * 4 * sizeof(Float32));
            }

            // Floatのポインタとして取得
            const Float32* GetAsFloatPtr() const
            {
                return reinterpret_cast<const Float32*>(&this->_faaMat[0][0]);
            }

            void Mul(const Matrix4& in_rSrcMat)
            {
                // 行列同士の計算で行側と列側でそれぞれVector4クラスに格納
                // Vector4クラス同士の掛け算で行列の各行を作成
                auto row01 = Vector4(this->_faaMat[0][0], this->_faaMat[0][1], this->_faaMat[0][2],
                                     this->_faaMat[0][3]);
                auto row02 = Vector4(this->_faaMat[1][0], this->_faaMat[1][1], this->_faaMat[1][2],
                                     this->_faaMat[1][3]);
                auto row03 = Vector4(this->_faaMat[2][0], this->_faaMat[2][1], this->_faaMat[2][2],
                                     this->_faaMat[2][3]);
                auto row04 = Vector4(this->_faaMat[3][0], this->_faaMat[3][1], this->_faaMat[3][2],
                                     this->_faaMat[3][3]);

                auto col01 = Vector4(in_rSrcMat._faaMat[0][0], in_rSrcMat._faaMat[1][0],
                                     in_rSrcMat._faaMat[2][0], in_rSrcMat._faaMat[3][0]);
                auto col02 = Vector4(in_rSrcMat._faaMat[0][1], in_rSrcMat._faaMat[1][1],
                                     in_rSrcMat._faaMat[2][1], in_rSrcMat._faaMat[3][1]);
                auto col03 = Vector4(in_rSrcMat._faaMat[0][2], in_rSrcMat._faaMat[1][2],
                                     in_rSrcMat._faaMat[2][2], in_rSrcMat._faaMat[3][2]);
                auto col04 = Vector4(in_rSrcMat._faaMat[0][3], in_rSrcMat._faaMat[1][3],
                                     in_rSrcMat._faaMat[2][3], in_rSrcMat._faaMat[3][3]);

                auto newRow01 = Vector4(row01.AddMul(col01), row01.AddMul(col02),
                                        row01.AddMul(col03), row01.AddMul(col04));
                auto newRow02 = Vector4(row02.AddMul(col01), row02.AddMul(col02),
                                        row02.AddMul(col03), row02.AddMul(col04));
                auto newRow03 = Vector4(row03.AddMul(col01), row03.AddMul(col02),
                                        row03.AddMul(col03), row03.AddMul(col04));
                auto newRow04 = Vector4(row04.AddMul(col01), row04.AddMul(col02),
                                        row04.AddMul(col03), row04.AddMul(col04));

                Float32 temp[4][4] = {
                    {newRow01.x, newRow01.y, newRow01.z, newRow01.w},
                    {newRow02.x, newRow02.y, newRow02.z, newRow02.w},
                    {newRow03.x, newRow03.y, newRow03.z, newRow03.w},
                    {newRow04.x, newRow04.y, newRow04.z, newRow04.w},
                };

                ::memcpy(this->_faaMat, temp, sizeof(Float32) * 4 * 4);
            }

            // 平行移動ベクトル取得
            void OutputTranslation(Vector3* out);

            /// <summary>
            /// 逆行列に変換したのを出力
            /// </summary>
            void OutputInvert(Matrix4* out);

            /// <summary>
            /// 行列式を取得.
            /// </summary>
            Float32 GetDet();

            /// <summary>
            /// 余因子行列を出力.
            /// </summary>
            void OutputYoinshi(Matrix4* out);

            /// <summary>
            /// 行と列を指定した3x3の余因子行列を出力.
            /// </summary>
            void OutputYoinshi3x3(Matrix3* out_pMat, Sint32 in_iRow, Sint32 in_iCol);

            /// <summary>
            /// 転置行列に変換.
            /// </summary>
            void ChangeTentsu();

            /// <summary>
            /// 各要素にスケール値を掛ける.
            /// </summary>
            void Scale(Float32);

            /// <summary>
            /// Creates the scale matrix4.
            /// </summary>
            static Matrix4 CreateScale(const Float32 in_fX, const Float32 in_fY,
                                       const Float32 in_fZ)
            {
                Float32 faaTemp[4][4] = {
                    {in_fX, 0.0f, 0.0f, 0.0f},
                    {0.0f, in_fY, 0.0f, 0.0f},
                    {0.0f, 0.0f, in_fZ, 0.0f},
                    {0.0f, 0.0f, 0.0f, 1.0f},
                };

                return Matrix4(faaTemp);
            }

            /// <summary>
            /// Creates the scale.
            /// </summary>
            static Matrix4 CreateScale(const Math::Vector3& in_rScale)
            {
                return CreateScale(in_rScale._fX, in_rScale._fY, in_rScale._fZ);
            }

            /// <summary>
            /// Creates the scale.
            /// </summary>
            static Matrix4 CreateScale(const Float32 in_fScale)
            {
                return CreateScale(in_fScale, in_fScale, in_fScale);
            }

            /// <summary>
            /// Creates the rotation z.
            /// 回転する単位はラジアンなので注意です。
            /// 1ラジアン = 2pi / 360
            /// なぜ?
            ///   2pi = 360度なのでこれを360分割すれば1ラジアンになるから
            /// </summary>
            /// <returns></returns>
            static Matrix4 CreateRotationZ(const Float32 in_fRadian);

            /// <summary>
            /// Creates the translation.
            /// </summary>
            static Matrix4 CreateTranslation(const Vector3& in_rPos);

            // 縦横のサイズを元に単位正方形の座標系に行列変換する行列作成
            static Matrix4 CreateSimpleViewProj(const Float32 in_fWidth, const Float32 in_fHeight);

            // カメラ情報からビュー行列生成
            static Matrix4 CreateLookAt(const Vector3& in_rEye, const Vector3& in_rTarget,
                                        const Vector3& in_rUp);

            // 正射影行列作成
            static Matrix4 CreateOrtho(const Float32 in_fWidth, const Float32 in_fHeight,
                                       const Float32 in_fNear, const Float32 in_fFar);

            // 透視射影行列作成
            static Matrix4 CreatePerspectiveFOV(const Float32 in_fFov, const Float32 in_fWidth,
                                                const Float32 in_fHeight, const Float32 in_fNear,
                                                const Float32 in_fFar);

            // 単位行列定義
            static const Matrix4 Identity;
        };
    }  // namespace Math

}  // namespace Core
