#pragma once
#include <memory>
#include <mutex>

#include "GameWorld.h"
#include "Includes/Camera.h"

class TextureManager;
class GraphicsEngine;

class Engine
{
public:
    static Engine& GetInstance() // Meyers' Singleton thread-safe initialization in modern C++ (no mutex needed)
	{
        static Engine instance;
        return instance;
    }
	Engine(const Engine&) = delete; // Delete copy constructor and assignment operator
	Engine& operator=(const Engine&) = delete;
	Engine(Engine&&) = delete; // Delete move constructor and assignment operator
    Engine& operator=(Engine&&) = delete;

    bool Init(HWND& aHWnd, std::shared_ptr<CommonUtilities::Timer> aTimer);
    bool InitSubsystems();

    std::shared_ptr<HWND> GetHWnd();

    GraphicsEngine& GetGraphicsEngine() const { return *myGraphicsEngine; }
    CommonUtilities::Timer& GetTimer() const { return *myTimer; }

private:
    Engine();
    ~Engine();
    bool myIsInitialized = false;

    std::shared_ptr<HWND> myHWnd;
    std::shared_ptr<CommonUtilities::Timer> myTimer;
    static std::mutex myMutex;
    static std::unique_ptr<Engine> myInstance;
    std::unique_ptr<GraphicsEngine> myGraphicsEngine;

};

