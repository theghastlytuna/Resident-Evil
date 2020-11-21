#include "RotationTrigger.h"
#include "Transform.h"
#include "ECS.h"

void RotationTrigger::OnEnter()
{
	Trigger::OnEnter();

	triggered = true;
}

void RotationTrigger::OnExit()
{
	Trigger::OnExit();

	triggered = false;
}

void RotationTrigger::OnHold()
{
	if (!triggered)
	{
		return;
	}

	for (int i = 0; i < m_targetEntities.size(); i++)
	{
		ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).SetRotationAngleDeg(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetRotationAngleDeg() + 1.f, true);
	}
}
