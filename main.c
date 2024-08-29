#include <windows.h>
#include "gui.h"

#define TIMER_ID 1
#define TIMER_INTERVAL 1000 // 1 second
HWND hListView = NULL;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    // Create and show the main window
    if (!CreateMainWindow(hInstance, nCmdShow))
    {
        MessageBox(NULL, "Failed to create main window!", "Error", MB_ICONERROR);
        return 1;
    }

    // Set a timer to refresh every second
    SetTimer(NULL, TIMER_ID, TIMER_INTERVAL, NULL);

    // Start the message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Kill the timer
    KillTimer(NULL, TIMER_ID);

    return (int)msg.wParam;
}
