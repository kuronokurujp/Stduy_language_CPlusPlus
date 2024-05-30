#ifndef __MATH__QUATERNION_H__
#define __MATH__QUATERNION_H__

#include "vector3.h"

namespace Math
{
	class Quaternion
	{
	public:

		Quaternion();

		explicit Quaternion(const float in_x, const float in_y, const float in_z, const float in_w);
		explicit Quaternion(const Vector3& in_rAxis, const float angle);

		~Quaternion() {}

		/// <summary>
		/// Sets the specified in x.
		/// </summary>
		/// <param name="in_x">The in x.</param>
		/// <param name="in_y">The in y.</param>
		/// <param name="in_z">The in z.</param>
		/// <param name="in_w">The in w.</param>
		/// <returns></returns>
		void Set(const float in_x, const float in_y, const float in_z, const float in_w);

		/// <summary>
		/// Angles the specified in in r angle.
		/// </summary>
		/// <param name="in_in_rAngle">The in in r angle.</param>
		/// <returns></returns>
		void AngleUnitRadian(const Vector3& in_in_rAngle);

		float x, y, z, w;

		/// <summary>
		/// Concatenates the specified in q.
		/// </summary>
		/// <param name="in_q">The in q.</param>
		/// <param name="in_p">The in p.</param>
		/// <returns></returns>
		static Quaternion Concatenate(const Quaternion& in_q, const Quaternion& in_p);

		static Quaternion Identity;
	};
}

#endif // __MATH__QUATERNION_H__
