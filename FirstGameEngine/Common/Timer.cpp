#include "Timer.h"

void Timer::Reset()
{
	QueryPerformanceFrequency(&currentTime);
	QueryPerformanceCounter(&previousTime);
	if (currentTime.QuadPart != 0) multiplieToSeconds = 1.0 / static_cast<double>(currentTime.QuadPart);
	else multiplieToSeconds = 1.0;
	deltaTime = 0.0;
}

void Timer::Tick()
{
	QueryPerformanceCounter(&currentTime);
	deltaTime = (currentTime.QuadPart - previousTime.QuadPart) * multiplieToSeconds;
	previousTime = currentTime;
}