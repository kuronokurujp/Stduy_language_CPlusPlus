#pragma once

#include "Core.h"
#include "Vector2.h"

namespace Core
{
    namespace Math
    {
        /// <summary>
        /// ベクトルクラス.
        /// このクラスはメモリを直接参照するので仮想関数を作ってはだめ.
        /// </summary>
        class Vector3
        {
        public:
            explicit Vector3() : _fX(0.0f), _fY(0.0f), _fZ(0.0f) {}
            explicit Vector3(Float32 in_fX, Float32 in_fY, Float32 in_fZ)
                : _fX(in_fX), _fY(in_fY), _fZ(in_fZ)
            {
            }
            explicit Vector3(const Vector2& in_rVec2)
                : _fX(in_rVec2._fX), _fY(in_rVec2._fY), _fZ(0.0f)
            {
            }

            Vector3(Vector3& in_rVec3) : _fX(in_rVec3._fX), _fY(in_rVec3._fY), _fZ(in_rVec3._fZ) {}

            Vector3(const Vector3& in_rVec3)
                : _fX(in_rVec3._fX), _fY(in_rVec3._fY), _fZ(in_rVec3._fZ)
            {
            }

            // 整数設定
            explicit Vector3(Sint32 in_iX, Sint32 in_iY, Sint32 in_iZ);

            // 情報をクリア
            inline void Clear() { this->_fX = this->_fY = this->_fZ = 0.0f; }

            // 値設定
            void Set(const Float32 in_fX, const Float32 in_fY, const Float32 in_fZ);
            void Set(const Float32 in_val);

            // 掛け算
            void Mul(const Float32 b);

            // 積和
            void Madd(const Vector3& a, const Float32 b);

            // 積和設定
            void SetMadd(const Vector3& a, const Float32 b, const Vector3& c);

            // 加算
            void SetAdd(const Vector3& a, const Vector3& b);

            // 減算
            void SetSub(const Vector3& a, const Vector3& b);

            // 積算
            void SetMul(const Vector3& a, const Vector3& b);

            // ２次元補間を行う
            void SetInterporation(const Vector3& a, const Vector3& ab, const Vector3& ac, Float32 u,
                                  Float32 v);

            //	正規化
            void Normalize();

            // 演算定義
            void operator+=(const Vector3& a);  // 足し算
            void operator-=(const Vector3& a);  // 引き算
            void operator=(const Vector3& a);   // コピー
            void operator*=(const Vector3& a);  // 掛け算
            void operator/=(const Vector3& a);  // 割り算

            /// <summary>
            /// Operator*s the specified scale.
            /// オペレーターで戻り値はconstにしない方が良い
            /// 戻り値はどのようにアクセスするのか予測が出来ない
            /// constを入れるのか入れないのか、汎用として扱えるようにするためには制約は付けてはいけない
            /// </summary>
            /// <param name="scale">The scale.</param>
            /// <returns></returns>
            Vector3 operator*(const Float32 scale) HE_NOEXCEPT
            {
                return Vector3(this->_fX * scale, this->_fY * scale, this->_fZ * scale);
            }

            /// <summary>
            /// Operator+s the specified in a.
            /// </summary>
            /// <param name="in_a">The in a.</param>
            /// <returns></returns>
            Vector3 operator+(const Vector3 in_a) HE_NOEXCEPT
            {
                return Vector3(this->_fX + in_a._fX, this->_fY + in_a._fY, this->_fZ + in_a._fZ);
            }

            /// <summary>
            /// ベクトル要素をfloatポインタで返す.
            /// </summary>
            /// <returns></returns>
            const Float32* GetAsFloatPtr() HE_NOEXCEPT
            {
                return reinterpret_cast<Float32*>(&this->_fX);
            }

            // 頻繁にアクセスする変数にはprivate指定にはしない
            Float32 _fX = 0.0f;
            Float32 _fY = 0.0f;
            Float32 _fZ = 0.0f;

            // 各要素を1にしてそれ以外の要素を0にしたベクトル
            static Vector3 UnitX;
            static Vector3 UnitY;
            static Vector3 UnitZ;
            static Vector3 Zero;
            static Vector3 One;

            // 内積
            static float Dot(const Vector3& in_rV, const Vector3& in_rV2);

            // 外積
            static Vector3 Cross(const Vector3& in_rV, const Vector3& in_rV2);

            // ベクトルの大きさを2乗したのを取得
            // こちらの方が計算が早い
            // 比較などで利用できる
            static float GetLengthSquared(const Vector3& in_rV);

            // 大きさ取得
            static float GetVector3Mag(const Vector3& in_v);

            // 距離取得
            static float GetVector3Distance(const Vector3& in_v, const Vector3& in_v2);
        };
    }  // namespace Math
}  // namespace Core
