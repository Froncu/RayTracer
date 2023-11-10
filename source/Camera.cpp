#include "Camera.h"

#include "SDL.h"
#include "Utilities.hpp"

Camera::Camera(const Vector3& origin, float fieldOfViewAngle) :
	m_Origin{ origin },
	m_ForwardDirection{ VECTOR3_UNIT_Z },
	m_RightDirection{ VECTOR3_UNIT_X },
	m_UpDirection{ VECTOR3_UNIT_Y },

	m_FieldOfViewAngle{ fieldOfViewAngle },
	m_FieldOfViewValue{ tanf(m_FieldOfViewAngle / 2.0f) },

	m_TotalPitch{},
	m_TotalYaw{},

	m_CameraToWorld{},

	m_DidMove{}
{
	UpdateCameraToWorld();
}

void Camera::Update(const Timer& timer)
{
	static constexpr float
		MOVEMENT_SPEED{ 15.0f },
		MOUSE_SENSITIVITY{ 0.25f },
		MAX_TOTAL_PITCH{ TO_RADIANS * 90.0f - FLT_EPSILON },
		DEFAULT_FIELD_OF_VIEW_ANGLE{ TO_RADIANS * 45.0f },
		MOUSE_MOVEMENT_ORIGIN_CORRECTOR{ 0.0625f };

	const float
		deltaTime{ timer.GetElapsed() },
		fieldOfViewScalar{ std::min(m_FieldOfViewAngle / DEFAULT_FIELD_OF_VIEW_ANGLE, 1.0f) };

	m_DidMove = false;

	//	Mouse Input
	int mouseX, mouseY;
	const uint32_t mouseState{ SDL_GetRelativeMouseState(&mouseX, &mouseY) };

	switch (mouseState)
	{
	case SDL_BUTTON(1):
		m_Origin -= m_ForwardDirection * MOVEMENT_SPEED * float(mouseY) * MOUSE_SENSITIVITY * MOUSE_MOVEMENT_ORIGIN_CORRECTOR;
		m_TotalYaw += TO_RADIANS * fieldOfViewScalar * mouseX * MOUSE_SENSITIVITY;
		UpdateCameraToWorld();
		m_DidMove = true;
		break;

	case SDL_BUTTON(3):
		m_TotalYaw += TO_RADIANS * fieldOfViewScalar * mouseX * MOUSE_SENSITIVITY;
		m_TotalPitch += TO_RADIANS * fieldOfViewScalar * mouseY * MOUSE_SENSITIVITY;
		m_TotalPitch = std::max(-MAX_TOTAL_PITCH, std::min(m_TotalPitch, MAX_TOTAL_PITCH));
		UpdateCameraToWorld();
		m_DidMove = true;
		break;

	case SDL_BUTTON_X2:
		m_Origin -= m_UpDirection * MOVEMENT_SPEED * float(mouseY) * MOUSE_SENSITIVITY * MOUSE_MOVEMENT_ORIGIN_CORRECTOR;
		m_DidMove = true;
		break;
	}

	//	Keyboard Input
	const uint8_t* pKeyboardState{ SDL_GetKeyboardState(nullptr) };

	if (pKeyboardState[SDL_SCANCODE_W])
	{
		m_Origin += m_ForwardDirection * deltaTime * MOVEMENT_SPEED;
		m_DidMove = true;
	}

	if (pKeyboardState[SDL_SCANCODE_S])
	{
		m_Origin -= m_ForwardDirection * deltaTime * MOVEMENT_SPEED;
		m_DidMove = true;
	}

	if (pKeyboardState[SDL_SCANCODE_A])
	{
		m_Origin -= m_RightDirection * deltaTime * MOVEMENT_SPEED;
		m_DidMove = true;
	}

	if (pKeyboardState[SDL_SCANCODE_D])
	{
		m_Origin += m_RightDirection * deltaTime * MOVEMENT_SPEED;
		m_DidMove = true;
	}
}