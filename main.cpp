#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // Don’t get strange min and max defines
#define REPORT_DX_WARNINGS // Remove some warnings from the DirectX headers
#include <memory>
#include <windows.h>

#include "Engine.h"
#include "GameWorld.h"
#include "GraphicsEngine.h"
#include "Includes/MeehanInputHandler.h"
#include "Includes/MeehanTimer.h"

std::shared_ptr<CommonUtilities::InputHandler> inputHandler = std::make_shared<CommonUtilities::InputHandler>();

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(

	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE,
	_In_ LPWSTR,
	_In_ int nCmdShow

) {

	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszClassName = L"TGP_MEEHAN";
	RegisterClassExW(&wcex);

	int width = 1280;
	int height = 720;
	HWND hWnd = CreateWindow(L"TGP_MEEHAN", L"TGP MEEHAN",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
	{
		return 0;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	bool shouldRun = true;

	Engine& engine = Engine::GetInstance();
	std::shared_ptr<CommonUtilities::Timer> timer = std::make_shared<CommonUtilities::Timer>();
	if(engine.Init(hWnd, timer))
	{
		shouldRun = true;
	}
	else
	{
		shouldRun = false;
	}

	GameWorld gameWorld(inputHandler, timer);

	MSG msg = {};
	while (shouldRun)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				shouldRun = false;
			}
		}

		if (!gameWorld.Run())
		{
			shouldRun = false;
		}
	}
	engine.GetGraphicsEngine().CleanupAllExceptDevice();
	engine.GetGraphicsEngine().ReportLiveObjects();
	engine.GetGraphicsEngine().CleanupDevice();
	engine.GetGraphicsEngine().CleanupInfoQueue();

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (inputHandler->UpdateEvents(message, wParam, lParam))
	{
		return 0;
	}
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}