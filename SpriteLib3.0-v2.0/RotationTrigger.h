#pragma once

#include "Trigger.h"

class RotationTrigger : public Trigger
{
public:
	void OnEnter() override;
	void OnExit() override;
	void OnHold() override;
protected:
	int target;
	bool triggered = false;
};

