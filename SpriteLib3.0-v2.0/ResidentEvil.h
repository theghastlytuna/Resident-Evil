#pragma once
#include "Scene.h"

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
	bool flipped = false;
};
