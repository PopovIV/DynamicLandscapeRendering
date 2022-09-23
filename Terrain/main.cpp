#include "system.h"

//--------------------------------------------------------------------------------------
// Entry point to the program. Creates system class and clears everything after it's
// work is done
//--------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {

    System* system;
    bool result;

    // Create the system object.
    system = new System;
    if (!system)
        return 0;

    // Initialize and run the system object.
    result = system->Initialize();
    if (result)
        system->Run();

    // Shutdown and release the system object.
    system->Shutdown();
    delete system;
    system = nullptr;

    return 0;

}
