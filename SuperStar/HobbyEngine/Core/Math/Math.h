#ifndef __CORE_MATH_H__
#define __CORE_MATH_H__

#include <cmath>
#include "Vector2.h"
#include "Rect2.h"
//#include "vector3.h"
//#include "quaternion.h"
//#include "matrix4.h"

namespace Core
{
    namespace Math
    {
        // 円周率
        static const float PI = 3.1415926535f;
        static const float TwoPI = 2 * PI;
        static const float PIOver2 = PI / 2.0f;
        static const float RADIANS_UNIT = PI / 180.0f;
        static const float INVERT_RADIANS_UNIT = 180.0f / PI;

        /// <summary>
        /// Radianses to degrees.
        /// </summary>
        /// <param name="in_radians">The in radians.</param>
        /// <returns></returns>
        static inline const float RadiansToDegrees(float in_radians)
        {
            return INVERT_RADIANS_UNIT * in_radians;
        }

        /// <summary>
        /// Degreeses to radians.
        /// </summary>
        /// <param name="in_degrees">The in degrees.</param>
        /// <returns></returns>
        static inline const float DegreesToRadians(float in_degrees)
        {
            return RADIANS_UNIT * in_degrees;
        }

        /// <summary>
        /// Sins the specified in angle.
        /// </summary>
        /// <param name="in_angle">The in angle.</param>
        /// <returns></returns>
        static inline float Sin(const float in_angle)
        {
            return sinf(in_angle);
        }

        /// <summary>
        /// Coses the specified in angle.
        /// </summary>
        /// <param name="in_angle">The in angle.</param>
        /// <returns></returns>
        static inline float Cos(const float in_angle)
        {
            return cosf(in_angle);
        }

        /// <summary>
        /// Tans the specified in angle.
        /// </summary>
        /// <param name="in_angle">The in angle.</param>
        /// <returns></returns>
        static inline float Tan(const float in_angle)
        {
            return tanf(in_angle);
        }

        /// <summary>
        /// Acoses the specified in value.
        /// </summary>
        /// <param name="in_value">The in value.</param>
        /// <returns></returns>
        static inline float Acos(const float in_value)
        {
            return acosf(in_value);
        }

        /// <summary>
        /// Atan2s the specified in x.
        /// </summary>
        /// <param name="in_x">The in x.</param>
        /// <param name="in_y">The in y.</param>
        /// <returns></returns>
        static inline float Atan2(const float in_x, const float in_y)
        {
            return atan2f(in_y, in_x);
        }

        static inline float Cot(const float in_angle)
        {
            return 1.0f / Tan(in_angle);
        }

        /// <summary>
        /// 値が直近0かどうか.
        /// </summary>
        /// <param name="in_value">The in value.</param>
        /// <param name="in_epsilon">The in epsilon.</param>
        /// <returns></returns>
        static inline bool NearZero(const float in_value, const float in_epsilon = 0.001f)
        {
            if (fabsf(in_value) <= in_epsilon)
            {
                return true;
            }

            return false;
        }

        /// <summary>
        /// Rounds the f.
        /// </summary>
        /// <param name="in_value">The in value.</param>
        /// <returns></returns>
        static inline float RoundF(const float in_value)
        {
            // 四捨五入する時に+/-でずらす方向を変えている
            float offset = (in_value >= 0.0f) ? 0.5f : -0.5f;
            return static_cast<float>(static_cast<int>(in_value + offset));
        }

        /// <summary>
        /// Rounds the specified in r value.
        /// </summary>
        /// <param name="in_rValue">The in r value.</param>
        /// <returns></returns>
        static inline int Round(const float in_value)
        {
            return static_cast<int>(RoundF(in_value));
        }

        /// <summary>
        /// Maximums the specified a.
        /// </summary>
        /// <param name="a">a.</param>
        /// <param name="b">The b.</param>
        /// <returns></returns>
        static inline float Max(const float a, const float b)
        {
            if (a > b)
            {
                return a;
            }

            return b;
        }

#if 0
        /// <summary>
        /// Vector3s the transform.
        /// </summary>
        /// <param name="in_rVec3">The in r vec3.</param>
        /// <param name="in_rQuaternion">The in r quaternion.</param>
        /// <returns></returns>
        Vector3 Vector3Transform(const Vector3&in_rVec3, const Quaternion& in_rQuaternion);

        /// <summary>
        /// Matrix4s the form quaternion.
        /// </summary>
        /// <param name="in_rQuaternion">The in r quaternion.</param>
        /// <returns></returns>
        Matrix4 Matrix4FormQuaternion(const Quaternion& in_rQuaternion);
#endif
    }
}

#endif
