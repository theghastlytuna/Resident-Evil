#include "CreationTrigger.h"
#include "ECS.h"

void CreationTrigger::OnEnter()
{
	Trigger::OnEnter();

	if (!triggered)
	{
		

		
		triggered = true;
	}
}

void CreationTrigger::OnExit()
{
	Trigger::OnExit();
}

