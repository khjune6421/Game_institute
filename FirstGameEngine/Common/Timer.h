#include <Windows.h>

class Timer
{
	double multiplieToSeconds = 1.0;
	double deltaTime = 0.0;

	LARGE_INTEGER currentTime = {};
	LARGE_INTEGER previousTime = {};

public:
	Timer() = default;

	void Reset();
	void Tick();

	float GetTime() const { return static_cast<float>(currentTime.QuadPart * multiplieToSeconds); }
	float GetTimeInMS() const { return static_cast<float>(currentTime.QuadPart * multiplieToSeconds * 1000.0); }

	float GetDeltaTime() const { return static_cast<float>(deltaTime); }
	float GetDeltaTimeInMS() const { return static_cast<float>(deltaTime * 1000.0); }
};