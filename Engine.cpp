#include "Engine.h"

#include "GraphicsEngine.h"

#include "Includes/MeehanTimer.h"

Engine::Engine() = default;
Engine::~Engine() = default;

bool Engine::Init(HWND& aHWnd, std::shared_ptr<CommonUtilities::Timer> aTimer)
{
    myHWnd = std::make_shared<HWND>(aHWnd);
	myTimer = aTimer;

	if (InitSubsystems())
	{
		return true;
	}
	return false;
}

bool Engine::InitSubsystems()
{
	if (!myIsInitialized) 
	{
		myGraphicsEngine = std::make_unique<GraphicsEngine>();

		myIsInitialized = true;
	}

	if (!myGraphicsEngine->Init(1080, 1920, *myHWnd))
	{
		return false;
	}
		return true;
}

std::shared_ptr<HWND> Engine::GetHWnd()
{
    return myHWnd;
}