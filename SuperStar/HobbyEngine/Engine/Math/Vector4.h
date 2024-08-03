#pragma once

#include "Engine/Core.h"

namespace Core::Math
{
    class Vector4
    {
    public:
        explicit Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        explicit Vector4(float aX, float aY, float aZ, float aW) : x(aX), y(aY), z(aZ), w(aW) {}

        Vector4 Mul(const Vector4& in_rSrc)
        {
            float outputX = this->x * in_rSrc.x;
            float outputY = this->y * in_rSrc.y;
            float outputZ = this->z * in_rSrc.z;
            float outputW = this->w * in_rSrc.w;

            return Vector4(outputX, outputY, outputZ, outputW);
        }

        float AddMul(const Vector4& in_rSrc)
        {
            float outputX = this->x * in_rSrc.x;
            float outputY = this->y * in_rSrc.y;
            float outputZ = this->z * in_rSrc.z;
            float outputW = this->w * in_rSrc.w;

            return (outputX + outputY + outputZ + outputW);
        }

        // 頻繁にアクセスする変数にはprivate指定にはしない
        float x, y, z, w;
    };
}  // namespace Core::Math
