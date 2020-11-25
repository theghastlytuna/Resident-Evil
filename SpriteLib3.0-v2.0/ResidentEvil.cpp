#include "ResidentEvil.h"
#include "Utilities.h"

ResidentEvil::ResidentEvil(std::string name)
	: Scene(name)
{
	m_gravity = b2Vec2(0.f, 0.f);
	m_physicsWorld->SetGravity(m_gravity);
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
		ECS::SetIsMainPlayer(entity, true);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//set components
		std::string fileName = "top_downfull_body.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 50, 50);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 2.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(0.f), float32(0.f));//start point 10, 100

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.35f, 1.2f); //square body

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

}

void ResidentEvil::KeyboardUp()
{
	if (Input::GetKeyUp(Key::S))
	{
		flipped = false;
	}
}
