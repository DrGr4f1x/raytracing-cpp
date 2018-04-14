//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

// Precompiled header
#include "stdafx.h"

// Timer header
#include "Timer.h"

/**
*  Constructor.  Determines the clock frequency.
*/
Timer::Timer() 
    : m_oldTime()
    , m_ticksPerMicrosecond(0.0)
    , m_elapsedSeconds(0.0)
    , m_elapsedMicroseconds(0.0)
	, m_isRunning(false)
{
    LARGE_INTEGER ticksPerSec;
    QueryPerformanceFrequency(&ticksPerSec);
    m_ticksPerMicrosecond = static_cast<double>(ticksPerSec.QuadPart) * 0.000001;
}


/**
*  Starts the timer.
*/
void Timer::Start()
{
    m_isRunning = true;
    QueryPerformanceCounter(&m_oldTime);
}


/**
*  Takes a final sample and stops the timer.
*/
void Timer::Stop()
{
	Sample();
    m_isRunning = false;
}


/**
*  Takes a sample from a running timer.  This stores the current time as the
*  previous time, and so starts a new timing interval.
*/
void Timer::Sample()
{
    if(m_isRunning)
    {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        __int64 ticks = currentTime.QuadPart - m_oldTime.QuadPart;
        m_oldTime = currentTime;

        m_elapsedMicroseconds = ticks / m_ticksPerMicrosecond;
        m_elapsedSeconds = m_elapsedMicroseconds * 0.000001;
    }
}


/**
*  Gets the elapsed seconds between the most recent sample and the previous sample
*  (or the timer start time, if only 1 sample has been taken).
*  @return The elapsed time in seconds
*/
double Timer::GetElapsedSeconds() const
{
    return m_elapsedSeconds;
}


/**
*  Gets the elapsed microseconds between the most recent sample and the previous sample
*  (or the timer start time, if only 1 sample has been taken).
*  @return The elapsed time in microseconds
*/
double Timer::GetElapsedMicroseconds() const
{
    return m_elapsedMicroseconds;
}