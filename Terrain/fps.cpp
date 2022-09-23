#include "fps.h"

// Function to initialize Fps counter
void Fps::Initialize() {

    m_fps = 0;
    m_count = 0;
    m_startTime = timeGetTime();

}

// Function to update fps per frame
void Fps::Frame() {

    m_count++;

    if (timeGetTime() >= (m_startTime + 1000)) {
        m_fps = m_count;
        m_count = 0;
        m_startTime = timeGetTime();
    }

}