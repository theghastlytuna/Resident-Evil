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

	//enemy
	{
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Health>(entity);

		//set components
		std::string fileName = "zombie_top_down.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 50, 50);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 2.f));
		ECS::GetComponent<Health>(entity).health = 50;

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 30.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(60.f), float32(0.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), vec2(0.f, 0.f), false, 
				ENEMY, PLAYER | OBJECTS | GROUND | ENVIRONMENT, 0.5f, 1.2f); //circle body

		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));

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
		Scene::ZombieSpawn();
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
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	//auto& bulletEntity = ECS::GetComponent<PhysicsBody>(bulletPhyBody);

	if (Input::GetKeyDown(Key::T))
	{
		PhysicsBody::SetDraw(!PhysicsBody::GetDraw());
	}

	if (Input::GetKeyDown(Key::W) && Input::GetKey(Key::Space))
	{
		//m_physicsWorld->CreateBody(bulletDef)
		//bulletEntity.SetPosition(player.GetPosition().x, player.GetPosition().y);

			//Create Bullet Entity
		{
			auto bulletEntity = ECS::CreateEntity();
			//auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());

			//Adding Components
			ECS::AttachComponent<Sprite>(bulletEntity);
			ECS::AttachComponent<Transform>(bulletEntity);
			ECS::AttachComponent<PhysicsBody>(bulletEntity);


			std::string fileName = "whiteBall.png";
			ECS::GetComponent<Sprite>(bulletEntity).LoadSprite(fileName, 5, 5);
			ECS::GetComponent<Sprite>(bulletEntity).SetTransparency(1.f);
			ECS::GetComponent<Transform>(bulletEntity).SetPosition(vec3(player.GetPosition().x, player.GetPosition().y, 10.f));

			auto& bulletSpr = ECS::GetComponent<Sprite>(bulletEntity);
			auto& bulletPhsBody = ECS::GetComponent<PhysicsBody>(bulletEntity);

			float shrinkX = 5.f;

			b2Body* bulletBody;
			b2BodyDef bulletDef;
			bulletDef.type = b2_dynamicBody;
			bulletDef.position.Set(player.GetPosition().x, player.GetPosition().y);

			bulletBody = m_physicsWorld->CreateBody(&bulletDef);

			bulletPhsBody = PhysicsBody(bulletEntity, bulletBody, float(bulletSpr.GetWidth() - shrinkX), vec2(0.f, 0.f), false,
				ENEMY , OBJECTS | PICKUP | TRIGGER | GROUND | ENVIRONMENT, 0.35f, 1.2f); //circle body

			bulletBody->SetFixedRotation(true);
			bulletPhsBody.SetRotationAngleDeg(0.f);
			bulletPhsBody.SetColor(vec4(0.f, 1.f, 6.f, 0.3f));
		}
		
		
	}

	if (Input::GetKeyDown(Key::J))
	{
		if (zombieSpawning == false)
			zombieSpawning = true;
		else
			zombieSpawning = false;
		Scene::CreateZombie("zombie_top_down.png", 50, 50, 0, 0, 30, 0);
		//Scene::CreateObjectBox("boxSprite.jpg", 40, 40, 0, 0, 0, 0);
		//ECS::GetComponent<Player>(MainEntities::MainPlayer()).ReassignComponents(ECS::GetComponent<PhysicsBody*>(MainEntities::MainPlayer()));
	}
}

void ResidentEvil::KeyboardUp()
{
	if (Input::GetKeyUp(Key::S))
	{
		flipped = false;
	}
}
