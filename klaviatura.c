
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

HHOOK keyboardHook;

// Unicode belgini yozish
void yozish(wchar_t belgi) {
    INPUT input[2] = {0};

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wScan = belgi;
    input[0].ki.dwFlags = KEYEVENTF_UNICODE;

    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wScan = belgi;
    input[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;

    SendInput(2, input, sizeof(INPUT));
}

// Tugmalarni kuzatish
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *key = (KBDLLHOOKSTRUCT *)lParam;

        // Dastur o'zi yuborgan belgilarni qayta ishlamaymiz
        if (key->flags & LLKHF_INJECTED)
            return CallNextHookEx(keyboardHook, nCode, wParam, lParam);

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            int shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

            wchar_t belgi = 0;

            switch (key->vkCode) {
                case 'W':
                case 'w':
                    belgi = shift ? L'Ş' : L'ş';
                    break;

                case 'C':
                case 'c':
                    belgi = shift ? L'Ç' : L'ç';
                    break;

                case VK_OEM_4: // [
                    belgi = shift ? L'Ō' : L'ō';
                    break;

                case VK_OEM_6: // ]
                    belgi = shift ? L'Ḡ' : L'ḡ';
                    break;
            }

            if (belgi != 0) {
                yozish(belgi);
                return 1;
            }
        }

        // Asl tugma bosilishi va qo'yilishini to'xtatamiz
        if (key->vkCode == VK_OEM_4 ||
            key->vkCode == VK_OEM_6 ) {

            if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
                return 1;
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

int main() {
    printf("Yangi o'zbek klaviaturasi ishga tushdi!\n");
    printf(" w  = sh\n");
    printf(" c  = ch\n");
    printf(" [  = o belgisi\n");
    printf(" ]  = g belgisi\n");
    printf("\nChiqish uchun Ctrl + Alt + F12 bosing.\n");

    keyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        KeyboardProc,
        GetModuleHandle(NULL),
        0
    );

    if (keyboardHook == NULL) {
        printf("Klaviaturani ulashda xatolik!\n");
        return 1;
    }

    // Chiqish uchun maxsus tugmalar birikmasi
    if (!RegisterHotKey(NULL, 1, MOD_CONTROL | MOD_ALT, VK_F12)) {
        printf("Chiqish tugmasini ro'yxatdan o'tkazib bo'lmadi!\n");
        UnhookWindowsHookEx(keyboardHook);
        return 1;
    }

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == 1)
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterHotKey(NULL, 1);
    UnhookWindowsHookEx(keyboardHook);

    printf("\nKlaviatura odatiy holatga qaytdi.\n");

    return 0;
}

