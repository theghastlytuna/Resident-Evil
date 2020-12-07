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

	//make level
	{
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
		Scene::CreatePlatform("Table_down.png", 40, 70, -100, -10, 0, 0, 45);//obstacle centre left table
		Scene::CreatePlatform("Table_down.png", 40, 70, 100, -10, 0, 0, -45);//obstacle centre right table
		Scene::CreatePlatform("table.png", 120, 100, 0, -100, 50, 30, 0);//obstacle centre bottom table
		Scene::CreatePlatform("Statue_1.png", 50, 50, 300, 300, 0, 0, 165);//obstacle north east box
		Scene::CreatePlatform("BookShelf_1.png", 120, 30, -285, 300, 0, 0, 25);//obstacle north west bookshelf
		Scene::CreatePlatform("BookShelf_1.png", 70, 30, -140, 320, 0, 0, 180);//obstacle north west bookshelf
		Scene::CreatePlatform("BookShelf_1.png", 70, 30, -70, 320, 0, 0, 0);//obstacle north west bookshelf
		Scene::CreatePlatform("BookShelf_1.png", 70, 30, 40, 300, 0, 0, 90);//obstacle north bookshelf
		Scene::CreatePlatform("BookShelf_1.png", 120, 30, -275, -300, 0, 0, 45);//obstacle south west table
		Scene::CreatePlatform("BookShelf_1.png", 70, 30, -240, -250, 0, 0, 150);//obstacle south west table
		Scene::CreatePlatform("Statue_1.png", 100, 100, 250, -250, 0, 0, 75);//obstacle south east statue
		Scene::CreatePlatform("Table_down.png", 40, 70, 300, 360, 0, 0, 0);//obstacle north east wall
		Scene::CreatePlatform("Table_down.png", 40, 70, 360, 110, 0, 0, 30);//obstacle east table
		Scene::CreatePlatform("Bookshelf_1.png", 70, 30, 310, -10, 0, 0, 30);//obstacle east bookshelf
		Scene::CreatePlatform("Bookshelf_1.png", 70, 30, -360, 0, 0, 0, 0);//obstacle west bookshelf
	}

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
		
		ECS::SetIsMainPlayer(entity, true);

		//Add components
		ECS::AttachComponent<Player>(entity);
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Health>(entity);
		ECS::AttachComponent<Ammo>(entity);

		//set components
		std::string fileName = "top_downfull_body.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 50, 50);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 30.f));
		ECS::GetComponent<Health>(entity).health = 50;

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 35.f;

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

	//Health Bar
	{
		//Create health bar
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainUI(entity, true);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//set components
		std::string fileName = "healthBarGreen.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 90, 30);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1000.f, 1000.f, 45.f));
	}

	//Ammo Bar
	{
		//Create ammo counter
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainAmmoUI(entity, true);



		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//set components
		std::string fileName = "healthBarGreen.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 90, 30);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1000.f, 1000.f, 45.f));
		ECS::GetComponent<Transform>(entity).SetRotationAngleZ(-1.5707);
	}

	//Game over Kiryu
	{
		auto entity = ECS::CreateEntity();



		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//set components
		std::string fileName = "kiryu.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 200, 200);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1000.f, 975.f, 40.f));
	}

	//Game over text
	{
		auto entity = ECS::CreateEntity();



		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//set components
		std::string fileName = "gameOverText.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 220, 80);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1000.f, 925.f, 41.f));
	}

	//game over rigged text bubble
	{
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//set components
		std::string fileName = "riggedText.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 150, 150);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(910.f, 1100.f, 41.f));
	}

	//game over camera target
	{
		auto entity = ECS::CreateEntity();
		gameOver = entity;

		//Add components
		ECS::AttachComponent<Transform>(entity);

		//set components

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1000.f, 995.f, 40.f));
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
	{
		if (zombieSpawning && activeZombies.size() <= 8 && Timer::time > spawnTimer)
		{
			activeZombies.push_back(Scene::ZombieSpawn(spawners));
			srand(time(NULL));
			spawnTimer = 5 + (rand() % 5 - 3);
			Timer::Reset();
		}
	}

	////check for valid spawned zombies
	//if (spawnedZombie > -1)
	//{
	//	activeZombies.push_back(spawnedZombie);
	//	for (int i = 0; i < activeZombies.size(); i++)
	//	{
	//		std::cout << activeZombies[i] << std::endl;
	//	}
	//}

	//check for dead zombies
	{
		for (int i = 0; i < activeZombies.size(); i++)
		{
			if (ECS::GetComponent<Health>(activeZombies[i]).health <= 1)
			{
				if (ECS::GetComponent<DropAmmo>(activeZombies[i]).hasAmmo == true)
				{
					ammoPickupStorage.push_back(Scene::CreateAmmoPickup(
						ECS::GetComponent<PhysicsBody>(activeZombies[i]).GetPosition().x, ECS::GetComponent<PhysicsBody>(activeZombies[i]).GetPosition().y));
				}
				PhysicsBody::m_bodiesToDelete.push_back(activeZombies[i]);
				activeZombies.erase(activeZombies.begin() + i);
			}
		}
	}
	
	Follow();//make zombies move

	//Check for collided bullets
	{
		for (int i = 0; i < bulletStorage.size(); i++)
		{
			if (ECS::GetComponent<BulletCollide>(bulletStorage[i]).collided == true)
			{
				PhysicsBody::m_bodiesToDelete.push_back(bulletStorage[i]);
				//int* r =(int*)bulletStorage[i];
				bulletStorage.erase(bulletStorage.begin() + i);
			}
		}
	}

	//Check for collided ammo pickups
	{
		for (int i = 0; i < ammoPickupStorage.size(); i++)
		{
			if (ECS::GetComponent<Ammo>(ammoPickupStorage[i]).ammoCollided == true)
			{
				PhysicsBody::m_bodiesToDelete.push_back(ammoPickupStorage[i]);
				ammoPickupStorage.erase(ammoPickupStorage.begin() + i);
			}
		}
	}

	//Check player health bar
	{
		ECS::GetComponent<Transform>(MainEntities::MainUI()).SetPosition(vec3(player.GetPosition().x - 200, player.GetPosition().y + 130, 40.f));

		if (ECS::GetComponent<Health>(MainEntities::MainPlayer()).health < 20)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainUI()).LoadSprite(deadBar, 120, 30, false);
		}
		else if (ECS::GetComponent<Health>(MainEntities::MainPlayer()).health < 30)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainUI()).LoadSprite(redBar, 120, 30, false);
		}
		else if (ECS::GetComponent<Health>(MainEntities::MainPlayer()).health < 40)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainUI()).LoadSprite(orangeBar, 120, 30, false);
		}
		else if (ECS::GetComponent<Health>(MainEntities::MainPlayer()).health < 50)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainUI()).LoadSprite(yellowBar, 120, 30, false);
		}
		else if (ECS::GetComponent<Health>(MainEntities::MainPlayer()).health == 50)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainUI()).LoadSprite(greenBar, 120, 30, false);
		}
	}

	//Check player ammo bar
	{
		ECS::GetComponent<Transform>(MainEntities::MainAmmoUI()).SetPosition(vec3(player.GetPosition().x - 198, player.GetPosition().y + 100, 40.f));

		if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo < 10)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainAmmoUI()).LoadSprite(ammoBar0, 15, 120, false);
		}
		else if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo < 20)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainAmmoUI()).LoadSprite(ammoBar1, 15, 120, false);
		}
		else if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo < 30)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainAmmoUI()).LoadSprite(ammoBar2, 15, 120, false);
		}
		else if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo < 40)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainAmmoUI()).LoadSprite(ammoBar3, 15, 120, false);
		}
		else if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo < 50)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainAmmoUI()).LoadSprite(ammoBar4, 15, 120, false);
		}
		else if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo <= 50)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainAmmoUI()).LoadSprite(ammoBar5, 15, 120, false);
		}
		else if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo <= 60)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainAmmoUI()).LoadSprite(ammoBar6, 15, 120, false);
		}
	}

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));

	if (ECS::GetComponent<Health>(MainEntities::MainPlayer()).health <= 0)
	{
		ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(gameOver));
		ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(gameOver));
	}
}

