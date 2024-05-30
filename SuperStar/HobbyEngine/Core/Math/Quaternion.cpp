#include "Math/math.h"

#include "Math/quaternion.h"

namespace Math
{
	// 行列が行優先前提で処理している
	Quaternion Quaternion::Identity(0.0f, 0.0f, 0.0f, 1.0f);

	Quaternion::Quaternion()
	{
		*this = Identity;
	}

	Quaternion::Quaternion(const float in_x, const float in_y, const float in_z, const float in_w)
	{
		this->Set(in_x, in_y, in_z, in_w);
	}

	Quaternion::Quaternion(const Vector3& in_rAxis, const float angle)
	{
		float scaler = Sin(angle * 0.5f);
		this->x = in_rAxis.x * scaler;
		this->y = in_rAxis.y * scaler;
		this->z = in_rAxis.z * scaler;

		this->w = Cos(angle * 0.5f);
	}

	auto Quaternion::AngleUnitRadian(const Vector3& in_rAngle) -> void
	{
		// オイラー角をクォータニオンとして設定
		float cosXAxis = Cos(in_rAngle.x * 0.5f);
		float sinXAxis = Sin(in_rAngle.x * 0.5f);
		float cosYAxis = Cos(in_rAngle.y * 0.5f);
		float sinYAxis = Sin(in_rAngle.y * 0.5f);
		float cosZAxis = Cos(in_rAngle.z * 0.5f);
		float sinZAxis = Sin(in_rAngle.z * 0.5f);

		// ローカル座標系からワールド座標系への変換
		this->w = cosYAxis * cosXAxis * cosZAxis + sinYAxis * sinXAxis * sinZAxis;
		this->x = cosYAxis * sinXAxis * cosZAxis + sinYAxis * cosXAxis * sinZAxis;
		this->y = sinYAxis * cosXAxis * cosZAxis - cosYAxis * sinXAxis * sinZAxis;
		this->z = cosYAxis * cosXAxis * sinZAxis - sinYAxis * sinXAxis * cosZAxis;
	}

	auto Quaternion::Set(const float in_x, const float in_y, const float in_z, const float in_w) -> void
	{
		this->x = in_x;
		this->y = in_y;
		this->z = in_z;
		this->w = in_w;
	}

	/// <summary>
	/// Concatenates the specified in q.
	/// </summary>
	/// <param name="in_q">The in q.</param>
	/// <param name="in_p">The in p.</param>
	/// <returns></returns>
	Quaternion Quaternion::Concatenate(const Quaternion& in_q, const Quaternion& in_p)
	{
		Quaternion retVal;

		Vector3 pv(in_p.x, in_p.y, in_p.z);
		Vector3 qv(in_q.x, in_q.y, in_q.z);

		Vector3 newVec = (qv * in_p.w) + (pv * in_q.w) + Vector3::Cross(pv, qv);
		retVal.x = newVec.x;
		retVal.y = newVec.y;
		retVal.z = newVec.z;

		retVal.w = in_p.w * in_q.w - Vector3::Dot(pv, qv);

		return retVal;
	}
}