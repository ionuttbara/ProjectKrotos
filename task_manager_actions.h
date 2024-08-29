#ifndef TASK_MANAGER_ACTIONS_H
#define TASK_MANAGER_ACTIONS_H

#include <windows.h>

// Function declarations for button actions
void CleanupMemory(HWND hwnd);
void KillNonRespondingProcesses(HWND hwnd);
void RunNewTask(HWND hwnd, BOOL runCmd);
void KillSelectedProcess(HWND hListView);

#endif // TASK_MANAGER_ACTIONS_H
