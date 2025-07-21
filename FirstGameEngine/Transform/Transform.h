#pragma once

struct Transform
{
	Transform(float x = 0.0f, float y = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, float rotation = 0.0f)
		: x(x), y(y), scaleX(scaleX), scaleY(scaleY), rotation(rotation) {}

	float x, y;
	float scaleX, scaleY;
	float rotation;
};

// 나중에 lookat 같은 기능 추가?