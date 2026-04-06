module;
#include <Windows.h>
#include <WinUser.h>
#include <iostream>
export module parsetree_app;

struct parsetree_win {
    HWND top_win;
    HWND help_button, settings_button, examples_button;
};

struct parsetree_border {
    RECT left, right, top, bottom;
};

export const char* CLASS_NAME = "parsetree_class";
static constexpr int FIXED_POS_MODIFIER = 30;
static parsetree_win parsetree_w;
static parsetree_border parsetree_borders = {
    .left = {0, 0, 0, 0},
    .right = {0, 0, 0, 0},
    .top = {0, 0, 0, 0},
    .bottom = {0, 0, 0, 0}
};
static bool borders_drawn = false;
// min max info of window size
static MINMAXINFO* mmi = NULL;

// paint initializers
static PAINTSTRUCT ps;
static HDC hdc;

static void create_parsetree_properties(HWND hwnd) {
    LONG units = GetDialogBaseUnits();
    RECT rect;
    int win_w=0, win_h=0;
    if (GetWindowRect(hwnd, &rect)) { win_w = (int)(rect.right - rect.left); win_h = (int)(rect.bottom - rect.top); }
    if (win_w == 0 || win_h == 0) return;
    int button_w = MulDiv(LOWORD(units), 50, 4), button_h = MulDiv(HIWORD(units), 14, 8);

    int border_left = parsetree_borders.left.left;
    int border_right = parsetree_borders.right.right;

    // help button
    parsetree_w.help_button = CreateWindow(
        "BUTTON",
        "Help",
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        border_left, 0, button_w, button_h,
        hwnd, NULL, NULL, NULL // 2nd null allows button to make actions.
    );

    // examples button
    parsetree_w.examples_button = CreateWindow(
        "BUTTON",
        "Examples",
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        border_left + button_w, 0, button_w, button_h,
        hwnd, NULL, NULL, NULL
    );
}

static void update_parsetree_properties(HWND hwnd) {

    LONG units = GetDialogBaseUnits();
    RECT rect;
    int win_w = 0, win_h = 0;
    if (GetWindowRect(hwnd, &rect)) { win_w = (int)(rect.right - rect.left); win_h = (int)(rect.bottom - rect.top); }
    if (win_w == 0 || win_h == 0) return;
    int button_w = MulDiv(LOWORD(units), 50, 4), button_h = MulDiv(HIWORD(units), 14, 8);
    
    int border_left = parsetree_borders.left.left;
    int border_right = parsetree_borders.right.right;
    
    // update all the child windows
    SetWindowPos(
        parsetree_w.help_button,
        HWND_TOP,
        border_left, 0, button_w, button_h,
        SWP_SHOWWINDOW
    );

    SetWindowPos(
        parsetree_w.examples_button,
        HWND_TOP,
        border_left + button_w, 0, button_w, button_h,
        SWP_SHOWWINDOW
    );
}

static void draw_borders(HDC hdc, HWND hwnd) {
    if (borders_drawn) return;
    RECT win_rect;
    // need client area instead of window area
    GetClientRect(hwnd, &win_rect);
    // draw rect edges to the borders
    parsetree_borders.left = {
        .left = 0,
        .top = 0,
        .right = 30,
        .bottom = win_rect.bottom
    };
    parsetree_borders.right = {
        .left = win_rect.right - 30,
        .top = 0,
        .right = win_rect.right,
        .bottom = win_rect.bottom
    };
    DrawEdge(hdc, &parsetree_borders.left, EDGE_ETCHED, BF_RECT);
    DrawEdge(hdc, &parsetree_borders.right, EDGE_ETCHED, BF_RECT);
    borders_drawn = true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    mmi = (MINMAXINFO*)lParam;
    switch (msg) {
    case WM_CREATE:
        std::cout << "creating!" << std::endl;
        create_parsetree_properties(hwnd);
        break;
    case WM_PAINT:
        std::cout << "Painting!" << std::endl;
        hdc = BeginPaint(hwnd, &ps);
        draw_borders(hdc, hwnd);
        EndPaint(hwnd, &ps);
        break;
    case WM_SIZING:
    case WM_SIZE:
        std::cout << "updating!" << std::endl; // also scale the properties to the window size (not just movement)
        update_parsetree_properties(hwnd);
        break;
    case WM_CLOSE:
        std::cout << "closing!" << std::endl;
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_GETMINMAXINFO:
        mmi->ptMinTrackSize.x = 800;
        mmi->ptMinTrackSize.y = 600;
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Exported function that creates a window
static HWND create_window(HINSTANCE hInstance, int nShowCmd) {
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
        WS_OVERLAPPEDWINDOW | WS_THICKFRAME,
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
    parsetree_w.top_win = create_window(hInstance, SW_SHOW);
    if (!parsetree_w.top_win) return;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}