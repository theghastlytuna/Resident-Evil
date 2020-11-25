#pragma once

#include "Trigger.h"

class RevealTrigger : public Trigger
{
public:
	void OnEnter() override;
	void OnExit() override;
	void OnHold() override;
	bool triggered = false;
};

