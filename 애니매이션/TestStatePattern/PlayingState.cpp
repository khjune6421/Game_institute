#include "PlayingState.h"
#include "TestDefine.h"
#include "GameObject.h"
#include <iostream>

void PlayingState::HandleEvent(GameObject& obj, Event e)
{
	if (e == Event::Hungry)
	{
		obj.ChangeState(State::Eating);
	}
	else if (e == Event::Tired)
	{
		obj.ChangeState(State::Sleeping);
	}
}

void PlayingState::Update(GameObject&)
{
	std::cout << "GameObject is now Playing.\n";
}