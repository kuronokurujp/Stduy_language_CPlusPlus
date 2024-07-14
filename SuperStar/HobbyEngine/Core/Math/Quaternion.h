#pragma once

#include "Vector3.h"

namespace Core
{
    namespace Math
    {
        class Quaternion
        {
        public:
            Quaternion();

            explicit Quaternion(const Float32 in_fX, const Float32 in_fY, const Float32 in_fZ,
                                const Float32 in_fW);
            explicit Quaternion(const Vector3& in_rAxis, const Float32 in_fAngle);

            ~Quaternion() {}

            /// <summary>
            /// Sets the specified in x.
            /// </summary>
            /// <param name="in_x">The in x.</param>
            /// <param name="in_y">The in y.</param>
            /// <param name="in_z">The in z.</param>
            /// <param name="in_w">The in w.</param>
            /// <returns></returns>
            void Set(const Float32 in_fX, const Float32 in_fY, const Float32 in_fZ,
                     const Float32 in_fW);

            /// <summary>
            /// Angles the specified in in r angle.
            /// </summary>
            /// <param name="in_in_rAngle">The in in r angle.</param>
            /// <returns></returns>
            void AngleUnitRadian(const Vector3& in_in_rAngle);

            Float32 _fX, _fY, _fZ, _fW;

            /// <summary>
            /// Concatenates the specified in q.
            /// </summary>
            /// <param name="in_q">The in q.</param>
            /// <param name="in_p">The in p.</param>
            /// <returns></returns>
            static Quaternion Concatenate(const Quaternion& in_fQ, const Quaternion& in_fP);

            static const Quaternion Identity;
        };
    }  // namespace Math
}  // namespace Core
