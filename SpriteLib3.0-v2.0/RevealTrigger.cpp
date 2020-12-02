#include "RevealTrigger.h"
#include "ECS.h"

void RevealTrigger::OnEnter()
{
	Trigger::OnEnter();

	for (int i = 0; i < m_targetEntities.size(); i++)
	{
		ECS::GetComponent<Sprite>(m_targetEntities[i]).SetTransparency(1.f);
	}
}

void RevealTrigger::OnExit()
{

}

void RevealTrigger::OnHold()
{

}
