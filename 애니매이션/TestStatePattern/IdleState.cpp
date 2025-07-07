#include "IdleState.h"
#include "TestDefine.h"
#include "GameObject.h"
#include <iostream>

void IdleState::HandleEvent(GameObject& obj, Event e)
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
void IdleState::Update(GameObject&)
{
	std::cout << "GameObject is now Idle.\n";
}