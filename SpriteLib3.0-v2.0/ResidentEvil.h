#pragma once
#include "Scene.h"
#include "ResidentEvilContactListener.h"

class ResidentEvil : public Scene
{
public:
	ResidentEvil(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	void Update() override;

	//Input overrides
	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;

protected:
	ResidentEvilContactListener listener;

	bool flipped = false;
};
