#include "tab_gpugraph.h"
#include <windows.h>
#include <tchar.h>

void InitTabGpuGraphContent(HWND hwndParent) {
    CreateWindow(TEXT("STATIC"), _T("GPU Graph Placeholder"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10, 30, 780, 450,
        hwndParent, NULL, GetModuleHandle(NULL), NULL);
}
