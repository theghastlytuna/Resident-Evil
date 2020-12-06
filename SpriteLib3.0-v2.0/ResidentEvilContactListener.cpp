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

	if ((filterA.categoryBits == FRIENDLY && filterB.categoryBits == ENEMY) || (filterB.categoryBits == FRIENDLY && filterA.categoryBits == ENEMY))
	{
		if (filterA.categoryBits == ENEMY)
		{
			ECS::GetComponent<Health>((int)fixtureA->GetBody()->GetUserData()).health -= 10;
			//ECS::GetComponent<PhysicsBody>((int)fixtureB->GetBody()

		}
		else if (filterB.categoryBits == ENEMY)
		{
			ECS::GetComponent<Health>((int)fixtureB->GetBody()->GetUserData()).health -= 10;
			//DestroyTrigger;
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
