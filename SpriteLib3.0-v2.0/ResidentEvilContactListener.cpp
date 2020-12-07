#include "ResidentEvilContactListener.h"
#include "ECS.h"
#include "ResidentEvil.h"
#include <vector>
#include "Utilities.h"

ResidentEvilContactListener::ResidentEvilContactListener()
	: b2ContactListener()
{

}

void ResidentEvilContactListener::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();

	int* moveBullet;

	//if neither or both are sensors, will be false
	if ((sensorA ^ sensorB))
	{
		if (sensorA)
		{
			TriggerEnter(fixtureA);
		}
		else if (sensorB)
		{
			TriggerEnter(fixtureB);
		}
	}

	b2Filter filterA = fixtureA->GetFilterData();
	b2Filter filterB = fixtureB->GetFilterData();

	//Player and enemy contact
	if ((filterA.categoryBits == PLAYER && filterB.categoryBits == ENEMY) || (filterB.categoryBits == PLAYER && filterA.categoryBits == ENEMY))
	{
		if (filterA.categoryBits == PLAYER)
		{
			ECS::GetComponent<Health>((int)fixtureA->GetBody()->GetUserData()).health -= 10;
			
		}
		else if (filterB.categoryBits == PLAYER)
		{
			ECS::GetComponent<Health>((int)fixtureB->GetBody()->GetUserData()).health -= 10;

		}
	}

	//Bullet and enemy contact
	if ((filterA.categoryBits == FRIENDLY && filterB.categoryBits == ENEMY) || (filterB.categoryBits == FRIENDLY && filterA.categoryBits == ENEMY))
	{
		if (filterA.categoryBits == ENEMY)
		{
			ECS::GetComponent<Health>((int)fixtureA->GetBody()->GetUserData()).health -= 10;
			ECS::GetComponent<BulletCollide>((int)fixtureB->GetBody()->GetUserData()).collided = true;

		}
		else if (filterB.categoryBits == ENEMY)
		{
			ECS::GetComponent<Health>((int)fixtureB->GetBody()->GetUserData()).health -= 10;
			ECS::GetComponent<BulletCollide>((int)fixtureA->GetBody()->GetUserData()).collided = true;
			
		}
	}
	//Player and ammo pickup contact
	if ((filterA.categoryBits == PICKUP && filterB.categoryBits == PLAYER) || (filterB.categoryBits == PICKUP && filterA.categoryBits == PLAYER))
	{
		if (filterA.categoryBits == PLAYER && ECS::GetComponent<Ammo>((int)fixtureA->GetBody()->GetUserData()).ammo != 60)
		{
			if ((ECS::GetComponent<Ammo>((int)fixtureA->GetBody()->GetUserData()).ammo + 10) > 60) 
			{
				ECS::GetComponent<Ammo>((int)fixtureA->GetBody()->GetUserData()).ammo = 60;
			}
			else
			{
				ECS::GetComponent<Ammo>((int)fixtureA->GetBody()->GetUserData()).ammo += 10;
			}
			std::cout << "Ammo count: " << ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo << std::endl;
			ECS::GetComponent<Ammo>((int)fixtureB->GetBody()->GetUserData()).ammoCollided = true;
		}
		else if (filterB.categoryBits == PLAYER && ECS::GetComponent<Ammo>((int)fixtureB->GetBody()->GetUserData()).ammo != 60)
		{
			if ((ECS::GetComponent<Ammo>((int)fixtureB->GetBody()->GetUserData()).ammo + 10) > 60)
			{
				ECS::GetComponent<Ammo>((int)fixtureB->GetBody()->GetUserData()).ammo = 60;
			}
			else
			{
				ECS::GetComponent<Ammo>((int)fixtureB->GetBody()->GetUserData()).ammo += 10;
			}
			std::cout << "Ammo count: " << ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo << std::endl;
			ECS::GetComponent<Ammo>((int)fixtureA->GetBody()->GetUserData()).ammoCollided = true;
		}
	}
}

void ResidentEvilContactListener::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();

	//if neither or both are sensors, will be false
	if ((sensorA ^ sensorB))
	{
		if (sensorA)
		{
			TriggerExit(fixtureA);
		}
		else if (sensorB)
		{
			TriggerExit(fixtureB);
		}
	}

}

void ResidentEvilContactListener::TriggerEnter(b2Fixture* sensor)
{
	int entity = (int)sensor->GetBody()->GetUserData();

	ECS::GetComponent<Trigger*>(entity)->OnEnter();
}

void ResidentEvilContactListener::TriggerExit(b2Fixture* sensor)
{
	int entity = (int)sensor->GetBody()->GetUserData();

	ECS::GetComponent<Trigger*>(entity)->OnExit();
}
