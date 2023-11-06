#pragma once

#include "Constants.hpp"
#include "Matrix.hpp"
#include "Timer.h"

class Camera final
{
public:
	Camera(const Vector3& origin, float fieldOfViewAngle = TO_RADIANS * 45.0f);

	inline Matrix CalculateCameraToWorld() const
	{
		const Vector3& upDirection{ Vector3::Cross(m_ForwardDirection, m_RightDirection).GetNormalized() };

		return Matrix
		(
			m_RightDirection.GetVector4(),
			upDirection.GetVector4(),
			m_ForwardDirection.GetVector4(),
			m_Origin.GetPoint4()
		);
	}

	void Update(const Timer& timer);

	inline void SetOrigin(const Vector3& origin)
	{
		m_Origin = origin;
		m_TargetOrigin = m_Origin;
	}

	inline void SetFieldOfViewAngle(float angle)
	{
		static const float MAX_FOV_ANGLE{ TO_RADIANS * 180.0f };
		m_TargetFieldOfViewAngle = std::max(0.0f, std::min(angle, MAX_FOV_ANGLE));
	}

	inline void IncrementFieldOfViewAngle(float angleIncrementer)
	{
		SetFieldOfViewAngle(m_FieldOfViewAngle + angleIncrementer);
	}

	inline const Vector3& GetOrigin() const
	{
		return m_Origin;
	}

	inline float GetFieldOfViewValue() const
	{
		return m_FieldOfViewValue;
	}

private:
	Vector3
		m_Origin, m_TargetOrigin,
		m_ForwardDirection,
		m_RightDirection;

	float
		m_FieldOfViewAngle, m_TargetFieldOfViewAngle,
		m_FieldOfViewValue,

		m_TotalPitch, m_TargetPitch,
		m_TotalYaw, m_TargetYaw;

	const float m_SmoothFactor;

	Matrix m_CameraToWorld;
};