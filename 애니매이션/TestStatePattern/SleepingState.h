#pragma once
#include "IState.h"

class SleepingState : public IState
{
public:
	void HandleEvent(GameObject& obj, Event e) override;
	void Update(GameObject& obj) override;
};