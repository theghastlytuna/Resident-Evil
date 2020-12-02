#include "RotationTrigger.h"
#include "Transform.h"
#include "ECS.h"

void RotationTrigger::OnEnter()
{
	Trigger::OnEnter();

	triggered = true;

	ECS::GetComponent<Sprite>(m_triggerEntity).SetTransparency(0.f);

	if (!hold)
	{
		for (int i = 0; i < m_targetEntities.size(); i++)
		{
			ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).SetRotationAngleDeg(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetRotationAngleDeg() + 90.f, true);
		}
	}
}

void RotationTrigger::OnExit()
{
	Trigger::OnExit();

	triggered = false;

	ECS::GetComponent<Sprite>(m_triggerEntity).SetTransparency(1.f);

}

void RotationTrigger::OnHold()
{
	if (!triggered)
	{
		return;
	}

	if (hold)
	{
		for (int i = 0; i < m_targetEntities.size(); i++)
		{
			ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).SetRotationAngleDeg(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetRotationAngleDeg() + 1.f, true);
		}
	}

}