void ResidentEvil::Follow()
{
	for (int i = 0; i < activeZombies.size(); i++)
	{

		auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
		auto& enemy = ECS::GetComponent<PhysicsBody>(activeZombies[i]);

		vec2 total = vec2(player.GetPosition().x - enemy.GetPosition().x, player.GetPosition().y - enemy.GetPosition().y);
		float length = (sqrt((total.x * total.x) + (total.y * total.y)));
		vec2 normal = vec2(total.x / length, total.y / length);

		//enemy.GetBody()->ApplyForceToCenter(b2Vec2(normal.x * 3000.f, normal.y * 3000.f ),true);
		enemy.GetBody()->SetLinearVelocity(b2Vec2(normal.x * 15.f, normal.y * 15.f));

		b2Vec2 baseVector = b2Vec2(1, 0);//0 degrees

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
			player.SetRotationAngleDeg(player.GetRotationAngleDeg() + 1.5f);
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
			player.SetRotationAngleDeg(player.GetRotationAngleDeg() - 1.5f);
			return;
		}
		player.GetBody()->ApplyForceToCenter(b2Vec2(300000.f, 0.f), true);
		player.SetRotationAngleDeg(0.f);
	}


}

void ResidentEvil::KeyboardDown()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());

	//if (Input::GetKeyDown(Key::N))
	//{
	//	ammoEntity = Scene::CreateAmmoPickup(-40.f, 0.f);
	//	ammoPickupStorage.push_back(ammoEntity);
	//}

	//if (Input::GetKeyDown(Key::T))
	//{
	//	PhysicsBody::SetDraw(!PhysicsBody::GetDraw());
	//}

	if (Input::GetKeyDown(Key::W) && Input::GetKey(Key::Space))
	{
		if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo > 0)
		{
			bulletEntity = Scene::CreateBullet(player.GetBody()->GetPosition().x, player.GetBody()->GetPosition().y);
			bulletStorage.push_back(bulletEntity);
			ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo -= 1;
			std::cout << "Ammo count: " << ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo << std::endl;
		}
	}

	//if (Input::GetKeyDown(Key::J))
	//{
	//	if (zombieSpawning == false)
	//		zombieSpawning = true;
	//	else
	//		zombieSpawning = false;
	//}

	//if (Input::GetKeyDown(Key::K))//DELETE ALL ZOMBIES
	//{
	//	for (int i = 0; i < activeZombies.size(); i++)
	//	{
	//		PhysicsBody::m_bodiesToDelete.push_back(activeZombies[i]);
	//	}
	//	activeZombies.clear();

	//}
}

void ResidentEvil::KeyboardUp()
{
	if (Input::GetKeyUp(Key::S))
	{
		flipped = false;
	}
}