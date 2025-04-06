#pragma once
#include "MeehanTimer.h"
#include <chrono>
#include <windows.h>

CommonUtilities::Timer::Timer() : myDelta(0)
{
	myStartTime = std::chrono::high_resolution_clock::now();
	myLastUpdate = myStartTime;
}

void CommonUtilities::Timer::Update()
{
	auto frameStart = std::chrono::steady_clock::now();
	std::chrono::duration<float, std::milli> delta = frameStart - myLastUpdate;
	myDelta = delta.count()/1000;
	myLastUpdate = frameStart;
}

float CommonUtilities::Timer::GetDeltaTime() const
{
	return myDelta;
}

double CommonUtilities::Timer::GetTotalTime() const
{
	std::chrono::duration<double> newTotalTime = std::chrono::high_resolution_clock::now() - myStartTime;
	std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(newTotalTime);
	return std::chrono::duration<double>(s).count();
}

auto CommonUtilities::Timer::GetCurrTime() const
{
	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	return currentTime;
}