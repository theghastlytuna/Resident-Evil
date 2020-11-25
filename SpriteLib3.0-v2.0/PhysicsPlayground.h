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

	int puzzle1Object1 = 0;

	int puzzle2Trigger1 = 0;
	int puzzle2Object1 = 0;
	int puzzle2Object2 = 0;
	int puzzle2Object3 = 0;
	int puzzle3Trigger1 = 0;
	int puzzle3Trigger2 = 0;
	int puzzle3Object1 = 0;

	int winTrigger = 0;
	int winSprite1 = 0;
	int winSprite2 = 0;
};
