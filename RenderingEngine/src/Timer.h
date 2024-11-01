#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	void Reset();
	float Elapsed()const;
	float ElapsedMillis();
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};