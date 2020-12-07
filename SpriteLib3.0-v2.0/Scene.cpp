#include "Scene.h"
#include "Utilities.h"
#include <time.h>
#include "Matrix.h"
#include <Box2D/Box2D.h>

Scene::Scene(std::string name)
{
	m_physicsWorld = new b2World(m_gravity);
	m_name = name;
}

void Scene::Unload()
{
	if (m_sceneReg != nullptr)
	{
		delete m_sceneReg;
		m_sceneReg = nullptr;
	}

	if (m_physicsWorld != nullptr)
	{
		delete m_physicsWorld;
		m_physicsWorld = nullptr;
	}
}

void Scene::InitScene(float windowWidth, float windowHeight)
{
	//Default scene class init just includes a camera so stuff doesn't immediately crash
	printf("You shouldn't be running this, while initializing this Scene Type.\n");

	//Dynamically allocates the register
	if (m_sceneReg == nullptr)
	{
		m_sceneReg = new entt::registry;
	}

	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;

	//Setup Main Camera entity
	{
		//Creates Camera entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainCamera(entity, true);

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		//ECS::AttachComponent<HorizontalScroll>(entity);
		vec4 temp = vec4(-75.f, 75.f, -75.f, 75.f);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);
	}

	//Setup new Entity
	{
		//Creates entity
		auto entity = ECS::CreateEntity();
		m_helloWorldSign = entity;

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		std::string fileName = "HelloWorld.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 100, 60);
		ECS::GetComponent<Sprite>(entity).SetTransparency(0.5f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));
	}
}

void Scene::Update()
{
	auto& tempSpr = m_sceneReg->get<Sprite>(m_helloWorldSign);
	
	tempSpr.SetTransparency((0.5 * sin(Timer::time * 3.f)) + 0.5f);
}

void Scene::AdjustScrollOffset()
{
	float maxSizeX = ECS::GetComponent<Camera>(MainEntities::MainCamera()).GetOrthoSize().y;
	float maxSizeY = ECS::GetComponent<Camera>(MainEntities::MainCamera()).GetOrthoSize().w;

	float playerHalfSize = ECS::GetComponent<Sprite>(MainEntities::MainPlayer()).GetWidth() / 2.f;

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetOffset((maxSizeX * BackEnd::GetAspectRatio()) - playerHalfSize * 11);
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetOffset(maxSizeY - playerHalfSize* 6);
}

void Scene::CreateCameraEntity(bool mainCamera, float windowWidth, float windowHeight, float left, float right, float bottom, float top, 
									float zNear, float zFar, float aspectRatio, bool vertScroll, bool horizScroll)
{
	//Setup main camera
	{
		//Creates Camera Entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainCamera(entity, false);

		ECS::AttachComponent<Camera>(entity);
		if (horizScroll)
		{
			ECS::AttachComponent<HorizontalScroll>(entity);
		}
		if (vertScroll)
		{
			ECS::AttachComponent<VerticalScroll>(entity);
		}

		vec4 temp = vec4(left, right, bottom, top);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, zNear, zFar);

		if (horizScroll)
		{
			ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		}
		if (vertScroll)
		{
			ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		}
	}
}

int Scene::ZombieSpawn(spawnerPos spawners[])
{
	srand(time(NULL));
	int selectedPos = rand() % 5;
	
	if (Timer::time > 5)
	{
		std::cout << "Spawning at Spawner" << selectedPos << std::endl;
		Timer::Reset();
		return Scene::CreateZombie("zombie_top_down.png", 50, 50, spawners[selectedPos].spawnerPosX, spawners[selectedPos].spawnerPosY, 30, 0);
	}
	return -1;
}

unsigned Scene::HealthBar(int posX, int posY, std::string filename)
{
	//Create health bar
	auto entity = ECS::CreateEntity();
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());



	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//set components
	std::string fileName = filename;
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 100, 50);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posX - 10, posY + 10, 110.f));


	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth()), vec2(0.f, 0.f), false, UI, 0.5f, 1.2f); //circle body

	return entity;
}

unsigned Scene::CreatePlatform(std::string fileName, int spriteX, int spriteY, float posX, float posY, float shrinkX, float shrinkY, float angle)
{
	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Sets up components
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, spriteX, spriteY);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posX, posY, 2.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
		float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY | OBJECTS);
	tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

	tempPhsBody.SetRotationAngleDeg(angle);

	return entity;
}

