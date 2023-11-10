#pragma once

#include <iostream>

#include "Constants.hpp"
#include "Matrix.hpp"
#include "Timer.h"

class Camera final
{
public:
	Camera(const Vector3& origin = Vector3(0.0f, 0.0f, 0.0f), float fieldOfViewAngle = TO_RADIANS * 45.0f);

	inline const Matrix& GetCameraToWorld() const
	{
		return m_CameraToWorld;
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
		system("CLS");
		std::cout
			<< CONTROLS
			<< "--------\n" 
			<< "FIELD OF VIEW ANGLE: " << TO_DEGREES * m_FieldOfViewAngle << " degrees\n--------";
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
	inline void UpdateCameraToWorld()
	{
		static constexpr Vector3 WORLD_UP{ 0.0f, 1.0f, 0.0f };
		m_ForwardDirection = Matrix(Matrix::CreateRotorX(m_TotalPitch) * Matrix::CreateRotorY(m_TotalYaw)).TransformVector(VECTOR3_UNIT_Z);
		m_RightDirection = Vector3::Cross(WORLD_UP, m_ForwardDirection).GetNormalized();
		m_UpDirection = Vector3::Cross(m_ForwardDirection, m_RightDirection).GetNormalized();

		m_CameraToWorld = Matrix
		(
			m_RightDirection.GetVector4(),
			m_UpDirection.GetVector4(),
			m_ForwardDirection.GetVector4(),
			m_Origin.GetPoint4()
		);
	}

	Vector3
		m_Origin,
		m_ForwardDirection,
		m_RightDirection,
		m_UpDirection;

	float
		m_FieldOfViewAngle,
		m_FieldOfViewValue,

		m_TotalPitch,
		m_TotalYaw;

	Matrix m_CameraToWorld;

	bool m_DidMove;
};