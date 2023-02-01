//--------------------------------------------------------------------------------------
// Terrain class is ahigh precision timer that measures the exact time between frames of execution
//--------------------------------------------------------------------------------------
#ifndef _TIMER_H_
#define _TIMER_H_

#include <windows.h>

class Timer {
public:
    // constructors
    Timer() {};
    Timer(const Timer& other) {};
    // destructor
    ~Timer() {};

    // Function to initialize timer
    bool Initialize();
    // Function to initialize timer
    void Frame();

    // Function to get most recent frame time that was calculated
    float GetTime() { return m_frameTime; };
    // Function to start timer
    void StartTimer() { QueryPerformanceCounter((LARGE_INTEGER*)&m_beginTime); };
    // Function to stop timer
    void StopTimer() { QueryPerformanceCounter((LARGE_INTEGER*)&m_endTime); };

private:
    float m_frequency;
    INT64 m_startTime;
    float m_frameTime;
    INT64 m_beginTime, m_endTime;
};

#endif
