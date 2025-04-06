#pragma once
#include <chrono>
#include <thread>

class Timer;

namespace CommonUtilities
{
	class Timer
	{
	public:
		Timer();
		Timer(const Timer& aTimer) = delete;
		Timer& operator=(const Timer& aTimer) = delete;

		void Update();

		float GetDeltaTime() const;
		double GetTotalTime() const;
	private:
		auto GetCurrTime() const;
		float myDelta;
		std::chrono::high_resolution_clock::time_point myStartTime;
		std::chrono::high_resolution_clock::time_point myTotalTime;
		std::chrono::high_resolution_clock::time_point myLastUpdate;
	};
}