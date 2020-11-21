#pragma once

#include "Scene.h"
#include "PhysicsPlaygroundListener.h"

class PhysicsPlayground : public Scene
{
public:
	PhysicsPlayground(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	void Update() override;

	//Input overrides
	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;

protected:
	PhysicsPlaygroundListener listener;

	int ball = 0;
	int spinnerTrigger = 0;
	int spinner = 0;
	int scalerTrigger = 0;
	int scaler0 = 0;
	int scaler1 = 0;
	int scaler2 = 0;
};
