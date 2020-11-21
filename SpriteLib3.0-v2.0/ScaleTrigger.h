#pragma once
#include "Trigger.h"
#include "ECS.h"

class ScaleTrigger : public Trigger
{
public:
	void OnEnter() override;
	void OnExit() override;
	void OnHold() override;
protected:

	bool triggered = false;
};

