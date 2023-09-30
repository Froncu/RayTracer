#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
	public:
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
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
				MOVEMENT_SPEED{ 10.0f },
				ROTATION_SPEED{ 0.25f };

			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			if (pKeyboardState[SDL_SCANCODE_W])
				origin += MOVEMENT_SPEED * forward * deltaTime;

			if (pKeyboardState[SDL_SCANCODE_S])
				origin -= MOVEMENT_SPEED * forward * deltaTime;

			if (pKeyboardState[SDL_SCANCODE_A])
				origin -= MOVEMENT_SPEED * right * deltaTime;

			if (pKeyboardState[SDL_SCANCODE_D])
				origin += MOVEMENT_SPEED * right * deltaTime;

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			switch (mouseState)
			{
			case SDL_BUTTON(1):
				origin -= MOVEMENT_SPEED * forward * float(mouseY) * deltaTime;
				totalYaw += ROTATION_SPEED * mouseX * deltaTime;
				break;

			case SDL_BUTTON(3):
				totalYaw += ROTATION_SPEED * mouseX * deltaTime;
				totalPitch += ROTATION_SPEED * mouseY * deltaTime;
				break;
			}

			//todo: W2
			forward = Matrix(Matrix::CreateRotationX(totalPitch) * Matrix::CreateRotationY(totalYaw)).TransformVector(Vector3::UnitZ);
		}
		
		void SetFieldOfViewAngle(float angle)
		{
			fovAngle = angle;
			fovValue = tanf(dae::TO_RADIANS * fovAngle / 2.0f);
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
		float
			fovAngle{ 90.f },
			fovValue{ tanf(dae::TO_RADIANS * fovAngle / 2.0f) };
	};
}
