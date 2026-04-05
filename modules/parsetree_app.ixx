module;
#include <Windows.h>
#include <iostream>
export module parsetree_app;

export const char* CLASS_NAME = "parsetree_class";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Exported function that creates a window
export HWND create_window(HINSTANCE hInstance, int nShowCmd) {
    WNDCLASSEX wc = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = 0,
        .lpfnWndProc = WndProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = hInstance,
        .hIcon = LoadIcon(NULL, IDI_APPLICATION),
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName = NULL,
        .lpszClassName = CLASS_NAME,
        .hIconSm = LoadIcon(NULL, IDI_APPLICATION)
    };

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window registration failed.", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "parse tree visualizer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        MessageBox(NULL, "Window creation failed.", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }

    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    return hwnd;
}

export void parsetree_app() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HWND hwnd = create_window(hInstance, SW_SHOW);
    if (!hwnd) return;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}