unsigned Scene::CreateObjectBall(std::string fileName, int spriteX, int spriteY, float posX, float posY, float shrinkX, float shrinkY)
{
	auto entity = ECS::CreateEntity();
	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Sets up the components
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, spriteX, spriteY);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posX, posY, 3.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	//tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false);
	tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetWidth() - shrinkY) / 2.f), vec2(0.f, 0.f), false, OBJECTS, GROUND | ENVIRONMENT | PLAYER | TRIGGER | OBJECTS, 0.3f);

	tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));

	return entity;
}

unsigned Scene::CreateObjectBox(std::string fileName, int spriteX, int spriteY, float posX, float posY, float shrinkX, float shrinkY)
{
	auto entity = ECS::CreateEntity();
	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Sets up the components
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, spriteX, spriteY);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posX, posY, 3.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, OBJECTS, GROUND | ENVIRONMENT | PLAYER | TRIGGER | OBJECTS);

	tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));

	return entity;
}

unsigned Scene::CreateZombie(std::string fileName, int spriteX, int spriteY, float posX, float posY, float shrinkX, float shrinkY)
{
	auto entity = ECS::CreateEntity();
	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Health>(entity);

	//Sets up the components
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, spriteX, spriteY);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posX, posY, 3.f));
	ECS::GetComponent<Health>(entity).health = 50;

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), vec2(0.f, 0.f), false,
		ENEMY, PLAYER | OBJECTS | GROUND | ENVIRONMENT, 0.5f, 1.2f); //circle body

	tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));

	return entity;
}

//Create Bullet Entity
unsigned Scene::CreateBullet(float posX, float posY)
{
	auto entity = ECS::CreateEntity();
	auto player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer()); //Ignore green line; game crashes if you follow it's suggestion.

	//Adding Components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<BulletCollide>(entity);

	//Setting up components
	std::string fileName = "bulletFired.png";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 5, 1);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posX, posY, 100.f));

	auto& bulletSpr = ECS::GetComponent<Sprite>(entity);
	auto& bulletPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	float shrinkX = 0.f;

	b2Body* bulletBody;
	b2BodyDef bulletDef;
	bulletDef.type = b2_dynamicBody;
	float bulletForce = 32000.f;
	float playerAngle = player.GetRotationAngleDeg() * (PI / 180);

	vec2 initialDirection = vec2(13.f, 0.f);
	mat2 rotationMatrix = mat2(vec2(cos(playerAngle), -sin(playerAngle)), vec2(sin(playerAngle), cos(playerAngle)));
	vec2 rotatedDirection = rotationMatrix.operator*(initialDirection);
	bulletDef.position.Set(posX + rotatedDirection.x, posY + rotatedDirection.y);


	bulletBody = m_physicsWorld->CreateBody(&bulletDef);

	bulletPhsBody = PhysicsBody(entity, bulletBody, float(bulletSpr.GetWidth() - shrinkX), vec2(0.f, 0.f), false, FRIENDLY, ENEMY | OBJECTS | ENVIRONMENT, 0.f, 0.f); //circle body

	bulletBody->SetFixedRotation(false);
	bulletPhsBody.SetRotationAngleDeg(player.GetRotationAngleDeg());
	bulletPhsBody.SetColor(vec4(0.f, 1.f, 6.f, 0.3f));


	//bulletPhsBody.ApplyForce(vec3(9999.f, 0.f, 0.f));

	//bulletPhsBody->velocity = 


	bulletBody->ApplyLinearImpulseToCenter(b2Vec2(bulletForce * rotatedDirection.x, bulletForce* rotatedDirection.y), true);

	//std::vector<unsigned> bulletStorage(100);

	//unsigned* bulletAdd = bulletStorage.data();
	//bulletAdd = &entity;

	//PhysicsBody::m_bodiesToDelete.push_back(bulletStorage[]);
	return entity;
}

//Create ammo pickup

unsigned Scene::CreateAmmoPickup(int posX, int posY)
{
	//Create entity
	auto entity = ECS::CreateEntity();

	//Adding components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Ammo>(entity);

	//Setting up components
	std::string fileName = "ammoPickup.png";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 15);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posX, posY, 50.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(posX), float32(posY));

	int shrinkX = 0.f;

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), vec2(0.f, 0.f), false, PICKUP, PLAYER, 0.f, 0.f); //circle body

	tempPhsBody.SetColor(vec4(10.f, 20.f, 1.f, 0.3f));

	return entity;
}

