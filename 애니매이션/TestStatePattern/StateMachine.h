#pragma once
#include <array>
#include <memory>
#include "IState.h"
#include "testDefine.h"

template<typename Context>
class StateMachine
{
public:
	StateMachine() = default;

	void Init(State initState)
	{
		m_current = initState;
	}

	State CurrentState() const
	{
		return m_current;
	}

	void SetStates(std::array<std::shared_ptr<IState>, static_cast<size_t>(State::Count)>&& pool)
	{
		states = std::move(pool);
	}

	void ChangeState(State s)
	{
		m_current = s;
	}

	void HandleEvent(Context& ctx, Event e)
	{
		states[static_cast<size_t>(m_current)]->HandleEvent(ctx, e);
	}

	void Update(Context& ctx)
	{
		states[static_cast<size_t>(m_current)]->Update(ctx);
	}

private:
	State m_current = State::Idle;
	std::array<std::shared_ptr<IState>, static_cast<size_t>(State::Count)> states;
};
