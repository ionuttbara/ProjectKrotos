#include "tab_cpugraph.h"
#include <windows.h>
#include <tchar.h>
#include <psapi.h>
#include <stdio.h>

// Function prototypes
void DrawCpuGraph(HDC hdc);
void UpdateCpuInfo(HWND hwnd);
void GetCpuInfo(TCHAR* buffer, size_t size);
DWORD CountBits(ULONG_PTR bitmask);
BOOL GetCacheSize(DWORD level, DWORD* size);
TCHAR* FormatCacheSize(ULONGLONG size);

// Global handles
static HWND hCpuGraph = NULL;
static HWND hCpuInfo = NULL;

// Initialize the CPU Graph tab content
void InitTabCpuGraphContent(HWND hwndParent) {
    // Create a static control for the CPU graph placeholder
    hCpuGraph = CreateWindow(TEXT("STATIC"), _T("CPU Graph Placeholder"),
        WS_CHILD | WS_VISIBLE | SS_CENTER | SS_LEFT,
        10, 30, 550, 450,
        hwndParent, NULL, GetModuleHandle(NULL), NULL);

    // Create a static control for the CPU info
    hCpuInfo = CreateWindow(TEXT("STATIC"), _T(""),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        570, 30, 200, 450,
        hwndParent, NULL, GetModuleHandle(NULL), NULL);

    UpdateCpuInfo(hwndParent); // Initialize CPU information display
}

// Update the CPU graph and information
void UpdateCpuGraphContent(HWND hwndParent) {
    HDC hdc = GetDC(hCpuGraph);
    if (hdc) {
        DrawCpuGraph(hdc);
        ReleaseDC(hCpuGraph, hdc);
    }

    UpdateCpuInfo(hwndParent);
}

// Draw a placeholder graph (to be replaced with actual graphing logic)
void DrawCpuGraph(HDC hdc) {
    RECT rect;
    GetClientRect(hCpuGraph, &rect);

    // Clear the background
    FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

    // Example of drawing a simple rectangle (replace with actual graph drawing code)
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));
    RECT graphRect = { 10, 10, rect.right - 10, rect.bottom - 10 };
    FillRect(hdc, &graphRect, hBrush);
    DeleteObject(hBrush);
}

// Update the CPU information display
void UpdateCpuInfo(HWND hwndParent) {
    TCHAR info[1024];
    GetCpuInfo(info, sizeof(info) / sizeof(TCHAR));

    SetWindowText(hCpuInfo, info);
}

// Get detailed CPU information
void GetCpuInfo(TCHAR* buffer, size_t size) {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    DWORD bufferSize = 0;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pInfo = NULL;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pCurrent = NULL;
    DWORD returnedSize = 0;
    ULONG_PTR numSockets = 0, numCores = 0, numThreads = 0;
    ULONGLONG l1CacheSize = 0, l2CacheSize = 0, l3CacheSize = 0;

    // Get logical processor information
    if (!GetLogicalProcessorInformation(NULL, &bufferSize) && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        pInfo = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(bufferSize);
        if (pInfo) {
            if (GetLogicalProcessorInformation(pInfo, &returnedSize)) {
                pCurrent = pInfo;
                while ((ULONG_PTR)pCurrent < (ULONG_PTR)pInfo + bufferSize) {
                    if (pCurrent->Relationship == RelationProcessorCore) {
                        numSockets++;
                        numCores += CountBits(pCurrent->ProcessorMask);
                    }
                    pCurrent++;
                }
            }
            free(pInfo);
        }
    }

    // Get processor cache information
    DWORD cacheSize = 0;
    DWORD cacheLevels = 3; // L1, L2, L3
    for (DWORD i = 0; i < cacheLevels; i++) {
        DWORD cacheLevel = i + 1;
        if (GetCacheSize(cacheLevel, &cacheSize)) {
            switch (cacheLevel) {
            case 1:
                l1CacheSize = cacheSize;
                break;
            case 2:
                l2CacheSize = cacheSize;
                break;
            case 3:
                l3CacheSize = cacheSize;
                break;
            }
        }
    }

    // Format the information
    _stprintf(buffer, _T("CPU Graph\n\n")
                       _T("CPU Name: %s\n")
                       _T("Base Speed: %.2f GHz\n")
                       _T("Sockets: %u\n")
                       _T("Cores: %u\n")
                       _T("Threads: %u\n")
                       _T("L1 Cache: %s\n")
                       _T("L2 Cache: %s\n")
                       _T("L3 Cache: %s\n"),
              _T("Processor Name Here"), // Placeholder for CPU name
              3.5, // Placeholder for base speed
              (unsigned int)numSockets,
              (unsigned int)numCores,
              (unsigned int)numThreads,
              FormatCacheSize(l1CacheSize),
              FormatCacheSize(l2CacheSize),
              FormatCacheSize(l3CacheSize));
}

// Helper functions
DWORD CountBits(ULONG_PTR bitmask) {
    DWORD count = 0;
    while (bitmask) {
        count += (bitmask & 1);
        bitmask >>= 1;
    }
    return count;
}

BOOL GetCacheSize(DWORD level, DWORD* size) {
    // Placeholder function to retrieve cache size
    // In practice, you might use specific API or registry access
    *size = 1024 * level * 256; // Example size, in bytes
    return TRUE;
}

TCHAR* FormatCacheSize(ULONGLONG size) {
    static TCHAR formattedSize[50];
    if (size < 1024 * 1024) {
        _stprintf(formattedSize, _T("%llu KB"), size / 1024);
    } else {
        _stprintf(formattedSize, _T("%.2f MB"), (double)size / (1024 * 1024));
    }
    return formattedSize;
}
