#include "Vector3.h"
#include <math.h>

namespace Core
{
    namespace Math
    {
        Vector3 Vector3::Unit_X = Vector3(1.0f, 0.0f, 0.0f);
        Vector3 Vector3::Unit_Y = Vector3(0.0f, 1.0f, 0.0f);
        Vector3 Vector3::Unit_Z = Vector3(0.0f, 0.0f, 1.0f);
        Vector3 Vector3::Zero = Vector3();
        Vector3 Vector3::One = Vector3(1.0f, 1.0f, 1.0f);

        Vector3::Vector3(const Sint32 aX, const Sint32 aY, const Sint32 aZ)
        {
            x = static_cast<Float32>(aX);
            y = static_cast<Float32>(aY);
            z = static_cast<Float32>(aZ);
        }

        // 値設定
        void Vector3::Set(const Float32 in_x, const Float32 in_y, const Float32 in_z)
        {
            this->x = in_x;
            this->y = in_y;
            this->z = in_z;
        }

        void Vector3::Set(const Float32 in_val)
        {
            this->x = this->y = this->z = in_val;
        }

        void Vector3::operator+=(const Vector3& a)
        {
            this->x += a.x;
            this->y += a.y;
            this->z += a.z;
        }

        void Vector3::operator-=(const Vector3& a)
        {
            this->x -= a.x;
            this->y -= a.y;
            this->z -= a.z;
        }

        void Vector3::operator=(const Vector3& a)
        {
            this->x = a.x;
            this->y = a.y;
            this->z = a.z;
        }

        void Vector3::operator*=(const Vector3& a)
        {
            this->x *= a.x;
            this->y *= a.y;
            this->z *= a.z;
        }

        // 割り算
        void Vector3::operator/=(const Vector3& a)
        {
            this->x /= a.x;
            this->y /= a.y;
            this->z /= a.z;
        }

        // 掛け算
        void Vector3::Mul(const Float32 b)
        {
            this->x *= b;
            this->y *= b;
            this->z *= b;
        }

        // 積和
        // a 方向ベクトル
        // b 方向ベクトルのスカラー
        void Vector3::Madd(const Vector3& a, const Float32 b)
        {
            this->x += (a.x * b);
            this->y += (a.y * b);
            this->z += (a.z * b);
        }

        // 積和設定
        // a 方向ベクトル
        // b 方向ベクトルの長さスカラー
        // c 視点ベクトル
        void Vector3::SetMadd(const Vector3& a, const Float32 b, const Vector3& c)
        {
            this->x = a.x * b + c.x;
            this->y = a.y * b + c.y;
            this->z = a.z * b + c.z;
        }

        // 加算
        void Vector3::SetAdd(const Vector3& a, const Vector3& b)
        {
            this->x = a.x + b.x;
            this->y = a.y + b.y;
            this->z = a.z + b.z;
        }

        // 減算
        void Vector3::SetSub(const Vector3& a, const Vector3& b)
        {
            this->x = a.x - b.x;
            this->y = a.y - b.y;
            this->z = a.z - b.z;
        }

        // 積算
        void Vector3::SetMul(const Vector3& a, const Vector3& b)
        {
            this->x = a.x * b.x;
            this->y = a.y * b.y;
            this->z = a.z * b.z;
        }

        // ２次元補間を行う
        void Vector3::SetInterporation(
            const Vector3& a,
            const Vector3& ab,
            const Vector3& ac,
            Float32 u,
            Float32 v)
        {
            this->SetMadd(ab, u, a);	// a + u(b - a)
            this->Madd(ac, v);	// a + u(b - a) + v(c - a)
        }

        //	正規化
        void Vector3::Normalize()
        {
            Float32 mag_sq = this->x * this->x + this->y * this->y + this->z * this->z;
            if (mag_sq > 0.f)
            {
                Float32 onew_over_mag = 1.f / static_cast<Float32>(sqrt(mag_sq));
                this->x *= onew_over_mag;
                this->y *= onew_over_mag;
                this->z *= onew_over_mag;
            }
        }

        // 内積
        Float32 Vector3::Dot(const Vector3& in_rV, const Vector3& in_rV2)
        {
            return (in_rV.x * in_rV2.x + in_rV.y * in_rV2.y + in_rV.z * in_rV2.z);
        }

        // 外積
        Vector3 Vector3::Cross(const Vector3& in_rV, const Vector3& in_rV2)
        {
            Vector3 temp;
            temp.x = in_rV.y * in_rV2.z - in_rV.z * in_rV2.y;
            temp.y = in_rV.z * in_rV2.x - in_rV.x * in_rV2.z;
            temp.z = in_rV.x * in_rV2.y - in_rV.y * in_rV2.x;

            return temp;
        }

        // ベクトルの大きさを2乗したのを取得
        // こちらの方が計算が早い
        // 比較などで利用できる
        Float32 Vector3::GetLengthSquared(const Vector3& in_rV)
        {
            return	(in_rV.x * in_rV.x + in_rV.y * in_rV.y * in_rV.z * in_rV.z);
        }

        //	大きさ取得
        Float32 Vector3::GetVector3Mag(const Vector3& in_v)
        {
            return	static_cast<Float32>(sqrt(GetLengthSquared(in_v)));
        }

        //	距離取得
        Float32 Vector3::GetVector3Distance(const Vector3& in_v, const Vector3& in_v2)
        {
            Vector3	size;
            size.SetSub(in_v2, in_v);

            return (GetVector3Mag(size));
        }
    }
}
