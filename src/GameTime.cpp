#include "pch.h"
#include "GameTime.h"

void Time::UpdateTimers()
{
    for (auto iterator = m_TimerPtrs.begin(); iterator != m_TimerPtrs.end();)
    {
        if ((*iterator)->m_Cancelled)
        {
            iterator = m_TimerPtrs.erase(iterator);
            continue;
        }

        (*iterator)->Update();
        ++iterator;
    }
}

void Time::CleanTimers()
{
    m_TimerPtrs.clear();
}

void Time::SetTimeScale(float timeScale)
{
    m_TimeScale = std::max(0.0f,timeScale);
}
