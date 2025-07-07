#pragma once

class GameObject;
enum class Event;

class IState
{
public:
	virtual ~IState() = default;

	virtual void HandleEvent(GameObject& object, Event event) = 0;
	virtual void Update(GameObject& object) = 0;
};