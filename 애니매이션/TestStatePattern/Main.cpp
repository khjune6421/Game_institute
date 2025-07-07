#include <iostream>
#include "TestDefine.h"
#include "SimpleCase.h"
#include "GameObject.h"

int main()
{
	GameObject testObject;

	while (true)
	{
		char input;
		std::cout << "Enter event (h: Hungry, b: Bored, t: Tired, q: Quit): ";
		std::cin >> input;

		if (input == 'q') { break; }

		Event event;

		switch (input)
		{
		case 'h':
			event = Event::Hungry;
			break;

		case 'b':
			event = Event::Bored;
			break;

		case 't':
			event = Event::Tired;
			break;

		default:
			continue;
		}

		testObject.HandleEvent(event);
		testObject.Update();
	}

}