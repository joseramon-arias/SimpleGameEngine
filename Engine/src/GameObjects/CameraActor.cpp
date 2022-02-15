
#include "CameraActor.h"
#include "MoveComponent.h"
#include "SDL_scancode.h"
#include "Renderer.h"
#include "Game.h"

namespace Engine
{
	CameraActor::CameraActor(Game* game) :
		Actor(game)
	{
		m_MoveComp = new MoveComponent(this);
	}

	void CameraActor::UpdateActor(float deltaTime)
	{
		Actor::UpdateActor(deltaTime);

		Vector3 cameraPos = GetPosition();
		Vector3 target = GetPosition() + GetForward() * 100.0f;
		Vector3 up = Vector3::UnitZ;

		Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
		GetGame()->GetRenderer()->SetViewMatrix(view);
	}

	void CameraActor::ActorInput(const uint8_t* keys)
	{
		float forwardSpeed = 0.0f;
		float angularSpeed = 0.0f;

		if (keys[SDL_SCANCODE_W])
		{
			forwardSpeed += 300.f;
		}
		if (keys[SDL_SCANCODE_S])
		{
			forwardSpeed -= 300.f;
		}
		if (keys[SDL_SCANCODE_A])
		{
			angularSpeed -= CustomMath::TwoPi;
		}
		if (keys[SDL_SCANCODE_D])
		{
			angularSpeed += CustomMath::TwoPi;
		}

		m_MoveComp->SetForwardSpeed(forwardSpeed);
		m_MoveComp->SetAngularSpeed(angularSpeed);
	}
}