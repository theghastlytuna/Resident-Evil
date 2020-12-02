#include "ScaleTrigger.h"

void ScaleTrigger::OnEnter()
{
	Trigger::OnEnter();

	triggered = true;
	
	ECS::GetComponent<Sprite>(m_triggerEntity).SetTransparency(0.f);

	if (!hold)
	{
		if (grow)
		{
			for (int i = 0; i < m_targetEntities.size(); i++)
			{
				ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).ScaleBody(2.f, 0, true);
				ECS::GetComponent<Sprite>(m_targetEntities[i]).SetWidth(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetWidth());
				ECS::GetComponent<Sprite>(m_targetEntities[i]).SetHeight(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetHeight());
			}
		}
		else if (!grow)
		{
			for (int i = 0; i < m_targetEntities.size(); i++)
			{
				ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).ScaleBody(-2.f, 0, true);
				ECS::GetComponent<Sprite>(m_targetEntities[i]).SetWidth(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetWidth());
				ECS::GetComponent<Sprite>(m_targetEntities[i]).SetHeight(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetHeight());
			}
		}
	}
}

void ScaleTrigger::OnExit()
{
	Trigger::OnExit();

	triggered = false;

	ECS::GetComponent<Sprite>(m_triggerEntity).SetTransparency(1.f);

}

void ScaleTrigger::OnHold()
{
	if (!triggered)
	{
		return;
	}

	if (hold)
	{
		if (grow)
		{
			for (int i = 0; i < m_targetEntities.size(); i++)
			{
				ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).ScaleBody(0.02f, 0, true);
				ECS::GetComponent<Sprite>(m_targetEntities[i]).SetWidth(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetWidth());
				ECS::GetComponent<Sprite>(m_targetEntities[i]).SetHeight(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetHeight());
			}
		}
		else if (!grow)
		{
			for (int i = 0; i < m_targetEntities.size(); i++)
			{
				ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).ScaleBody(-0.02f, 0, true);
				ECS::GetComponent<Sprite>(m_targetEntities[i]).SetWidth(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetWidth());
				ECS::GetComponent<Sprite>(m_targetEntities[i]).SetHeight(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetHeight());
			}
		}
	}
}