unsigned Scene::CreateDestroyTrigger(int sizeX, int sizeY, float posX, float posY, unsigned int targetEntity0,
		bool isHold, unsigned int targetEntity1, unsigned int targetEntity2)//default values
{
	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Trigger*>(entity);

	//Sets up components
	std::string fileName = "boxSprite.jpg";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, sizeX, sizeY);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posX, posY, 80.f));
	ECS::GetComponent<Trigger*>(entity) = new DestroyTrigger();
	ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
	ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetEntity0);
	if (targetEntity1 != 0)
	{
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetEntity1);
	}
	if (targetEntity2 != 0)
	{
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetEntity2);
	}

	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(sizeX), float(sizeY), vec2(0.f, 0.f), true, TRIGGER, PLAYER | OBJECTS);
	tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));

	return entity;
}

unsigned Scene::CreateRotationTrigger(int sizeX, int sizeY, float posX, float posY, unsigned int targetEntity0,
		bool isHold, unsigned int targetEntity1, unsigned int targetEntity2)//default values
{
	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Trigger*>(entity);

	//Sets up components
	std::string fileName = "Red.png";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, sizeX, sizeY);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posX, posY, 80.f));
	ECS::GetComponent<Trigger*>(entity) = new RotationTrigger();
	ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
	ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetEntity0);
	RotationTrigger* temp = (RotationTrigger*)ECS::GetComponent<Trigger*>(entity);
	temp->hold = isHold;
	if (targetEntity1 != 0)
	{
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetEntity1);
	}
	if (targetEntity2 != 0)
	{
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetEntity2);
	}

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(sizeX), float(sizeY), vec2(0.f, 0.f), true, TRIGGER, PLAYER | OBJECTS);
	tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));

	return entity;
}

unsigned Scene::CreateScaleTrigger(int sizeX, int sizeY, float posX, float posY, unsigned int targetEntity0, 
		bool grow, bool isHold, unsigned int targetEntity1, unsigned int targetEntity2)//default values
{
	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Trigger*>(entity);

	//Sets up components
	std::string fileName = "Red.png";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, sizeX, sizeY);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posX, posY, 80.f));
	ECS::GetComponent<Trigger*>(entity) = new ScaleTrigger();
	ScaleTrigger* temp = (ScaleTrigger*)ECS::GetComponent<Trigger*>(entity);
	temp->grow = grow;
	ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
	ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetEntity0);
	if (targetEntity1 != 0)
	{
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetEntity1);
	}
	if (targetEntity2 != 0)
	{
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetEntity2);
	}

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(sizeX), float(sizeY), vec2(0.f, 0.f), true, TRIGGER, PLAYER | OBJECTS);
	tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));

	return entity;
}

unsigned Scene::CreateRevealTrigger(int sizeX, int sizeY, float posX, float posY, unsigned int targetEntity0,
		 bool isHold, unsigned int targetEntity1, unsigned int targetEntity2)
{
	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Trigger*>(entity);

	//Sets up components
	std::string fileName = "Red.png";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, sizeX, sizeY);
	ECS::GetComponent<Sprite>(entity).SetTransparency(0.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posX, posY, 80.f));
	ECS::GetComponent<Trigger*>(entity) = new RevealTrigger();
	ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
	ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetEntity0);
	if (targetEntity1 != 0)
	{
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetEntity1);
	}
	if (targetEntity2 != 0)
	{
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetEntity2);
	}

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(sizeX), float(sizeY), vec2(0.f, 0.f), true, TRIGGER, PLAYER | OBJECTS);
	tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));

	return entity;
}

entt::registry* Scene::GetScene() const
{
	return m_sceneReg;
}

void Scene::SetScene(entt::registry& scene)
{
	m_sceneReg = &scene;
}

vec4 Scene::GetClearColor() const
{
	return m_clearColor;
}

void Scene::SetClearColor(vec4 clearColor)
{
	m_clearColor = clearColor;
}

std::string Scene::GetName() const
{
	return m_name;
}

void Scene::SetName(std::string name)
{
	m_name = name;
}

b2Vec2 Scene::GetGravity() const
{
	return m_gravity;
}

void Scene::SetGravity(b2Vec2 grav)
{
	m_gravity = grav;
}

b2World & Scene::GetPhysicsWorld()
{
	return *m_physicsWorld;
}

void Scene::SetWindowSize(float windowWidth, float windowHeight)
{
	//TODO: Find new way to get the main camera
	auto& tempCam = m_sceneReg->get<Camera>(MainEntities::MainCamera());
	
	tempCam.SetWindowSize(vec2(windowWidth, windowHeight));
	tempCam.Orthographic(float(windowWidth / windowHeight), tempCam.GetOrthoSize().x, tempCam.GetOrthoSize().y,
															tempCam.GetOrthoSize().z, tempCam.GetOrthoSize().w,
															tempCam.GetNear(), tempCam.GetFar());
}
