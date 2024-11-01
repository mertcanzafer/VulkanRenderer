#include "Timer.h"

Timer::Timer()
{
	Reset();
}

void Timer::Reset()
{
	m_Start = std::chrono::high_resolution_clock::now();
}

float Timer::Elapsed() const
{
	float deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() 
		* 0.000000001f;
	return deltaTime;
}

float Timer::ElapsedMillis()
{
	return this->Elapsed() * 1000.0f;
}
