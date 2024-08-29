#include "menu.h"
#include "resource.h"
#include "task_manager_actions.h"
#include "buttons.h"
#include <shellapi.h>
#include <tchar.h>
#include <windows.h>
#include <commctrl.h> // For WC_LISTVIEW, LVS_REPORT, LVCOLUMN, etc.
#include <commdlg.h>

extern HWND hListView; // Declare as extern if defined in another source file

#define TIMER_ID 1 // Define TIMER_ID if not defined elsewhere
#define SC_LOCK 0xF140

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void HandleMenuCommand(HWND hwnd, int command)
{
    switch (command)
    {
    case ID_FILE_RUN:
        {
            TCHAR szCmdLine[MAX_PATH] = { 0 };
            OPENFILENAME ofn = { 0 };
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szCmdLine;
            ofn.nMaxFile = sizeof(szCmdLine) / sizeof(TCHAR);
            ofn.lpstrFilter = _T("Executable Files\0*.exe\0All Files\0*.*\0");
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn))
            {
                ShellExecute(NULL, _T("open"), ofn.lpstrFile, NULL, NULL, SW_SHOWNORMAL);
            }
        }
        break;

        case ID_FILE_REPLACE_TASKMANAGER:
        {
            HKEY hKey;
            TCHAR szCurrentFile[MAX_PATH] = { 0 };
            GetModuleFileName(NULL, szCurrentFile, MAX_PATH);

            // Open the registry key
            LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                        _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\taskmgr.exe"),
                                        0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
            if (lResult == ERROR_SUCCESS)
            {
                DWORD dwSize = MAX_PATH * sizeof(TCHAR);
                TCHAR szDebugger[MAX_PATH] = { 0 };

                // Check if the Debugger value exists
                lResult = RegQueryValueEx(hKey, _T("Debugger"), NULL, NULL, (LPBYTE)szDebugger, &dwSize);

                if (lResult == ERROR_SUCCESS && _tcsicmp(szDebugger, szCurrentFile) == 0)
                {
                    // If exists and matches current executable, remove it (uncheck)
                    RegDeleteValue(hKey, _T("Debugger"));
                    CheckMenuItem(GetMenu(hwnd), ID_FILE_REPLACE_TASKMANAGER, MF_UNCHECKED);
               }
                else
                {
                    // If doesn't exist or is different, set the current executable (check)
                    RegSetValueEx(hKey, _T("Debugger"), 0, REG_SZ, (const BYTE*)szCurrentFile, (DWORD)(_tcslen(szCurrentFile) + 1) * sizeof(TCHAR));
                    CheckMenuItem(GetMenu(hwnd), ID_FILE_REPLACE_TASKMANAGER, MF_CHECKED);
                 }

                RegCloseKey(hKey);
            }
            else
            {
                MessageBox(hwnd, _T("Failed to open registry key."), _T("Error"), MB_OK | MB_ICONERROR);
            }
        }
        break;

     case ID_FILE_EXIT:
        PostQuitMessage(0);
        break;

    case ID_SPEED_SLOW:
        KillTimer(hwnd, TIMER_ID);
        SetTimer(hwnd, TIMER_ID, 3000, NULL);
        break;

    case ID_SPEED_NORMAL:
        KillTimer(hwnd, TIMER_ID);
        SetTimer(hwnd, TIMER_ID, 1000, NULL);
        break;

    case ID_SPEED_FAST:
        KillTimer(hwnd, TIMER_ID);
        SetTimer(hwnd, TIMER_ID, 500, NULL);
        break;

    case ID_TOOLS_WINDOWS_SETTINGS:
        ShellExecute(NULL, _T("open"), _T("ms-settings://"), NULL, NULL, SW_SHOWNORMAL);
        break;

    case ID_TOOLS_CONTROL_PANEL:
        ShellExecute(NULL, _T("open"), _T("control"), NULL, NULL, SW_SHOWNORMAL);
        break;

    case ID_TOOLS_DATE_TIME:
        ShellExecute(NULL, _T("open"), _T("ms-settings:dateandtime"), NULL, NULL, SW_SHOWNORMAL);
        break;

    case ID_TOOLS_DEVICE_MANAGER:
        ShellExecute(NULL, _T("open"), _T("devmgmt.msc"), NULL, NULL, SW_SHOWNORMAL);
        break;

    case ID_TOOLS_GROUP_POLICY:
        ShellExecute(NULL, _T("open"), _T("gpedit.msc"), NULL, NULL, SW_SHOWNORMAL);
        break;

    case ID_TOOLS_SERVICES:
        ShellExecute(NULL, _T("open"), _T("services.msc"), NULL, NULL, SW_SHOWNORMAL);
        break;

    case ID_TOOLS_POWER_OPTIONS:
        ShellExecute(NULL, _T("open"), _T("ms-settings:powersleep"), NULL, NULL, SW_SHOWNORMAL);
        break;

    case ID_SHUTDOWN_SHUTDOWN:
        system("shutdown /s /t 0");
        break;

    case ID_SHUTDOWN_RESTART:
        system("shutdown /r /t 0");
        break;

    case ID_SHUTDOWN_LOGOFF:
        ExitWindowsEx(EWX_LOGOFF, 0);
        break;

    case ID_SHUTDOWN_LOCK:
        LockWorkStation();
        break;

    case ID_ABOUT:
        ShowAboutWindow(hwnd);
        break;

    case 2: // Kill Process
        KillSelectedProcess(hListView);
        ListProcesses(hListView);
        break;

    case 3: // Run New Task
        RunNewTask(hwnd, GetKeyState(VK_CONTROL) < 0);
        break;

    case 4: // Clean Up Memory
        CleanupMemory(hwnd);
        break;

 case 5:
            KillNonRespondingProcesses(hwnd);
            break;

    case6: // Exit
        PostQuitMessage(0);
        break;
    }
}
void AddMenus(HWND hwnd)
{
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();
    HMENU hSpeedMenu = CreateMenu();
    HMENU hToolsMenu = CreateMenu(); // New Tools menu
    HMENU hShutdownMenu = CreateMenu();

    // Check the state of the Debugger registry value
    BOOL bIsTaskManagerReplaced = FALSE;
    HKEY hKey;
    TCHAR szCurrentFile[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szCurrentFile, MAX_PATH);

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\taskmgr.exe"),
                     0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        DWORD dwSize = MAX_PATH * sizeof(TCHAR);
        TCHAR szDebugger[MAX_PATH] = { 0 };

        // Check if the Debugger value exists and matches the current executable
        if (RegQueryValueEx(hKey, _T("Debugger"), NULL, NULL, (LPBYTE)szDebugger, &dwSize) == ERROR_SUCCESS)
        {
            if (_tcsicmp(szDebugger, szCurrentFile) == 0)
            {
                bIsTaskManagerReplaced = TRUE;
            }
        }

        RegCloseKey(hKey);
    }

    AppendMenu(hFileMenu, MF_STRING, ID_FILE_RUN, _T("Run..."));
    AppendMenu(hFileMenu, MF_STRING | (bIsTaskManagerReplaced ? MF_CHECKED : 0), ID_FILE_REPLACE_TASKMANAGER, _T("Replace Task Manager"));
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, _T("Exit"));

    AppendMenu(hSpeedMenu, MF_STRING, ID_SPEED_SLOW, _T("Slow"));
    AppendMenu(hSpeedMenu, MF_STRING, ID_SPEED_NORMAL, _T("Normal"));
    AppendMenu(hSpeedMenu, MF_STRING, ID_SPEED_FAST, _T("Fast"));

    AppendMenu(hToolsMenu, MF_STRING, ID_TOOLS_WINDOWS_SETTINGS, _T("Windows Settings"));
    AppendMenu(hToolsMenu, MF_STRING, ID_TOOLS_CONTROL_PANEL, _T("Control Panel"));
    AppendMenu(hToolsMenu, MF_STRING, ID_TOOLS_DATE_TIME, _T("Date and Time"));
    AppendMenu(hToolsMenu, MF_STRING, ID_TOOLS_DEVICE_MANAGER, _T("Device Manager"));
    AppendMenu(hToolsMenu, MF_STRING, ID_TOOLS_GROUP_POLICY, _T("Group Policy Editor"));
    AppendMenu(hToolsMenu, MF_STRING, ID_TOOLS_SERVICES, _T("Services"));
    AppendMenu(hToolsMenu, MF_STRING, ID_TOOLS_POWER_OPTIONS, _T("Power Options"));

    AppendMenu(hShutdownMenu, MF_STRING, ID_SHUTDOWN_SHUTDOWN, _T("Shutdown"));
    AppendMenu(hShutdownMenu, MF_STRING, ID_SHUTDOWN_RESTART, _T("Restart"));
    AppendMenu(hShutdownMenu, MF_STRING, ID_SHUTDOWN_LOGOFF, _T("Log Off"));
    AppendMenu(hShutdownMenu, MF_STRING, ID_SHUTDOWN_LOCK, _T("Lock"));

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, _T("File"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSpeedMenu, _T("Speed"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hToolsMenu, _T("Tools"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hShutdownMenu, _T("Shutdown"));
    AppendMenu(hMenu, MF_STRING, ID_ABOUT, _T("About"));

    SetMenu(hwnd, hMenu);
}



void ShowAboutWindow(HWND hwnd)
{
    MessageBox(hwnd, _T("Tasker by Name Version 1.0"), _T("About Tasker"), MB_OK | MB_ICONINFORMATION);
}



