#include "GameObject.h"
#include "SleepingState.h"
#include "EatingState.h"
#include "PlayingState.h"
#include "IdleState.h"
#include <iostream>


GameObject::GameObject()
{
	m_stateMachine.Init(State::Idle);

	m_stateMachine.SetStates
	({
		std::make_shared<IdleState>(),
		std::make_shared<SleepingState>(),
		std::make_shared<EatingState>(),
		std::make_shared<PlayingState>()
	});

	m_stateMachine.ChangeState(State::Sleeping);
}

void GameObject::ChangeState(State newState)
{
	OnExit(m_stateMachine.CurrentState());
	m_stateMachine.ChangeState(newState);
	OnEnter(newState);
}

void GameObject::OnEnter(State state)
{
	switch (state)
	{
	case State::Idle:
		std::cout << "GameObject is now Idle." << std::endl;
		break;
	case State::Eating:
		std::cout << "GameObject is now Eating." << std::endl;
		break;
	case State::Playing:
		std::cout << "GameObject is now Playing." << std::endl;
		break;
	case State::Sleeping:
		std::cout << "GameObject is now Sleeping." << std::endl;
		break;
	default:
		break;
	}
}

void GameObject::OnExit(State state)
{
	switch (state)
	{
	case State::Idle:
		std::cout << "GameObject is exiting Idle state." << std::endl;
		break;
	case State::Eating:
		std::cout << "GameObject is exiting Eating state." << std::endl;
		break;
	case State::Playing:
		std::cout << "GameObject is exiting Playing state." << std::endl;
		break;
	case State::Sleeping:
		std::cout << "GameObject is exiting Sleeping state." << std::endl;
		break;
	default:
		break;
	}
}

void GameObject::HandleEvent(Event e)
{
	m_stateMachine.HandleEvent(*this, e);
}

void GameObject::Update()
{
	m_stateMachine.Update(*this);
}