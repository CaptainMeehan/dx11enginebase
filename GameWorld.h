#pragma once
#include <memory>
#include "GraphicsEngine.h"
#include "Includes/MeehanInputHandler.h"
#include "Includes/MeehanTimer.h"

class GameWorld
{
public:
    GameWorld(std::shared_ptr<CommonUtilities::InputHandler> anInputHandler, std::shared_ptr<CommonUtilities::Timer> aTimer)
        : myInputHandler(anInputHandler), myTimer(aTimer) {}

    bool Run();

private:
    bool Init();
    void Update();
    void Render();

    bool isRunning = true;
    std::shared_ptr<CommonUtilities::InputHandler> myInputHandler;
    std::shared_ptr<CommonUtilities::Timer> myTimer;
};
