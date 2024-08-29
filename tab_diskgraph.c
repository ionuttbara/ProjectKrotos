#include "tab_diskgraph.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>

// Function prototypes
void DrawDiskGraph(HDC hdc);
void UpdateDiskInfo(HWND hwnd);
void GetDiskInfo(TCHAR* buffer, size_t size);
TCHAR* FormatBytes(ULONGLONG bytes);  // Declaration of FormatBytes

// Global handle
static HWND hDiskGraph = NULL;
static HWND hDiskInfo = NULL;

// Initialize the Disk Graph tab content
void InitTabDiskGraphContent(HWND hwndParent) {
    // Create a static control for the Disk graph placeholder
    hDiskGraph = CreateWindow(TEXT("STATIC"), _T("Disk Graph Placeholder"),
        WS_CHILD | WS_VISIBLE | SS_CENTER | SS_LEFT,
        10, 30, 550, 450,
        hwndParent, NULL, GetModuleHandle(NULL), NULL);

    // Create a static control for the Disk info
    hDiskInfo = CreateWindow(TEXT("STATIC"), _T(""),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        570, 30, 200, 450,
        hwndParent, NULL, GetModuleHandle(NULL), NULL);

    UpdateDiskInfo(hwndParent); // Initialize Disk information display
}

// Update the Disk graph and information
void UpdateDiskGraphContent(HWND hwndParent) {
    HDC hdc = GetDC(hDiskGraph);
    if (hdc) {
        DrawDiskGraph(hdc);
        ReleaseDC(hDiskGraph, hdc);
    }

    UpdateDiskInfo(hwndParent);
}

// Draw a placeholder graph (to be replaced with actual graphing logic)
void DrawDiskGraph(HDC hdc) {
    RECT rect;
    GetClientRect(hDiskGraph, &rect);

    // Clear the background
    FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

    // Example of drawing a simple rectangle (replace with actual graph drawing code)
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
    RECT graphRect = { 10, 10, rect.right - 10, rect.bottom - 10 };
    FillRect(hdc, &graphRect, hBrush);
    DeleteObject(hBrush);
}

// Update the Disk information display
void UpdateDiskInfo(HWND hwndParent) {
    TCHAR info[1024];
    GetDiskInfo(info, sizeof(info) / sizeof(TCHAR));

    SetWindowText(hDiskInfo, info);
}

// Get detailed Disk information
void GetDiskInfo(TCHAR* buffer, size_t size) {
    TCHAR driveLetter[4] = {0};
    DWORD sectorsPerCluster, bytesPerSector, numberOfFreeClusters, totalNumberOfClusters;
    ULONGLONG totalBytes, freeBytes;
    TCHAR diskType[10] = {0};

    // Start building the disk info string
    StringCchCopy(buffer, size, _T("Disk Information\n\n"));

    // Loop through each drive letter from A to Z
    for (char drive = 'A'; drive <= 'Z'; ++drive) {
        driveLetter[0] = drive;
        driveLetter[1] = ':';
        driveLetter[2] = '\\';
        driveLetter[3] = '\0';

        // Check if the drive exists
        if (GetDiskFreeSpace(driveLetter, &sectorsPerCluster, &bytesPerSector, &numberOfFreeClusters, &totalNumberOfClusters)) {
            totalBytes = (ULONGLONG)totalNumberOfClusters * sectorsPerCluster * bytesPerSector;
            freeBytes = (ULONGLONG)numberOfFreeClusters * sectorsPerCluster * bytesPerSector;

            // Get the drive type
            UINT type = GetDriveType(driveLetter);
            switch (type) {
                case DRIVE_UNKNOWN:
                    StringCchPrintf(diskType, sizeof(diskType)/sizeof(TCHAR), _T("Unknown"));
                    break;
                case DRIVE_NO_ROOT_DIR:
                    StringCchPrintf(diskType, sizeof(diskType)/sizeof(TCHAR), _T("No Root Dir"));
                    break;
                case DRIVE_REMOVABLE:
                    StringCchPrintf(diskType, sizeof(diskType)/sizeof(TCHAR), _T("Removable"));
                    break;
                case DRIVE_FIXED:
                    StringCchPrintf(diskType, sizeof(diskType)/sizeof(TCHAR), _T("Fixed"));
                    break;
                case DRIVE_REMOTE:
                    StringCchPrintf(diskType, sizeof(diskType)/sizeof(TCHAR), _T("Network"));
                    break;
                case DRIVE_CDROM:
                    StringCchPrintf(diskType, sizeof(diskType)/sizeof(TCHAR), _T("CD-ROM"));
                    break;
                case DRIVE_RAMDISK:
                    StringCchPrintf(diskType, sizeof(diskType)/sizeof(TCHAR), _T("RAM Disk"));
                    break;
                default:
                    StringCchPrintf(diskType, sizeof(diskType)/sizeof(TCHAR), _T("Unknown"));
                    break;
            }

            // Append drive information to the buffer
            StringCchCat(buffer, size, _T("Drive "));
            StringCchCat(buffer, size, driveLetter);
            StringCchCat(buffer, size, _T("\nTotal Size: "));
            StringCchCat(buffer, size, FormatBytes(totalBytes));
            StringCchCat(buffer, size, _T("\nFree Space: "));
            StringCchCat(buffer, size, FormatBytes(freeBytes));
            StringCchCat(buffer, size, _T("\nType: "));
            StringCchCat(buffer, size, diskType);
            StringCchCat(buffer, size, _T("\n\n"));
        }
    }
}

// Helper function to format bytes to human-readable format
TCHAR* FormatBytes(ULONGLONG bytes) {
    static TCHAR formattedSize[50];
    if (bytes < 1024) {
        StringCchPrintf(formattedSize, sizeof(formattedSize)/sizeof(TCHAR), _T("%llu bytes"), bytes);
    } else if (bytes < 1024 * 1024) {
        StringCchPrintf(formattedSize, sizeof(formattedSize)/sizeof(TCHAR), _T("%.2f KB"), (double)bytes / 1024);
    } else if (bytes < 1024 * 1024 * 1024) {
        StringCchPrintf(formattedSize, sizeof(formattedSize)/sizeof(TCHAR), _T("%.2f MB"), (double)bytes / (1024 * 1024));
    } else {
        StringCchPrintf(formattedSize, sizeof(formattedSize)/sizeof(TCHAR), _T("%.2f GB"), (double)bytes / (1024 * 1024 * 1024));
    }
    return formattedSize;
}
