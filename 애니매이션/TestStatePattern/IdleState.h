#pragma once
#include "IState.h"

class IdleState : public IState
{
public:
	void HandleEvent(GameObject& obj, Event e) override;
	void Update(GameObject& obj) override;
};