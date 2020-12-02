#include "PhysicsPlayground.h"
#include "Utilities.h"
#include "Transform.h"

#include <random>

PhysicsPlayground::PhysicsPlayground(std::string name)
	: Scene(name)
{
	m_gravity = b2Vec2(0.f, -90.f);
	m_physicsWorld->SetGravity(m_gravity);

	m_physicsWorld->SetContactListener(&listener);
}

void PhysicsPlayground::InitScene(float windowWidth, float windowHeight)
{
	//Dynamically allocates the register
	m_sceneReg = new entt::registry;

	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;

	//Setup MainCamera Entity
	{
		/*Scene::CreateCamera(m_sceneReg, vec4(-75.f, 75.f, -75.f, 75.f), -100.f, 100.f, windowWidth, windowHeight, true, true);*/

		//Creates Camera entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainCamera(entity, true);

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		ECS::AttachComponent<HorizontalScroll>(entity);
		ECS::AttachComponent<VerticalScroll>(entity);

		vec4 temp = vec4(-140.f, 140.f, -140.f, 140.f);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		//Attaches the camera to vert and horiz scrolls
		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
	}


	//Player entity
	{
		/*Scene::CreatePhysicsSprite(m_sceneReg, "LinkStandby", 80, 60, 1.f, vec3(0.f, 30.f, 2.f), b2_dynamicBody, 0.f, 0.f, true, true)*/

		auto entity = ECS::CreateEntity();
		ECS::SetIsMainPlayer(entity, true);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<CanJump>(entity);

		//Sets up the components
		std::string fileName = "kiryu.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 40, 40);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 2.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(10.f), float32(100.f));//start point 10, 100

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.35f, 1.2f); //square body
		//tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetHeight() - shrinkY)/2.f), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f); //round body

		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetFixedRotation(true);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
		tempPhsBody.SetGravityScale(1.f);
	}

	//CHAMBER 1
	{
		Scene::CreatePlatform("black.png", 400, 10, -15.f, 150.f, 0.f, 0.f, 90.f);//left wall
		Scene::CreatePlatform("black.png", 800, 10, 390.f, -50.f, 0.f, 0.f, 0.f);//main floor
		Scene::CreatePlatform("black.png", 800, 10, 390.f, 250.f, 0.f, 0.f, 0.f);//main roof
		Scene::CreatePlatform("black.png", 200, 10, 455.f, 40.f, 0.f, 0.f, 90.f);//right wall
		Scene::CreatePlatform("black.png", 50, 70, 15.f, -10.f, 0.f, 0.f, 0.f);//left platform
		Scene::CreatePlatform("black.png", 55, 70, 430.f, -10.f, 0.f, 0.f, 0.f);//right platform
		Scene::CreatePlatform("black.png", 180, 5, 260.f, 70.f, 0.f, 0.f, 0.f);//puzzle floor
		Scene::CreatePlatform("black.png", 350, 5, 340.f, 140.f, 0.f, 0.f, 0.f);//puzzle roof
		Scene::CreatePlatform("black.png", 25, 60, 190.f, 80.f, 0.f, 0.f, 90.f);//puzzle left wall bottom
		Scene::CreatePlatform("black.png", 45, 60, 190.f, 120.f, 0.f, 0.f, 90.f);//puzzle left wall top

		puzzle1Object1 = Scene::CreateObjectBox("white.png", 120, 10, 220.f, 110.f, 0.f, 0.f);//puzzle moveable
		ECS::GetComponent<PhysicsBody>(puzzle1Object1).SetFixedRotation(true);
	}

	//CHAMBER 2
	{
		Scene::CreatePlatform("black.png", 1000, 10, 1200.f, 290.f, 0.f, 0.f, 0.f);//main roof
		Scene::CreatePlatform("black.png", 420, 5, 715.f, 140.f, 0.f, 0.f, 0.f);//connector floor
		Scene::CreatePlatform("black.png", 450, 5, 1200.f, 140.f, 0.f, 0.f, 0.f);//main floor
		Scene::CreatePlatform("black.png", 25, 5, 925.f, 130.f, 0.f, 0.f, 90.f);//trigger holder left
		Scene::CreatePlatform("black.png", 50, 5, 950.f, 115.f, 0.f, 0.f, 0.f);//trigger holder bottom
		Scene::CreatePlatform("black.png", 25, 5, 975.f, 130.f, 0.f, 0.f, 90.f);//trigger holder right

		puzzle2Object1 = Scene::CreatePlatform("black.png", 140, 10, 1100.f, 210.f, 0.f, 0.f, 90.f);//spinny thing 1
		puzzle2Object2 = Scene::CreatePlatform("black.png", 140, 10, 1350.f, 210.f, 0.f, 0.f, 0.f);//spinny thing 2
		puzzle2Object3 = Scene::CreateObjectBox("white.png", 25, 25, 1225.f, 160.f, 0.f, 0.f);//box
		puzzle2Trigger1 = Scene::CreateRotationTrigger(40, 10, 950.f, 120.f, puzzle2Object1, true, puzzle2Object2);//spinny trigger
	}

	//CHAMBER 3
	{
		Scene::CreatePlatform("black.png", 450, 10, 1420.f, -85.f, 0.f, 0.f, 90.f);//left wall
		Scene::CreatePlatform("black.png", 450, 10, 2100.f, -85.f, 0.f, 0.f, 90.f);//right wall
		Scene::CreatePlatform("black.png", 550, 10, 1880.f, -170.f, 0.f, 0.f, 0.f);//main floor
		Scene::CreatePlatform("black.png", 60, 10, 2070.f, -110.f, 0.f, 0.f, 0.f);//right platform
		Scene::CreatePlatform("black.png", 500, 10, 1665.f, -80.f, 0.f, 0.f, 0.f);//puzzle roof
		Scene::CreatePlatform("black.png", 10, 20, 1500.f, -70.f, 0.f, 0.f, 0.f);//trigger holder middle
		Scene::CreatePlatform("black.png", 10, 20, 1575.f, -70.f, 0.f, 0.f, 0.f);//trigger holder right
		Scene::CreatePlatform("black.png", 5, 35, 1942.f, -70.f, 0.f, 0.f, 0.f);//door holder top
		Scene::CreatePlatform("black.png", 20, 10, 1942.f, -80.f, 0.f, 0.f, 0.f);//door holder top 2? i guess?
		Scene::CreatePlatform("black.png", 20, 5, 1915.f, -160.f, 0.f, 0.f, -45.f);//ball catch
		Scene::CreatePlatform("black.png", 5, 10, 1942.f, -165.f, 0.f, 0.f, 0.f);//door holder bottom


		Scene::CreateObjectBox("white.png", 10, 80, 1950.f, -120.f, 0.f, 0.f);
		puzzle3Object1 = Scene::CreateObjectBall("whiteBall.png", 40, 40, 1700.f, -75.f, 0.f, 0.f);//ball
		puzzle3Trigger1 = Scene::CreateScaleTrigger(50, 10, 1460.f, -75.f, puzzle3Object1, false);//shrink trigger
		puzzle3Trigger2 = Scene::CreateScaleTrigger(50, 10, 1540.f, -75.f, puzzle3Object1, true);//grow trigger
	}

	//CHAMBER 4 (WIN)
	{
		Scene::CreatePlatform("black.png", 700, 10, 1750.f, -300.f, 0.f, 0.f, 0.f);//main floor

		//Setup taco bell
		{
			/*Scene::CreateSprite(m_sceneReg, "HelloWorld.png", 100, 60, 0.5f, vec3(0.f, 0.f, 0.f));*/

			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);

			//Set up the components
			std::string fileName = "taco bell.jpg";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 180, 110);
			ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(2000.f, -240.f, 0.f));
		}

		//Setup congratulations
		{
			//Creates entity
			auto entity = ECS::CreateEntity();
			winSprite1 = entity;

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);

			//Set up the components
			std::string fileName = "congratulations.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 150, 40);
			ECS::GetComponent<Sprite>(entity).SetTransparency(0.f);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(1800.f, -240.f, 0.f));
		}

		Scene::CreateRevealTrigger(180, 110, 2000.f, -240.f, winSprite1, false);
	}

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
}

