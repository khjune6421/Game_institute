#include "SleepingState.h"
#include "TestDefine.h"
#include "GameObject.h"
#include <iostream>

void SleepingState::HandleEvent(GameObject& obj, Event e)
{
	if (e == Event::Hungry)
	{
		obj.ChangeState(State::Eating);
	}
	else if (e == Event::Bored)
	{
		obj.ChangeState(State::Playing);
	}
}

void SleepingState::Update(GameObject&)
{
	std::cout << "GameObject is now Sleeping.\n";
}