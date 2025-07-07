#include "EatingState.h"
#include "TestDefine.h"
#include "GameObject.h"
#include <iostream>

void EatingState::HandleEvent(GameObject& obj, Event e)
{
	if (e == Event::Bored)
	{
		obj.ChangeState(State::Playing);
	}
	else if (e == Event::Tired)
	{
		obj.ChangeState(State::Sleeping);
	}
}

void EatingState::Update(GameObject&)
{
	std::cout << "GameObject is now Eating.\n";
}