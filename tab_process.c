#include "tab_process.h"
#include <commctrl.h>
#include <tchar.h>
#include "process_manager.h"

static HWND hListView = NULL;

void InitTabProcessContent(HWND hwndParent) {
    hListView = CreateWindowEx(
        0, WC_LISTVIEW, _T("Processes"),
        WS_CHILD | WS_VISIBLE | LVS_REPORT,
        10, 30, 780, 450,
        hwndParent, (HMENU)1, GetModuleHandle(NULL), NULL);

    // Create columns for the ListView
    LVCOLUMN lvCol;
    ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lvCol.cx = 80; lvCol.pszText = _T("PID"); ListView_InsertColumn(hListView, 0, &lvCol);
    lvCol.cx = 120; lvCol.pszText = _T("Session ID"); ListView_InsertColumn(hListView, 1, &lvCol);
    lvCol.cx = 120; lvCol.pszText = _T("Name"); ListView_InsertColumn(hListView, 2, &lvCol);
    lvCol.cx = 180; lvCol.pszText = _T("Platform"); ListView_InsertColumn(hListView, 3, &lvCol);
    lvCol.cx = 100; lvCol.pszText = _T("CPU Usage"); ListView_InsertColumn(hListView, 4, &lvCol);
    lvCol.cx = 100; lvCol.pszText = _T("RAM Usage"); ListView_InsertColumn(hListView, 5, &lvCol);
    lvCol.cx = 200; lvCol.pszText = _T("Command Line"); ListView_InsertColumn(hListView, 6, &lvCol);
    lvCol.cx = 120; lvCol.pszText = _T("Network Speed"); ListView_InsertColumn(hListView, 7, &lvCol);

    UpdateProcessList(hListView);
}
