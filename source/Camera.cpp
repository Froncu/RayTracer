#include "Camera.h"

#include "SDL.h"
#include "Utilities.hpp"

Camera::Camera(const Vector3& origin, float fieldOfViewAngle) :
	m_Origin{ origin }, m_TargetOrigin{ m_Origin },
	m_ForwardDirection{ VECTOR3_UNIT_Z },
	m_RightDirection{ VECTOR3_UNIT_X },

	m_FieldOfViewAngle{ fieldOfViewAngle }, m_TargetFieldOfViewAngle{ m_FieldOfViewAngle },
	m_FieldOfViewValue{ tanf(m_FieldOfViewAngle / 2.0f) },

	m_TotalPitch{}, m_TargetPitch{},
	m_TotalYaw{}, m_TargetYaw{},

	m_SmoothFactor{ 0.25f },

	m_CameraToWorld{ CalculateCameraToWorld() }
{
}

void Camera::Update(const Timer& timer)
{
	static constexpr float
		MOVEMENT_SPEED{ 15.0f },
		ROTATION_SPEED{ 0.25f },
		MAX_TOTAL_PITCH{ TO_RADIANS * 90.0f - FLT_EPSILON },
		DEFAULT_FIELD_OF_VIEW_ANGLE{ TO_RADIANS * 45.0f };

	static constexpr Vector3 WORLD_UP{ 0.0f, 1.0f, 0.0f };

	const float
		deltaTime{ timer.GetElapsed() },
		fieldOfViewScalar{ std::min(m_FieldOfViewAngle / DEFAULT_FIELD_OF_VIEW_ANGLE, 1.0f) };

	//	Mouse Input
	int mouseX, mouseY;
	const uint32_t mouseState{ SDL_GetRelativeMouseState(&mouseX, &mouseY) };

	switch (mouseState)
	{
	case SDL_BUTTON(1):
		m_TargetOrigin -= MOVEMENT_SPEED * m_ForwardDirection * float(mouseY) * deltaTime;
		m_TargetYaw += ROTATION_SPEED * fieldOfViewScalar * mouseX * deltaTime;
		break;

	case SDL_BUTTON(3):
		m_TargetYaw += ROTATION_SPEED * fieldOfViewScalar * mouseX * deltaTime;
		m_TargetPitch += ROTATION_SPEED * fieldOfViewScalar * mouseY * deltaTime;
		m_TargetPitch = std::max(-MAX_TOTAL_PITCH, std::min(m_TargetPitch, MAX_TOTAL_PITCH));
		break;
	}

	//	Keyboard Input
	const uint8_t* pKeyboardState{ SDL_GetKeyboardState(nullptr) };

	if (pKeyboardState[SDL_SCANCODE_W])
		m_TargetOrigin += MOVEMENT_SPEED * m_ForwardDirection * deltaTime;

	if (pKeyboardState[SDL_SCANCODE_S])
		m_TargetOrigin -= MOVEMENT_SPEED * m_ForwardDirection * deltaTime;

	if (pKeyboardState[SDL_SCANCODE_A])
		m_TargetOrigin -= MOVEMENT_SPEED * m_RightDirection * deltaTime;

	if (pKeyboardState[SDL_SCANCODE_D])
		m_TargetOrigin += MOVEMENT_SPEED * m_RightDirection * deltaTime;

	//	Lerp
	m_Origin = Lerp(m_Origin, m_TargetOrigin, m_SmoothFactor);
	m_TotalYaw = Lerp(m_TotalYaw, m_TargetYaw, m_SmoothFactor);
	m_TotalPitch = Lerp(m_TotalPitch, m_TargetPitch, m_SmoothFactor);
	m_FieldOfViewAngle = Lerp(m_FieldOfViewAngle, m_TargetFieldOfViewAngle, m_SmoothFactor);

	m_FieldOfViewValue = tanf(m_FieldOfViewAngle / 2.0f);

	m_ForwardDirection = Matrix(Matrix::CreateRotorX(m_TotalPitch) * Matrix::CreateRotorY(m_TotalYaw)).TransformVector(VECTOR3_UNIT_Z);
	m_RightDirection = Vector3::Cross(WORLD_UP, m_ForwardDirection).GetNormalized();
}