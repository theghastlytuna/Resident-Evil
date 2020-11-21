#include "ScaleTrigger.h"

void ScaleTrigger::OnEnter()
{
	Trigger::OnEnter();

	triggered = true;
}

void ScaleTrigger::OnExit()
{
	Trigger::OnExit();

	triggered = false;
}

void ScaleTrigger::OnHold()
{
	if (!triggered)
	{
		return;
	}

	for (int i = 0; i < m_targetEntities.size(); i++)
	{
		ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).ScaleBody(0.04f, 0, true);
		ECS::GetComponent<Sprite>(m_targetEntities[i]).SetWidth(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetWidth());
		ECS::GetComponent<Sprite>(m_targetEntities[i]).SetHeight(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetHeight());
	}
}