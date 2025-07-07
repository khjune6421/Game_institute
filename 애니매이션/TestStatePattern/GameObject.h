#pragma once
#include "StateMachine.h"

class GameObject
{
public:
	GameObject();
	~GameObject() = default;

	void HandleEvent(Event e);
	void Update();

	void ChangeState(State newState);

	void OnEnter(State state);
	void OnExit(State state);

private:
	StateMachine<GameObject> m_stateMachine;
};