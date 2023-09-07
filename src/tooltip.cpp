#include "tooltip.h"

#ifdef _WIN32
#include "windows/uipriv_windows.hpp"
#include "string_utils.h"

static uintptr_t CreateTooltip(HWND hparent, const wchar_t* text) {
    HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hparent, NULL, hInstance, NULL);

    if (!hwndTT) {
        printf("ERROR: Failed to create tooltip window.");
        return NULL;
    }

    SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    TTTOOLINFO ti = { 0 };
    ti.cbSize = sizeof(ti);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = hparent;
    ti.hinst = hInstance;
    ti.lpszText = (LPWSTR)text;
    GetClientRect(hparent, &ti.rect);
    ti.rect.right = 400;

    if (!SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti)) {
        printf("ERROR: Failed to set rect to tooltip window.");
        DestroyTooltip((uintptr_t)hwndTT);
        return NULL;
    }
    return (uintptr_t)hwndTT;
}

uintptr_t SpinboxSetTooltip(uiSpinbox* s, const char* text) {
    std::wstring wtext = UTF8toUTF16(text);
    HWND child = FindWindowExW((HWND)uiControlHandle(uiControl(s)), NULL, L"edit", NULL);
    if (!child) {
        printf("ERROR: Failed to get text entry from spinbox.");
        return NULL;
    }
    return CreateTooltip(child, wtext.c_str());
}

uintptr_t ControlSetTooltip(uiControl* c, const char* text) {
    std::wstring wtext = UTF8toUTF16(text);
    return CreateTooltip((HWND)uiControlHandle(c), wtext.c_str());
}

void DestroyTooltip(uintptr_t tooltip) {
    if (!DestroyWindow((HWND)tooltip))
        printf("Failed to destroy tooltip window.");
}

#elif defined(__linux__)
uintptr_t SpinboxSetTooltip(uiSpinbox* c, const char* text) { return NULL; }
uintptr_t ControlSetTooltip(uiControl* c, const char* text) { return NULL; }
void DestroyTooltip(uintptr_t tooltip) {}
#else
uintptr_t SpinboxSetTooltip(uiSpinbox* c, const char* text) { return NULL; }
uintptr_t ControlSetTooltip(uiControl* c, const char* text) { return NULL; }
void DestroyTooltip(uintptr_t tooltip) {}
#endif
