//--------------------------------------------------------------------------------------
// counts how many frames occur in a one second period and constantly updates that count
//--------------------------------------------------------------------------------------
#ifndef _FPS_H_
#define _FPS_H_

#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>

class Fps {
public:
    // constructors 
    Fps() {};
    Fps(const Fps& other) {};
    // destructor
    ~Fps() {};

    // Function to initialize Fps counter
    void Initialize();
    // Function to update fps per frame
    void Frame();
    // Function to get FPS
    int GetFps() { return m_fps; };

private:
    int m_fps, m_count;
    unsigned long m_startTime;
};

#endif
