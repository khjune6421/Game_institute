#include <iostream>
#include "TestDefine.h"

class TestObject
{
public:
	TestObject() : m_currentState(State::Sleeping) {}


	void HandleEvent(Event event)
	{
		switch (m_currentState)
		{
		case State::Eating:
			if (event == Event::Hungry)
			{
				m_currentState = State::Eating;
			}
			break;
		case State::Playing:
			if (event == Event::Bored)
			{
				m_currentState = State::Playing;
			}
			else if (event == Event::Tired)
			{
				m_currentState = State::Sleeping;
			}
			break;
		case State::Sleeping:
			if (event == Event::Tired)
			{
				m_currentState = State::Sleeping;
			}
			break;
		default:
			break;
		}
	}

	void Update()
	{
		switch (m_currentState)
		{
		case State::Eating:
			OnEating();
			break;

		case State::Playing:
			OnPlaying();
			break;

		case State::Sleeping:
			OnSleeping();
			break;

		default:
			break;
		}
	}

	void OnEating()
	{
		std::cout << "TestObject is now Eating.\n";
	}

	void OnPlaying()
	{
		std::cout << "TestObject is now Dancing.\n";
	}

	void OnSleeping()
	{
		std::cout << "TestObject is now Sleeping.\n";
	}

private:
	State m_currentState;
};