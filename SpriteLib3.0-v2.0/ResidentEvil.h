#pragma once
#include "Scene.h"
#include "ResidentEvilContactListener.h"
#include <vector>

class ResidentEvil : public Scene
{
public:
	ResidentEvil(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	void Update() override;
	void Follow();

	//Input overrides
	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;

protected:
	int m_player = 0;
	int m_enemy = 0;
	ResidentEvilContactListener listener;
	bool zombieSpawning = false;
	bool flipped = false;

	spawnerPos spawner0 = { 350, 240 }; // top rigth spawn 
	spawnerPos spawner1 = { -390, 230 }; // top left spawn
	spawnerPos spawner2 = { -380, -180 }; // bottom left spawn
	spawnerPos spawner3 = { 360, 50 }; // rigth spawn
	spawnerPos spawner4 = { -380, 30 }; // bottom right spawn
	spawnerPos spawners[5] = {spawner0, spawner1, spawner2, spawner3, spawner4};
	int spawnedZombie = -1;
	std::vector<int> activeZombies;
};
