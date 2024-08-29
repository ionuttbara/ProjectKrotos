#include "buttons.h"
#include <windows.h>

#define BUTTON_COUNT 4 // Number of buttons

void PositionButtons(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    int buttonWidth = (rect.right - rect.left - (BUTTON_COUNT + 1) * 10) / BUTTON_COUNT; // Width of each button
    int buttonHeight = 30;
    int yPos = rect.bottom - 40; // 40 pixels from the bottom

    // Position each button dynamically
    HWND hButton;
    hButton = GetDlgItem(hwnd, 2);
    SetWindowPos(hButton, NULL, 10, yPos, buttonWidth, buttonHeight, SWP_NOZORDER);

    hButton = GetDlgItem(hwnd, 4);
    SetWindowPos(hButton, NULL, 20 + buttonWidth, yPos, buttonWidth, buttonHeight, SWP_NOZORDER);

    hButton = GetDlgItem(hwnd, 3);
    SetWindowPos(hButton, NULL, 30 + 2 * buttonWidth, yPos, buttonWidth, buttonHeight, SWP_NOZORDER);

    hButton = GetDlgItem(hwnd, 5);
    SetWindowPos(hButton, NULL, 40 + 3 * buttonWidth, yPos, buttonWidth, buttonHeight, SWP_NOZORDER);
}

// Implement the AddButtons function
void AddButtons(HWND hwnd)
{
    // Create Kill Process button
    CreateWindow(TEXT("button"), TEXT("Kill Process"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 520, 100, 30, hwnd, (HMENU)2, GetModuleHandle(NULL), NULL);

    // Create Clean Up Memory button
    CreateWindow(TEXT("button"), TEXT("Clean Up Memory"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        120, 520, 120, 30, hwnd, (HMENU)4, GetModuleHandle(NULL), NULL);

    // Create Run New Task button
    CreateWindow(TEXT("button"), TEXT("Run New Task"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        250, 520, 110, 30, hwnd, (HMENU)3, GetModuleHandle(NULL), NULL);

    // Create Kill Non-Responding Tasks button
    CreateWindow(TEXT("button"), TEXT("Kill Non-Responding Tasks"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        370, 520, 180, 30, hwnd, (HMENU)5, GetModuleHandle(NULL), NULL);

    PositionButtons(hwnd);
}
