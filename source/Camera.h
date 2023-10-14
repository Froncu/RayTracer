#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"
#include <iostream>

namespace dae
{
	struct Camera
	{
	public:
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle},

			targetOrigin{ origin },
			targetfovAngle{ fovAngle }
		{
		}

		Vector3 origin{};

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};
		Vector3 movingDirection{};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};

		const float smoothFactor{ 0.25f };

		Vector3 targetOrigin{ origin };

		float
			targetYaw{ totalPitch },
			targetPitch{ totalYaw };

		Matrix CalculateCameraToWorld()
		{
			//todo: W2
			static const Vector3 WORLD_UP{ 0.0f, 1.0f, 0.0f };

			right = Vector3::Cross(WORLD_UP, forward);
			up = Vector3::Cross(forward, right);

			cameraToWorld = Matrix
			(
				right.Normalized(),
				up.Normalized(),
				forward.Normalized(),
				Vector4(origin.x, origin.y, origin.z, 1.0f)
			);

			return cameraToWorld;
		}

		void Update(Timer* pTimer)
		{
			static const float
				MOVEMENT_SPEED{ 15.0f },
				ROTATION_SPEED{ 0.25f },
				MAX_TOTAL_PITCH{ float(M_PI) / 2.0f - 0.0001f },
				DEFAULT_FOV_ANGLE{ 45.0f };

			const float 
				deltaTime = pTimer->GetElapsed(),
				fovScalar{ std::min(fovAngle / DEFAULT_FOV_ANGLE, 1.0f) };


			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			if (pKeyboardState[SDL_SCANCODE_W])
				targetOrigin += MOVEMENT_SPEED * forward * deltaTime;

			if (pKeyboardState[SDL_SCANCODE_S])
				targetOrigin -= MOVEMENT_SPEED * forward * deltaTime;

			if (pKeyboardState[SDL_SCANCODE_A])
				targetOrigin -= MOVEMENT_SPEED * right * deltaTime;

			if (pKeyboardState[SDL_SCANCODE_D])
				targetOrigin += MOVEMENT_SPEED * right * deltaTime;

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			switch (mouseState)
			{
			case SDL_BUTTON(1):
				targetOrigin -= MOVEMENT_SPEED * forward * float(mouseY) * deltaTime;
				targetYaw += ROTATION_SPEED * fovScalar * mouseX * deltaTime;
				break;

			case SDL_BUTTON(3):
				targetYaw += ROTATION_SPEED * fovScalar * mouseX * deltaTime;
				targetPitch += ROTATION_SPEED * fovScalar * mouseY * deltaTime;
				targetPitch = std::max(-MAX_TOTAL_PITCH, std::min(targetPitch, MAX_TOTAL_PITCH));
				break;
			}

			//todo: W2
			origin = Lerp(origin, targetOrigin, smoothFactor);
			totalYaw = Lerp(totalYaw, targetYaw, smoothFactor);
			totalPitch = Lerp(totalPitch, targetPitch, smoothFactor);
			fovAngle = Lerp(fovAngle, targetfovAngle, smoothFactor);
			fovValue = tanf(dae::TO_RADIANS * fovAngle / 2.0f);

			forward = Matrix(Matrix::CreateRotationX(totalPitch) * Matrix::CreateRotationY(totalYaw)).TransformVector(Vector3::UnitZ);
		}

		void SetOrigin(const Vector3& _origin)
		{
			origin = _origin;
			targetOrigin = origin;
		}
		
		void SetFieldOfViewAngle(float angle)
		{
			static float MAX_FOV_ANGLE{ 180.0f };

			targetfovAngle = std::max(0.0f, std::min(angle, MAX_FOV_ANGLE));
		}
		
		void IncrementFieldOfViewAngle(float angle)
		{
			SetFieldOfViewAngle(fovAngle + angle);
		}
		
		float GetFieldOfViewValue() const
		{
			return fovValue;
		}

	private:
		float Lerp(float a, float b, float t)
		{
			return a + t * (b - a);
		}

		Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
		{
			return a + t * (b - a);
		}

		float
			fovAngle{ 90.f },
			targetfovAngle{ fovAngle },
			fovValue{ tanf(dae::TO_RADIANS * fovAngle / 2.0f) };
	};
}
