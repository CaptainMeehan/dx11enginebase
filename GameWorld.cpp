#include "GameWorld.h"

#include "Engine.h"

bool GameWorld::Run()
{
	myTimer->Update();
	Update();
	Render();
	if (myInputHandler->GetKeyDown(VK_ESCAPE))
	{
		return false;
	}
	return true;
}

bool GameWorld::Init()
{
	return true;
}

void GameWorld::Update()
{
	GraphicsEngine& gE = Engine::GetInstance().GetGraphicsEngine();
	myInputHandler->Update();
	gE.Update(myTimer->GetDeltaTime(), *myInputHandler);
}
void GameWorld::Render()
{
	GraphicsEngine& gE = Engine::GetInstance().GetGraphicsEngine();
	gE.Render(myTimer->GetDeltaTime());
}