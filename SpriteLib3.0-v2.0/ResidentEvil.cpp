#include "ResidentEvil.h"
#include "Utilities.h"
#include "Timer.h"
#include <math.h>

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
	Scene::CreateFloor("floorbig.png", 400, 400, 0, 0, 0, 0, 0);//floor centre
	Scene::CreateFloor("floorbig.png", 400, 400, 200, 0, 0, 0, 0);//floor west
	Scene::CreateFloor("floorbig.png", 400, 400, 200, 200, 0, 0, 0);//floor north west
	Scene::CreateFloor("floorbig.png", 400, 400, 0, 200, 0, 0, 0);//floor north
	Scene::CreateFloor("floorbig.png", 400, 400, -200, 0, 0, 0, 0);//floor east
	Scene::CreateFloor("floorbig.png", 400, 400, -200, 200, 0, 0, 0);//floor north east
	Scene::CreateFloor("floorbig.png", 400, 400, 0, -200, 0, 0, 0);// floor south
	Scene::CreateFloor("floorbig.png", 400, 400, 200, -200, 0, 0, 0);// floor south west 
	Scene::CreateFloor("floorbig.png", 400, 400, -200, -200, 0, 0, 0);// floor south east 
	Scene::CreatePlatform("black.png", 10, 400, -400, -200, 0, 0, 0);//east south wall
	Scene::CreatePlatform("black.png", 10, 400, -400, 0, 0, 0, 0);//east wall
	Scene::CreatePlatform("black.png", 10, 400, -400, 200, 0, 0, 0);//east north wall
	Scene::CreatePlatform("black.png", 400, 10, -200, 400, 0, 0, 0);//north east wall
	Scene::CreatePlatform("black.png", 400, 10, 0, 400, 0, 0, 0);//north wall
	Scene::CreatePlatform("black.png", 400, 10, 200, 400, 0, 0, 0);//north west wall
	Scene::CreatePlatform("black.png", 10, 400, 400, 200, 0, 0, 0);//west north wall
	Scene::CreatePlatform("black.png", 10, 400, 400, 0, 0, 0, 0);//west wall
	Scene::CreatePlatform("black.png", 10, 400, 400, -200, 0, 0, 0);//west south wall
	Scene::CreatePlatform("black.png", 400, 10, 200, -400, 0, 0, 0);//south west wall
	Scene::CreatePlatform("black.png", 400, 10, 0, -400, 0, 0, 0);//south wall
	Scene::CreatePlatform("black.png", 400, 10, -200, -400, 0, 0, 0);//south east wall
	Scene::CreatePlatform("boxSprite.jpg", 100, 20, -100, -10, 0, 0, 45);//obstacle centre left rectangle
	Scene::CreatePlatform("boxSprite.jpg", 100, 20, 100, -10, 0, 0, -45);//obstacle centre right rectangle
	Scene::CreatePlatform("boxSprite.jpg", 50, 50, 300, 300, 0, 0, -15);//obstacle north east box
	Scene::CreatePlatform("boxSprite.jpg", 120, 10, -275, 300, 0, 0, 25);//obstacle north west rectangle
	Scene::CreatePlatform("boxSprite.jpg", 120, 10, -275, -300, 0, 0, 45);//obstacle south west rectangle
	Scene::CreatePlatform("table.png", 120, 100, 0, -100, 70, 30, 0);//obstacle centre bottom table
	Scene::CreatePlatform("boxSprite.jpg", 100, 100, 250, -250, 0, 0, 75);//obstacle south east box
	Scene::CreatePlatform("boxSprite.jpg", 70, 10, -240, -250, 0, 0, -30);//obstacle south west rectangle 
	Scene::CreatePlatform("boxSprite.jpg", 70, 10, -140, 340, 0, 0, 0);//obstacle north west rectangle
	Scene::CreatePlatform("boxSprite.jpg", 70, 10, 300, 360, 0, 0, 90);//obstacle north east wall
	Scene::CreatePlatform("boxSprite.jpg", 70, 20, -360, 0, 0, 0, 0);//obstacle west wall
	Scene::CreatePlatform("boxSprite.jpg", 70, 20, 310, 0, 0, 0, 30);//obstacle east rectangle
	activeZombies.push_back(Scene::CreateZombie("zombie_top_down.png", 50, 50, 0, 100, 30, 0));
	activeZombies.push_back(Scene::CreateZombie("zombie_top_down.png", 50, 50, 100, 0, 30, 0));
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

	//call zombie spawner
	if (zombieSpawning)
	{
		spawnedZombie = Scene::ZombieSpawn(spawners);
	}

	//check for valid spawned zombies
	if (spawnedZombie > -1)
	{
		activeZombies.push_back(spawnedZombie);
		for (int i = 0; i < activeZombies.size(); i++)
		{
			std::cout << activeZombies[i] << std::endl;
		}
	}

	//check for dead zombies
	{
		for (int i = 0; i < activeZombies.size(); i++)
		{
			if (ECS::GetComponent<Health>(activeZombies[i]).health <= 1)
			{
				PhysicsBody::m_bodiesToDelete.push_back(activeZombies[i]);
			}
		}
	}

	//change zombie rotation
	{
		b2Vec2 baseVector = b2Vec2(1, 0);//0 degrees

		for (int i = 0; i < activeZombies.size(); i++)
		{
			b2Vec2 playerPos = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer()).GetPosition();
			b2Vec2 enemyPos = ECS::GetComponent<PhysicsBody>(activeZombies[i]).GetPosition();
			b2Vec2 vectorToPlayer = playerPos - enemyPos;

			float dot = (baseVector.x * vectorToPlayer.x + baseVector.y * vectorToPlayer.y);
			float angle = acos(dot / (vectorToPlayer.Length() * baseVector.Length()));
			if (vectorToPlayer.y >= 0)
			{
				ECS::GetComponent<PhysicsBody>(activeZombies[i]).SetRotationAngleDeg(angle * (180 / PI));
			}
			else if (vectorToPlayer.y < 0)
			{
				ECS::GetComponent<PhysicsBody>(activeZombies[i]).SetRotationAngleDeg(-angle * (180 / PI));
			}
		}
	}
	
	Follow();
	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
}

void ResidentEvil::Follow()
{
	for (int a = 0; a < activeZombies.size(); a++)
	{

		auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
		auto& enemy = ECS::GetComponent<PhysicsBody>(activeZombies[a]);

		vec2 total = vec2(player.GetPosition().x - enemy.GetPosition().x, player.GetPosition().y - enemy.GetPosition().y);
		float length = (sqrt((total.x * total.x) + (total.y * total.y)));
		vec2 normal = vec2(total.x / length, total.y / length);
		
		//enemy.GetBody()->ApplyForceToCenter(b2Vec2(normal.x * 3000.f, normal.y * 3000.f ),true);
		enemy.GetBody()->SetLinearVelocity(b2Vec2(normal.x * 15.f, normal.y * 15.f));


	}
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

	if (Input::GetKeyDown(Key::K))//DELETE ALL ZOMBIES
	{
		for (int i = 0; i < activeZombies.size(); i++)
		{
			PhysicsBody::m_bodiesToDelete.push_back(activeZombies[i]);
		}
		activeZombies.clear();
	}
}

void ResidentEvil::KeyboardUp()
{
	if (Input::GetKeyUp(Key::S))
	{
		flipped = false;
	}
}
