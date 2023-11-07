#pragma once

#include <iostream>

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
	}

	inline void SetFieldOfViewAngle(float angle)
	{
		static const float MAX_FOV_ANGLE{ TO_RADIANS * 180.0f - FLT_EPSILON };
		m_FieldOfViewAngle = std::max(FLT_EPSILON, std::min(angle, MAX_FOV_ANGLE));
		m_FieldOfViewValue = tanf(m_FieldOfViewAngle / 2.0f);
	}

	inline void IncrementFieldOfViewAngle(float angleIncrementer)
	{
		SetFieldOfViewAngle(m_FieldOfViewAngle + angleIncrementer);
		std::cout << "--------\n" << "FIELD OF VIEW ANGLE: " << TO_DEGREES * m_FieldOfViewAngle << " degrees\n--------\n";
	}

	inline const Vector3& GetOrigin() const
	{
		return m_Origin;
	}

	inline float GetFieldOfViewValue() const
	{
		return m_FieldOfViewValue;
	}

	inline bool DidMove() const
	{
		return m_DidMove;
	}

private:
	Vector3
		m_Origin,
		m_ForwardDirection,
		m_RightDirection;

	float
		m_FieldOfViewAngle,
		m_FieldOfViewValue,

		m_TotalPitch,
		m_TotalYaw;

	Matrix m_CameraToWorld;

	bool m_DidMove;
};