void PhysicsPlayground::Update()
{
	Scene::AdjustScrollOffset();
	ECS::GetComponent<Trigger*>(puzzle2Trigger1)->OnHold();
	ECS::GetComponent<Trigger*>(puzzle3Trigger1)->OnHold();
	ECS::GetComponent<Trigger*>(puzzle3Trigger2)->OnHold();
}

void PhysicsPlayground::KeyboardHold()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	auto& canJump = ECS::GetComponent<CanJump>(MainEntities::MainPlayer());

	float speed = 1.f;
	b2Vec2 vel = b2Vec2(0.f, 0.f);

	if (Input::GetKey(Key::Shift))
	{

	}

	if (Input::GetKey(Key::A))
	{
		player.GetBody()->ApplyForceToCenter(b2Vec2(-300000.f * speed, 0.f), true);
	}
	if (Input::GetKey(Key::D))
	{
		player.GetBody()->ApplyForceToCenter(b2Vec2(300000.f * speed, 0.f), true);
	}

	//Change physics body size for circle
	if (Input::GetKey(Key::O))
	{
		player.ScaleBody(1.3 * Timer::deltaTime, 0);
	}
	else if (Input::GetKey(Key::I))
	{
		player.ScaleBody(-1.3 * Timer::deltaTime, 0);
	}
}

void PhysicsPlayground::KeyboardDown()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	auto& canJump = ECS::GetComponent<CanJump>(MainEntities::MainPlayer());

	if (Input::GetKeyDown(Key::T))
	{
		PhysicsBody::SetDraw(!PhysicsBody::GetDraw());
	}
	if (canJump.m_canJump)
	{
		if (Input::GetKeyDown(Key::Space))
		{
			player.GetBody()->ApplyLinearImpulseToCenter(b2Vec2(0.f, 450000.f), true);
			canJump.m_canJump = false;
		}
	}

	if (Input::GetKeyDown(Key::Z))
	{
		ECS::GetComponent<Camera>(MainEntities::MainCamera()).Zoom(20.f);
	}
	if (Input::GetKeyDown(Key::X))
	{
		ECS::GetComponent<Camera>(MainEntities::MainCamera()).Zoom(-20.f);
	}

}

void PhysicsPlayground::KeyboardUp()
{
	

}
