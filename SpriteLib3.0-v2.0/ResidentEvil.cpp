#include "ResidentEvil.h"
#include "Utilities.h"
#include "Timer.h"

ResidentEvil::ResidentEvil(std::string name)
	: Scene(name)
{
	m_gravity = b2Vec2(0.f, 0.f);
	m_physicsWorld->SetGravity(m_gravity);

	m_physicsWorld->SetContactListener(&listener);
}

void ResidentEvil::InitScene(float windowWidth, float windowHeight)
{
	m_sceneReg = new entt::registry;
	Scene::SetClearColor(vec4(0.f, 0.f, 0.f, 1.f));
	ECS::AttachRegister(m_sceneReg);

	float aspectRatio = windowWidth / windowHeight;

	//set spawner positions
	{
		spawnerPos spawner0;
		spawnerPos spawner1;
		spawnerPos spawner2;
		spawnerPos spawner3;
		spawnerPos spawner4;

		spawner0.spawnerPosX = 0;
		spawner0.spawnerPosY = 0;
		spawners[0] = spawner0;

		spawner1.spawnerPosX = 50;
		spawner1.spawnerPosY = 0;
		spawners[1] = spawner1;

		spawner2.spawnerPosX = 0;
		spawner2.spawnerPosY = -50;
		spawners[2] = spawner2;

		spawner3.spawnerPosX = -50;
		spawner3.spawnerPosY = 0;
		spawners[3] = spawner3;

		spawner4.spawnerPosX = 0;
		spawner4.spawnerPosY = 50;
		spawners[4] = spawner4;

	}

	//Creates Camera Entity
	{
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainCamera(entity, true);
		ECS::AttachComponent<Camera>(entity);
		ECS::AttachComponent<HorizontalScroll>(entity);
		ECS::AttachComponent<VerticalScroll>(entity);

		vec4 temp = vec4(-150, 150, -150, 150);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100, 100);

		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
	}

	//Setup floor
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Set up the components
		std::string fileName = "FloorBig.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 400, 400);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));
	}

	//main player
	{
		auto entity = ECS::CreateEntity();
		m_player = entity;
		ECS::SetIsMainPlayer(entity, true);

		//Add components
		ECS::AttachComponent<Player>(entity);
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Health>(entity);

		//set components
		std::string fileName = "top_downfull_body.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 50, 50);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 2.f));
		ECS::GetComponent<Health>(entity).health = 50;

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 30.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(0.f), float32(0.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), vec2(0.f, 0.f), false,
			PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER | GROUND | ENVIRONMENT, 0.35f, 1.2f); //circle body

		tempBody->SetFixedRotation(true);
		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

	}

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
}

void ResidentEvil::Update()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());

	Scene::AdjustScrollOffset();

	player.GetBody()->SetLinearVelocity(b2Vec2(player.GetBody()->GetLinearVelocity().x * 0.888f, player.GetBody()->GetLinearVelocity().y * 0.888f));

	if (zombieSpawning)
	{
		Scene::ZombieSpawn(spawners);
	}
	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
}

void ResidentEvil::KeyboardHold()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());


	if (Input::GetKey(Key::W))
	{
		if (Input::GetKey(Key::Space))
		{
			return;
		}

		player.GetBody()->ApplyForceToCenter(b2Vec2(0.f, 300000.f), true);
		player.SetRotationAngleDeg(90.f);
	}
	if (Input::GetKey(Key::A))
	{
		if (Input::GetKey(Key::Space))
		{
			player.SetRotationAngleDeg(player.GetRotationAngleDeg() + 1.f);
			return;
		}
		player.GetBody()->ApplyForceToCenter(b2Vec2(-300000.f, 0.f), true);
		player.SetRotationAngleDeg(180.f);
	}
	if (Input::GetKey(Key::S))
	{
		if (Input::GetKey(Key::Space))
		{
			if (flipped == false)
			{
				player.SetRotationAngleDeg(player.GetRotationAngleDeg() + 180.f);
			}
			flipped = true;
			return;
		}
		player.GetBody()->ApplyForceToCenter(b2Vec2(0.f, -300000.f), true);
		player.SetRotationAngleDeg(270.f);
	}
	if (Input::GetKey(Key::D))
	{
		if (Input::GetKey(Key::Space))
		{
			player.SetRotationAngleDeg(player.GetRotationAngleDeg() - 1.f);
			return;
		}
		player.GetBody()->ApplyForceToCenter(b2Vec2(300000.f, 0.f), true);
		player.SetRotationAngleDeg(0.f);
	}
}

void ResidentEvil::KeyboardDown()
{
	if (Input::GetKeyDown(Key::T))
	{
		PhysicsBody::SetDraw(!PhysicsBody::GetDraw());
	}

	if (Input::GetKeyDown(Key::J))
	{
		if (zombieSpawning == false)
			zombieSpawning = true;
		else
			zombieSpawning = false;
	}
}

void ResidentEvil::KeyboardUp()
{
	if (Input::GetKeyUp(Key::S))
	{
		flipped = false;
	}
}
