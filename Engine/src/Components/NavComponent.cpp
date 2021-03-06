
#include "NavComponent.h"
#include "Actor.h"
#include "GameObjects/TowerDefense/Tile.h"

namespace Engine
{
	NavComponent::NavComponent(Actor* owner, int updateOrder) :
		MoveComponent(owner, updateOrder),
		m_NextNode(nullptr)
	{
	}

	void NavComponent::Update(float deltaTime)
	{
		if (m_NextNode)
		{
			Vector3 diff = m_Owner->GetPosition() - m_NextNode->GetPosition();
			if (CustomMath::NearZero(diff.Length(), 2.0f))
			{
				m_NextNode = m_NextNode->GetParent();
				TurnTo(m_NextNode->GetPosition());
				ResetVelocity();
			}
		}

		MoveComponent::Update(deltaTime);
	}

	void NavComponent::StartPath(const Tile* start)
	{
		m_NextNode = start->GetParent();
		TurnTo(m_NextNode->GetPosition());
	}

	void NavComponent::TurnTo(const Vector3& pos)
	{
		Vector3 dir = pos - m_Owner->GetPosition();
		// New angle is just atan2 of this dir vector
		// (Negate y because +y is down on screen
		//m_Owner->SetRotation();
	}
}