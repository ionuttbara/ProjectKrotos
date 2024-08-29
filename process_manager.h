#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <windows.h>

// Declaration of ProcessNode
typedef struct ProcessNode {
    DWORD processID;
    struct ProcessNode* parent;
    struct ProcessNode* firstChild;
    struct ProcessNode* nextSibling;
} ProcessNode;

extern ProcessNode* rootNode;  // Declaration of global variable

// Function declarations
void ListProcesses(HWND hTreeView);
ProcessNode* FindProcessNode(ProcessNode* root, DWORD processID);
void KillProcessAndChildren(ProcessNode* node);
void KillSelectedProcess(HWND hListView);

void UpdateProcessList(HWND hwndListView);

#endif // PROCESS_MANAGER_H
