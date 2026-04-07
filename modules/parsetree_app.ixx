module;
#include <Windows.h>
#include <WinUser.h>
#include <iostream>
#include <array>
export module parsetree_app;

enum button_action_id {
    ACTION_HELP=1
};

// the top level windows and its contents
struct parsetree_win {
    HWND top_win;
    HWND help_button, settings_button, examples_button;
};

struct parsetree_border {
    RECT left, right, top, bottom;
};

// the secondary windows obtained through top-level button action
struct parsetree_button_win {
    HWND help_button_window;
    HWND settings_button_window;
    HWND examples_button_window;
};

// the properties of the secondary windows
struct help_win_contents {

};



// button window class names
const char* HELPWIN_CLASS_NAME = "help_window";
const char* SETTINGSWIN_CLASS_NAME = "settings_window";
const char* EXAMPLESWIN_CLASS_NAME = "examples_window";

// initial configuration stuff
static HINSTANCE g_hInstance;
const char* CLASS_NAME = "parsetree_class";
static constexpr int FIXED_POS_MODIFIER = 30;
static parsetree_win parsetree_w;
static parsetree_border parsetree_borders{};
static parsetree_button_win button_windows;
// min max info of window size
static MINMAXINFO* top_mmi = nullptr;
static MINMAXINFO* help_mmi = nullptr;

// paint initializers
static PAINTSTRUCT ps;
static HDC hdc;
static WNDCLASSEX wc{};

// rect usage
static RECT rc;

static void init_borders(HWND hwnd) {
    RECT win_rect;
    // need client area instead of window area
    GetClientRect(hwnd, &win_rect);
    // draw rect edges to the borders
    parsetree_borders.left = {
        .left = 0,
        .top = 0,
        .right = FIXED_POS_MODIFIER,
        .bottom = win_rect.bottom
    };
    parsetree_borders.right = {
        .left = win_rect.right - FIXED_POS_MODIFIER,
        .top = 0,
        .right = win_rect.right,
        .bottom = win_rect.bottom
    };
    parsetree_borders.top = {
        .left = FIXED_POS_MODIFIER,
        .top = 0,
        .right = win_rect.right - FIXED_POS_MODIFIER,
        .bottom = FIXED_POS_MODIFIER
    };
    parsetree_borders.bottom = {
        .left = FIXED_POS_MODIFIER,
        .top = win_rect.bottom - FIXED_POS_MODIFIER,
        .right = win_rect.right - FIXED_POS_MODIFIER,
        .bottom = win_rect.bottom
    };
}

static void update_help_window() {

}

