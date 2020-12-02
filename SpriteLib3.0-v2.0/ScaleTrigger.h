#pragma once
#include "Trigger.h"
#include "ECS.h"

class ScaleTrigger : public Trigger
{
public:
	void OnEnter() override;
	void OnExit() override;
	void OnHold() override;

	bool grow = true;
	bool triggered = false;
protected:

};

