#ifndef MENU_H
#define MENU_H

#include <windows.h>
#define TIMER_ID 1
#define ID_FILE_RUN                1001
#define ID_FILE_REPLACE_TASKMANAGER 1002
#define ID_FILE_EXIT               1003

#define ID_SPEED_SLOW              2001
#define ID_SPEED_NORMAL            2002
#define ID_SPEED_FAST              2003

#define ID_TOOLS_WINDOWS_SETTINGS  5001
#define ID_TOOLS_CONTROL_PANEL     5002
#define ID_TOOLS_DATE_TIME         5003
#define ID_TOOLS_DEVICE_MANAGER    5004
#define ID_TOOLS_GROUP_POLICY      5005
#define ID_TOOLS_SERVICES          5006
#define ID_TOOLS_POWER_OPTIONS     5007

#define ID_SHUTDOWN_SHUTDOWN       3001
#define ID_SHUTDOWN_RESTART        3002
#define ID_SHUTDOWN_LOGOFF         3003
#define ID_SHUTDOWN_LOCK           3004

#define ID_ABOUT                   4001

void AddMenus(HWND hwnd);
void HandleMenuCommand(HWND hwnd, int command);

#endif // MENU_H
