#include "process_manager.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <commctrl.h> // Ensure this is included for LVM_SETREDRAW
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <windows.h>
#ifndef LVM_SETREDRAW
#define LVM_SETREDRAW (LVM_FIRST + 21) // LVM_FIRST is defined in commctrl.h
#endif
// GetCommandLineFromProcess function
void GetCommandLineFromProcess(DWORD processID, TCHAR *cmdLine, size_t size)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (hProcess == NULL) {
        _tcscpy_s(cmdLine, size, _T("N/A"));
        return;
    }

    HMODULE hMod = NULL;
    DWORD cbNeeded = 0;
    if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
        TCHAR szProcessName[MAX_PATH];
        if (GetModuleFileNameEx(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR))) {
            _tcscpy_s(cmdLine, size, szProcessName);
        } else {
            _tcscpy_s(cmdLine, size, _T("N/A"));
        }
    } else {
        _tcscpy_s(cmdLine, size, _T("N/A"));
    }

    CloseHandle(hProcess);
}
void ListProcesses(HWND hListView)
{
     // Save the current selection
    int selectedIndex = (int)SendMessage(hListView, LVM_GETNEXTITEM, (WPARAM)-1, (LPARAM)LVNI_SELECTED);

    // Avoid flickering by suspending updates
    SendMessage(hListView, LVM_SETREDRAW, FALSE, 0);

    SendMessage(hListView, LVM_DELETEALLITEMS, 0, 0); // Clear the ListView

    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        MessageBox(NULL, _T("Failed to create process snapshot!"), _T("Error"), MB_ICONERROR);
        SendMessage(hListView, LVM_SETREDRAW, TRUE, 0);
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        MessageBox(NULL, _T("Process32First failed!"), _T("Error"), MB_ICONERROR);
        CloseHandle(hProcessSnap);
        SendMessage(hListView, LVM_SETREDRAW, TRUE, 0);
        return;
    }


    // Define columns if not already done
    // ...

    // Now walk the snapshot of processes
    do
    {
        TCHAR processName[MAX_PATH];
        TCHAR commandLine[MAX_PATH] = _T("N/A");
        TCHAR platform[100] = _T("N/A");
        TCHAR cpuUsage[20] = _T("N/A");
        TCHAR ramUsage[20] = _T("N/A");

        DWORD processID = pe32.th32ProcessID;
        DWORD sessionId = 0;

        _tcscpy_s(processName, _countof(processName), pe32.szExeFile);

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
        if (hProcess)
        {
            ProcessIdToSessionId(processID, &sessionId);

            // Retrieve platform (architecture)
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
                _tcscpy_s(platform, _countof(platform), _T("x64"));
            else if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
                _tcscpy_s(platform, _countof(platform), _T("x86"));
            else
                _tcscpy_s(platform, _countof(platform), _T("Unknown"));

            // Retrieve RAM usage
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
            {
                double memoryInMB = (double)pmc.WorkingSetSize / (1024 * 1024);
                _stprintf_s(ramUsage, _countof(ramUsage), _T("%.2f MB"), memoryInMB);
            }

            // Dummy CPU usage (use a more accurate method if needed)
            _tcscpy_s(cpuUsage, _countof(cpuUsage), _T("N/A"));

            GetCommandLineFromProcess(processID, commandLine, _countof(commandLine));

            TCHAR pidText[256];
            _stprintf_s(pidText, _countof(pidText), _T("%u"), processID);

            TCHAR sessionIdText[256];
            _stprintf_s(sessionIdText, _countof(sessionIdText), _T("%u"), sessionId);

            LVITEM lvItem = { 0 };
            lvItem.mask = LVIF_TEXT;
            lvItem.iItem = ListView_GetItemCount(hListView);

            lvItem.pszText = pidText;
            ListView_InsertItem(hListView, &lvItem);

            lvItem.iSubItem = 1;
            lvItem.pszText = sessionIdText;
            ListView_SetItem(hListView, &lvItem);

            lvItem.iSubItem = 2;
            lvItem.pszText = processName;
            ListView_SetItem(hListView, &lvItem);

            lvItem.iSubItem = 3;
            lvItem.pszText = platform;
            ListView_SetItem(hListView, &lvItem);

            lvItem.iSubItem = 4;
            lvItem.pszText = cpuUsage;
            ListView_SetItem(hListView, &lvItem);

            lvItem.iSubItem = 5;
            lvItem.pszText = ramUsage;
            ListView_SetItem(hListView, &lvItem);

            lvItem.iSubItem = 6;
            lvItem.pszText = commandLine;
            ListView_SetItem(hListView, &lvItem);

            CloseHandle(hProcess);
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

    // Resume drawing
    SendMessage(hListView, LVM_SETREDRAW, TRUE, 0);

    // Restore selection
    if (selectedIndex != -1 && selectedIndex < ListView_GetItemCount(hListView))
    {
        ListView_SetItemState(hListView, selectedIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        ListView_EnsureVisible(hListView, selectedIndex, FALSE);
    }
}

void UpdateProcessList(HWND hwndListView) {
    // Implementation for updating the ListView with process information
    // This is a placeholder and should be implemented as needed
}
