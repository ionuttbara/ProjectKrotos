#include "task_manager_actions.h"
#include <windows.h>
#include <psapi.h>
#include <shellapi.h>
#include <commctrl.h>
#include <tchar.h>

void CleanupMemory(HWND hwnd)
{
    DWORD processes[1024];
    DWORD cbNeeded;

    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded))
    {
        MessageBox(hwnd, _T("Failed to enumerate processes!"), _T("Error"), MB_ICONERROR);
        return;
    }

    DWORD processCount = cbNeeded / sizeof(DWORD);

    for (DWORD i = 0; i < processCount; i++)
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_SET_QUOTA, FALSE, processes[i]);
        if (hProcess == NULL)
        {
            continue;
        }

        EmptyWorkingSet(hProcess);
        CloseHandle(hProcess);
    }
}

void KillNonRespondingProcesses(HWND hwnd)
{
    TCHAR cmdLine[] = _T("/c taskkill.exe /f /fi \"status eq Not Responding\" & pause");
    HINSTANCE hInstance = ShellExecute(NULL, _T("open"), _T("cmd.exe"), cmdLine, NULL, SW_SHOWNORMAL);

    if ((INT_PTR)hInstance <= 32)
    {
        MessageBox(hwnd, _T("Failed to execute the taskkill command!"), _T("Error"), MB_ICONERROR);
    }
}

void RunNewTask(HWND hwnd, BOOL runCmd)
{
    if (runCmd)
    {
        ShellExecute(NULL, _T("open"), _T("cmd.exe"), NULL, NULL, SW_SHOWNORMAL);
    }
    else
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
}

void KillSelectedProcess(HWND hListView)
{
    // Get the selected item
    int iSelected = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
    if (iSelected == -1)
    {
        MessageBox(NULL, _T("No process selected!"), _T("Error"), MB_ICONERROR);
        return;
    }

    // Get the PID from the selected item
    TCHAR szPID[256];
    ListView_GetItemText(hListView, iSelected, 0, szPID, sizeof(szPID) / sizeof(TCHAR));
    DWORD dwPID = _ttoi(szPID);

    // Get the process name from the selected item
    TCHAR szProcessName[256];
    ListView_GetItemText(hListView, iSelected, 2, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));

    // Show a confirmation dialog including the process name and PID
    TCHAR szMessage[512];
    _stprintf_s(szMessage, sizeof(szMessage) / sizeof(TCHAR), _T("Are you sure you want to kill the process \"%s\" with PID %d?"), szProcessName, dwPID);

    int result = MessageBox(NULL, szMessage, _T("Confirm Kill Process"), MB_YESNO | MB_ICONQUESTION);
    if (result == IDNO)
    {
        return;
    }

    // Open the process with terminate permissions
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwPID);
    if (hProcess == NULL)
    {
        MessageBox(NULL, _T("Failed to open process!"), _T("Error"), MB_ICONERROR);
        return;
    }

    // Attempt to terminate the process
    if (!TerminateProcess(hProcess, 0))
    {
        MessageBox(NULL, _T("Failed to terminate process!"), _T("Error"), MB_ICONERROR);
    }
    else
    {

   }

    CloseHandle(hProcess);
}

