#include <iostream>
#include "TestDefine.h"

class TestObject
{
public:
    TestObject() : m_currentState(State::Idle) {}

    void HandleEvent(Event event)
    {
        switch (event)
        {
        case Event::Hungry:
            if (m_currentState == State::Idle || m_currentState == State::Eating)
            {
                m_currentState = State::Eating;
            }
            break;
        case Event::Bored:
            if (m_currentState == State::Idle || m_currentState == State::Playing)
            {
                m_currentState = State::Playing;
            }
            break;
        case Event::Tired:
            if (m_currentState == State::Idle || m_currentState == State::Sleeping)
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
        case State::Idle:
            break;
        case State::Eating:
            break;
        case State::Playing:
            break;
        case State::Sleeping:
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
        std::cout << "TestObject is now Playing.\n";
    }
    void OnSleeping()
    {
        std::cout << "TestObject is now Sleeping.\n";
    }

private:
    State m_currentState;
};
