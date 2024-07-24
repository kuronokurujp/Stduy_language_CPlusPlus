#pragma once
#include <cmath>

#include "Engine/Core.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "Rect2.h"
#include "Vector2.h"
#include "Vector3.h"

namespace Core
{
    namespace Math
    {
        // 円周率
        static const Float32 fPI                = 3.1415926535f;
        static const Float32 fTwoPI             = 2 * fPI;
        static const Float32 fPIOver2           = fPI / 2.0f;
        static const Float32 fRadiansUnit       = fPI / 180.0f;
        static const Float32 fInvertRadiansUnit = 180.0f / fPI;

        /// <summary>
        /// Radianses to degrees.
        /// </summary>
        /// <param name="in_radians">The in radians.</param>
        static inline const Float32 RadiansToDegrees(Float32 in_fRadians) HE_NOEXCEPT
        {
            return fInvertRadiansUnit * in_fRadians;
        }

        /// <summary>
        /// Degreeses to radians.
        /// </summary>
        /// <param name="in_degrees">The in degrees.</param>
        static inline const Float32 DegreesToRadians(Float32 in_fDegrees) HE_NOEXCEPT
        {
            return fRadiansUnit * in_fDegrees;
        }

        /// <summary>
        /// Sins the specified in angle.
        /// </summary>
        static inline Float32 Sin(const Float32 in_fAngle) HE_NOEXCEPT
        {
            return sinf(in_fAngle);
        }

        /// <summary>
        /// Coses the specified in angle.
        /// </summary>
        static inline Float32 Cos(const Float32 in_fAngle) HE_NOEXCEPT
        {
            return cosf(in_fAngle);
        }

        /// <summary>
        /// Tans the specified in angle.
        /// </summary>
        /// <param name="in_angle">The in angle.</param>
        static inline Float32 Tan(const Float32 in_fAngle) HE_NOEXCEPT
        {
            return tanf(in_fAngle);
        }

        /// <summary>
        /// Acoses the specified in value.
        /// </summary>
        /// <param name="in_value">The in value.</param>
        static inline Float32 Acos(const Float32 in_fValue) HE_NOEXCEPT
        {
            return acosf(in_fValue);
        }

        /// <summary>
        /// Atan2s the specified in x.
        /// </summary>
        /// <param name="in_x">The in x.</param>
        /// <param name="in_y">The in y.</param>
        static inline Float32 Atan2(const Float32 in_fX, const Float32 in_fY) HE_NOEXCEPT
        {
            return atan2f(in_fY, in_fX);
        }

        static inline Float32 Cot(const Float32 in_fAngle)
        {
            return 1.0f / Tan(in_fAngle);
        }

        /// <summary>
        /// 値が直近0かどうか.
        /// </summary>
        /// <param name="in_value">The in value.</param>
        /// <param name="in_epsilon">The in epsilon.</param>
        static inline Bool NearZero(const Float32 in_fValue,
                                    const Float32 in_fEpsilon = 0.001f) HE_NOEXCEPT
        {
            if (fabsf(in_fValue) <= in_fEpsilon)
            {
                return TRUE;
            }

            return FALSE;
        }

        /// <summary>
        /// Rounds the f.
        /// </summary>
        /// <param name="in_value">The in value.</param>
        static inline Float32 RoundF(const Float32 in_fValue) HE_NOEXCEPT
        {
            // 四捨五入する時に+/-でずらす方向を変えている
            Float32 offset = (in_fValue >= 0.0f) ? 0.5f : -0.5f;
            return static_cast<Float32>(static_cast<Sint32>(in_fValue + offset));
        }

        /// <summary>
        /// Rounds the specified in r value.
        /// </summary>
        /// <param name="in_rValue">The in r value.</param>
        static inline Sint32 Round(const Float32 in_fValue) HE_NOEXCEPT
        {
            return static_cast<Sint32>(RoundF(in_fValue));
        }

        /// <summary>
        /// Vector3s the transform.
        /// </summary>
        extern Vector3 Vector3Transform(const Vector3& in_rVec3, const Quaternion& in_rQuaternion);
        /// <summary>
        /// Matrix4s the form quaternion.
        /// </summary>
        extern Matrix4 Matrix4FormQuaternion(const Quaternion& in_rQuaternion);
    }  // namespace Math
}  // namespace Core
