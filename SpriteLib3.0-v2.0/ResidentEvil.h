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

	//Input overrides
	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;

	std::vector<int> bulletStorage;
	std::vector<int> ammoPickupStorage;

protected:
	int m_player = 0;
	ResidentEvilContactListener listener;
	bool zombieSpawning = false;
	bool flipped = false;

	spawnerPos spawner0 = { 0,0 };
	spawnerPos spawner1 = { 50,0 };
	spawnerPos spawner2 = { 0, -50 };
	spawnerPos spawner3 = { -50, 0 };
	spawnerPos spawner4 = { 0, 50 };
	spawnerPos spawners[5] = {spawner0, spawner1, spawner2, spawner3, spawner4};
	int spawnedZombie = -1;
	std::vector<int> activeZombies;

	int bulletEntity = -1;
	int ammoEntity = -1;
};
