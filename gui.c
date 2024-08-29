#include "resource.h"
#include "process_manager.h"
#include "task_manager_actions.h"
#include "menu.h"
#include "tab_process.h"
#include "tab_diskgraph.h"
#include "tab_gpugraph.h"
#include "tab_cpugraph.h"
#include <commctrl.h>
#include <commdlg.h>
#include <tchar.h>
#include <windows.h>
#include <shellapi.h>
#include <psapi.h>

#define WINDOW_CLASS_NAME _T("ProjectKrotosClass")
#define WINDOW_TITLE _T("Task Manager")
#define TIMER_ID 1
#define TIMER_INTERVAL 1000 // 1 second

void AddTabControls(HWND hwnd);
void PositionButtons(HWND hwnd);
void UpdateTabContent(HWND hwnd, int tabIndex);

HWND hTab; // Handle to the tab control
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        InitCommonControls();
        AddMenus(hwnd);
        AddTabControls(hwnd);

        // Set the initial tab to "Processes"
        TabCtrl_SetCurSel(hTab, 0);

        // Ensure the initial tab content is displayed correctly
        SendMessage(hwnd, WM_SIZE, 0, 0);

        break;

    case WM_COMMAND:
        HandleMenuCommand(hwnd, LOWORD(wParam));
        break;

    case WM_SIZE:
    {
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);

        // Resize the Tab control
        SetWindowPos(hTab, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);

        // Position controls based on the selected tab
        int tabIndex = TabCtrl_GetCurSel(hTab);
        UpdateTabContent(hwnd, tabIndex);

        // Position the buttons
        PositionButtons(hwnd);
    }
    break;

    case WM_NOTIFY:
    {
        LPNMHDR pnmhdr = (LPNMHDR)lParam;
        if (pnmhdr->code == TCN_SELCHANGE)
        {
            int tabIndex = TabCtrl_GetCurSel(hTab);
            UpdateTabContent(hwnd, tabIndex);
        }
    }
    break;

    case WM_TIMER:
        if (wParam == TIMER_ID)
        {
            // Update the content of the currently selected tab
            int tabIndex = TabCtrl_GetCurSel(hTab);
            switch (tabIndex)
            {
            case 0:
                UpdateProcessList(GetDlgItem(hwnd, 1));
                break;
            case 1:
                // Update Disk Graph content
                break;
            case 2:
                // Update GPU Graph content
                break;
            case 3:
                // Update CPU Graph content
                break;
            }
        }
        break;

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_ID);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


BOOL CreateMainWindow(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    HWND hwnd = CreateWindow(
        WINDOW_CLASS_NAME, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if (!hwnd)
    {
        return FALSE;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    SetTimer(hwnd, TIMER_ID, TIMER_INTERVAL, NULL);

    return TRUE;
}

void AddTabControls(HWND hwnd)
{
    hTab = CreateWindow(WC_TABCONTROL, _T(""),
        WS_CHILD | WS_VISIBLE | TCS_TABS,
        0, 0, 0, 0,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    TCITEM tie;
    tie.mask = TCIF_TEXT;
    tie.pszText = _T("Processes");
    TabCtrl_InsertItem(hTab, 0, &tie);
    tie.pszText = _T("Disk Graph");
    TabCtrl_InsertItem(hTab, 1, &tie);
    tie.pszText = _T("GPU Graph");
    TabCtrl_InsertItem(hTab, 2, &tie);
    tie.pszText = _T("CPU Graph");
    TabCtrl_InsertItem(hTab, 3, &tie);

    // Initialize controls for each tab
    InitTabProcessContent(hTab);
    InitTabDiskGraphContent(hTab);
    InitTabGpuGraphContent(hTab);
    InitTabCpuGraphContent(hTab);

    // Initially hide all tab content
    UpdateTabContent(hwnd, TabCtrl_GetCurSel(hTab));
}


void UpdateTabContent(HWND hwnd, int tabIndex)
{
    // Hide all tab contents
    HWND hProcessTab = GetDlgItem(hwnd, 1);
    HWND hDiskGraphTab = GetDlgItem(hwnd, 2);
    HWND hGpuGraphTab = GetDlgItem(hwnd, 3);
    HWND hCpuGraphTab = GetDlgItem(hwnd, 4);

    ShowWindow(hProcessTab, SW_HIDE);
    ShowWindow(hDiskGraphTab, SW_HIDE);
    ShowWindow(hGpuGraphTab, SW_HIDE);
    ShowWindow(hCpuGraphTab, SW_HIDE);

    // Show the content for the currently selected tab
    switch (tabIndex)
    {
    case 0:
        ShowWindow(hProcessTab, SW_SHOW);
        break;
    case 1:
        ShowWindow(hDiskGraphTab, SW_SHOW);
        break;
    case 2:
        ShowWindow(hGpuGraphTab, SW_SHOW);
        break;
    case 3:
        ShowWindow(hCpuGraphTab, SW_SHOW);
        break;
    }
}