static LRESULT CALLBACK help_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    help_mmi = (MINMAXINFO*)lParam;
    switch (uMsg) {
        case WM_CLOSE:
            std::cout << "closing help window!" << std::endl;
            ShowWindow(hwnd, SW_HIDE);
            break;
        case WM_PAINT:
            std::cout << "Painting help window!" << std::endl;
            break;
        case WM_SIZING:
        case WM_SIZE:
            std::cout << "Resizing help window!" << std::endl;
            break;
        case WM_DESTROY:
            break;
        case WM_GETMINMAXINFO:
            help_mmi->ptMinTrackSize.x = 400;
            help_mmi->ptMinTrackSize.y = 300;
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

static void create_help_window(HWND hwnd) {
    WNDCLASSEX help_win_class = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = 0,
        .lpfnWndProc = help_proc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = g_hInstance,
        .hIcon = LoadIcon(NULL, IDI_APPLICATION),
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName = NULL,
        .lpszClassName = HELPWIN_CLASS_NAME,
        .hIconSm = LoadIcon(NULL, IDI_APPLICATION),
    };
    if (!RegisterClassEx(&help_win_class)) {
        MessageBox(NULL, "Window registration failed.", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    button_windows.help_button_window = CreateWindowEx(
        0,
        HELPWIN_CLASS_NAME,
        "help",
        WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        hwnd, NULL, g_hInstance, NULL
    );

    if (!button_windows.help_button_window) {
        MessageBox(NULL, "Window creation failed.", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    ShowWindow(button_windows.help_button_window, SW_SHOWNORMAL);
    UpdateWindow(button_windows.help_button_window);
}

static void handle_button_actions(HWND hwnd, WPARAM wParam) {
    switch (LOWORD(wParam)) {
        case ACTION_HELP:
            if (!IsWindow(button_windows.help_button_window)) create_help_window(hwnd);
            else {
                ShowWindow(button_windows.help_button_window, SW_SHOWNORMAL);
                UpdateWindow(button_windows.help_button_window);
                SetForegroundWindow(button_windows.help_button_window);
            }
            break;
    }
}

static void create_parsetree_properties(HWND hwnd) {
    LONG units = GetDialogBaseUnits();
    RECT rect;
    int win_w=0, win_h=0;
    if (GetWindowRect(hwnd, &rect)) { win_w = (int)(rect.right - rect.left); win_h = (int)(rect.bottom - rect.top); }
    if (win_w == 0 || win_h == 0) return;
    int button_w = MulDiv(LOWORD(units), 50, 4), button_h = MulDiv(HIWORD(units), 14, 8);

    init_borders(hwnd);
    int border_left = parsetree_borders.left.right;
    int border_right = parsetree_borders.right.right;
    int border_top = parsetree_borders.top.bottom;
    int border_bottom = parsetree_borders.bottom.bottom;

    // help button
    parsetree_w.help_button = CreateWindow(
        "BUTTON",
        "Help",
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        border_left, border_top, button_w, button_h,
        hwnd, (HMENU)ACTION_HELP, NULL, NULL // 2nd null allows button to make actions.
    );

    // examples button
    parsetree_w.examples_button = CreateWindow(
        "BUTTON",
        "Examples",
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        border_left + button_w, border_top, button_w, button_h,
        hwnd, NULL, NULL, NULL
    );
}

static void update_parsetree_properties(HWND hwnd) {

    LONG units = GetDialogBaseUnits();
    RECT rect;
    int win_w = 0, win_h = 0;
    if (GetClientRect(hwnd, &rect)) { win_w = (int)(rect.right - rect.left); win_h = (int)(rect.bottom - rect.top); }
    if (win_w == 0 || win_h == 0) return;
    int button_w = MulDiv(LOWORD(units), 50, 4), button_h = MulDiv(HIWORD(units), 14, 8);

    init_borders(hwnd);

    int border_left = parsetree_borders.left.right;
    int border_right = parsetree_borders.right.right;
    int border_top = parsetree_borders.top.bottom;
    int border_bottom = parsetree_borders.bottom.bottom;


    // update all the child windows
    SetWindowPos(
        parsetree_w.help_button,
        HWND_TOP,
        border_left, border_top, button_w, button_h,
        SWP_SHOWWINDOW
    );

    SetWindowPos(
        parsetree_w.examples_button,
        HWND_TOP,
        border_left + button_w, border_top, button_w, button_h,
        SWP_SHOWWINDOW
    );
}

static void draw_borders(HDC hdc, HWND hwnd) {
    FillRect(hdc, &parsetree_borders.left, (HBRUSH)(COLOR_3DFACE + 1));
    FillRect(hdc, &parsetree_borders.right, (HBRUSH)(COLOR_3DFACE + 1));
    FillRect(hdc, &parsetree_borders.top, (HBRUSH)(COLOR_3DFACE + 1));
    FillRect(hdc, &parsetree_borders.bottom, (HBRUSH)(COLOR_3DFACE + 1));
    DrawEdge(hdc, &parsetree_borders.left, EDGE_ETCHED, BF_RECT);
    DrawEdge(hdc, &parsetree_borders.right, EDGE_ETCHED, BF_RECT);
    DrawEdge(hdc, &parsetree_borders.top, EDGE_ETCHED, BF_RECT);
    DrawEdge(hdc, &parsetree_borders.bottom, EDGE_ETCHED, BF_RECT);
}

static LRESULT CALLBACK top_level_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    top_mmi = (MINMAXINFO*)lParam;
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
    case WM_COMMAND:
        // button actions
        handle_button_actions(hwnd, wParam);
        break;
    case WM_SIZING:
    case WM_SIZE:
        std::cout << "updating!" << std::endl; // also scale the properties to the window size (not just movement)
        update_parsetree_properties(hwnd);
        InvalidateRect(hwnd, NULL, true);
        break;
    case WM_CLOSE:
        std::cout << "closing!" << std::endl;
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_GETMINMAXINFO:
        top_mmi->ptMinTrackSize.x = 800;
        top_mmi->ptMinTrackSize.y = 600;
        break;
    case WM_ERASEBKGND:
        std::cout << "clearing!" << std::endl;
        hdc = (HDC)wParam;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));
        return 1; // handled erasing.
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Exported function that creates a window
static HWND create_window(int nShowCmd) {
    WNDCLASSEX wc = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = 0,
        .lpfnWndProc = top_level_wnd_proc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = g_hInstance,
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
        NULL, NULL, g_hInstance, NULL
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
    g_hInstance = GetModuleHandle(NULL);
    parsetree_w.top_win = create_window(SW_SHOW);
    if (!parsetree_w.top_win) return;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}