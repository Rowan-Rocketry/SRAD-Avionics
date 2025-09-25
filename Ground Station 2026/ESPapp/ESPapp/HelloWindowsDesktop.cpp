//
//#include <windows.h>
//#include <string>
//#include <vector>
//#include <sstream>
//#include <thread>
//#include <atomic>
//#include <iostream>
//
//HINSTANCE hInst;
//HWND hCombo, hConnectBtn, hOutput;
//std::atomic<bool> keepRunning(false);
//std::thread serialThread;
//
//void ListAvailablePorts(HWND combo) {
//    SendMessage(combo, CB_RESETCONTENT, 0, 0);
//
//    for (int i = 1; i <= 20; i++) {
//        std::wstring port = L"\\\\.\\COM" + std::to_wstring(i);
//
//        HANDLE h = CreateFileW(port.c_str(),
//            GENERIC_READ | GENERIC_WRITE,
//            0, NULL, OPEN_EXISTING, 0, NULL);
//
//        if (h != INVALID_HANDLE_VALUE) {
//            CloseHandle(h);
//            std::wstring simplePort = L"COM" + std::to_wstring(i);
//            SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)simplePort.c_str());
//        }
//    }
//
//    if (SendMessage(combo, CB_GETCOUNT, 0, 0) > 0) {
//        SendMessage(combo, CB_SETCURSEL, 0, 0);
//    }
//}
//
//void SerialReader(std::wstring portName) {
//    std::wstring fullPort = L"\\\\.\\" + portName;
//
//    HANDLE hSerial = CreateFileW(fullPort.c_str(),
//        GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
//    if (hSerial == INVALID_HANDLE_VALUE) {
//        MessageBox(NULL, L"Failed to open COM port", L"Error", MB_OK);
//        return;
//    }
//
//    DCB dcbSerialParams = { 0 };
//    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
//
//    if (!GetCommState(hSerial, &dcbSerialParams)) {
//        MessageBox(NULL, L"Error getting COM state", L"Error", MB_OK);
//        CloseHandle(hSerial);
//        return;
//    }
//
//    dcbSerialParams.BaudRate = CBR_115200;
//    dcbSerialParams.ByteSize = 8;
//    dcbSerialParams.StopBits = ONESTOPBIT;
//    dcbSerialParams.Parity = NOPARITY;
//
//    if (!SetCommState(hSerial, &dcbSerialParams)) {
//        MessageBox(NULL, L"Error setting COM state", L"Error", MB_OK);
//        CloseHandle(hSerial);
//        return;
//    }
//
//    char buf[128];
//    DWORD bytesRead;
//
//    while (keepRunning) {
//        if (ReadFile(hSerial, buf, sizeof(buf) - 1, &bytesRead, NULL)) {
//            if (bytesRead > 0) {
//                buf[bytesRead] = '\0';
//                std::wstring wbuf;
//                wbuf.assign(buf, buf + bytesRead);
//                SendMessage(hOutput, LB_ADDSTRING, 0, (LPARAM)wbuf.c_str());
//            }
//        }
//        Sleep(50);
//    }
//
//    CloseHandle(hSerial);
//}
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
//    switch (message) {
//    case WM_CREATE:
//        hCombo = CreateWindow(L"COMBOBOX", NULL,
//            CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
//            10, 10, 150, 200, hWnd, (HMENU)1001, hInst, NULL);
//
//        hConnectBtn = CreateWindow(L"BUTTON", L"Connect",
//            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
//            170, 10, 80, 25, hWnd, (HMENU)1002, hInst, NULL);
//
//        hOutput = CreateWindow(L"LISTBOX", NULL,
//            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
//            10, 50, 400, 300, hWnd, (HMENU)1003, hInst, NULL);
//
//        ListAvailablePorts(hCombo);
//        break;
//
//    case WM_COMMAND:
//        if (LOWORD(wParam) == 1002) { // Connect button
//            if (!keepRunning) {
//                int sel = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
//                if (sel != CB_ERR) {
//                    wchar_t portName[20];
//                    SendMessage(hCombo, CB_GETLBTEXT, sel, (LPARAM)portName);
//
//                    keepRunning = true;
//                    serialThread = std::thread(SerialReader, std::wstring(portName));
//                    SetWindowText(hConnectBtn, L"Disconnect");
//                }
//            }
//            else { // Disconnect
//                keepRunning = false;
//                if (serialThread.joinable()) serialThread.join();
//                SetWindowText(hConnectBtn, L"Connect");
//            }
//        }
//        break;
//
//    case WM_DESTROY:
//        keepRunning = false;
//        if (serialThread.joinable()) serialThread.join();
//        PostQuitMessage(0);
//        break;
//
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//    return 0;
//}
//
//int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//    LPWSTR lpCmdLine, int nCmdShow) {
//
//    hInst = hInstance;
//    WNDCLASS wc = { };
//    wc.lpfnWndProc = WndProc;
//    wc.hInstance = hInstance;
//    wc.lpszClassName = L"ESPWinApp";
//    RegisterClass(&wc);
//
//    HWND hWnd = CreateWindow(L"ESPWinApp", L"Lobter Finder",
//        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 450, 400,
//        NULL, NULL, hInstance, NULL);
//
//    if (!hWnd) return FALSE;
//
//    ShowWindow(hWnd, nCmdShow);
//    UpdateWindow(hWnd);
//
//    MSG msg;
//    while (GetMessage(&msg, NULL, 0, 0)) {
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//
//    return (int)msg.wParam;
//}



// DualESPBridge.cpp
#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <atomic>
#include <iostream>

HINSTANCE hInst;
HWND hCombo1, hCombo2, hConnectBtn1, hConnectBtn2, hOutput1, hOutput2;
std::atomic<bool> keepRunning1(false), keepRunning2(false);
std::thread serialThread1, serialThread2;

void ListAvailablePorts(HWND combo) {
    SendMessage(combo, CB_RESETCONTENT, 0, 0);

    for (int i = 1; i <= 30; i++) {
        std::wstring port = L"\\\\.\\COM" + std::to_wstring(i);

        HANDLE h = CreateFileW(port.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0, NULL, OPEN_EXISTING, 0, NULL);

        if (h != INVALID_HANDLE_VALUE) {
            CloseHandle(h);
            std::wstring simplePort = L"COM" + std::to_wstring(i);
            SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)simplePort.c_str());
        }
    }

    if (SendMessage(combo, CB_GETCOUNT, 0, 0) > 0) {
        SendMessage(combo, CB_SETCURSEL, 0, 0);
    }
}

// convert UTF-8 buffer (char*) to wide string
std::wstring Utf8ToWString(const char* buf, int len) {
    if (len <= 0) return L"";
    int needed = MultiByteToWideChar(CP_UTF8, 0, buf, len, NULL, 0);
    if (needed <= 0) return L"";
    std::wstring w;
    w.resize(needed);
    MultiByteToWideChar(CP_UTF8, 0, buf, len, &w[0], needed);
    return w;
}

void SerialReader(std::wstring portName, HWND hList, std::atomic<bool>& runningFlag) {
    std::wstring fullPort = L"\\\\.\\" + portName;

    HANDLE hSerial = CreateFileW(fullPort.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::wstring msg = L"Failed to open " + portName;
        MessageBox(NULL, msg.c_str(), L"Error", MB_OK);
        runningFlag = false;
        return;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        MessageBox(NULL, L"Error getting COM state", L"Error", MB_OK);
        CloseHandle(hSerial);
        runningFlag = false;
        return;
    }

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        MessageBox(NULL, L"Error setting COM state", L"Error", MB_OK);
        CloseHandle(hSerial);
        runningFlag = false;
        return;
    }

    // set reasonable timeouts
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    char buf[1024];
    DWORD bytesRead;

    while (runningFlag) {
        if (ReadFile(hSerial, buf, sizeof(buf) - 1, &bytesRead, NULL)) {
            if (bytesRead > 0) {
                // convert UTF-8 to wide and add to listbox
                std::wstring wbuf = Utf8ToWString(buf, (int)bytesRead);
                // trim trailing CR/LF
                while (!wbuf.empty() && (wbuf.back() == L'\n' || wbuf.back() == L'\r')) wbuf.pop_back();
                if (!wbuf.empty()) {
                    SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)wbuf.c_str());
                    // scroll to bottom
                    int count = (int)SendMessage(hList, LB_GETCOUNT, 0, 0);
                    if (count > 0) SendMessage(hList, LB_SETCURSEL, count - 1, 0);
                }
            }
        }
        else {
            // read error: break out
            break;
        }
        Sleep(20);
    }

    CloseHandle(hSerial);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        // Combo + connect for ESP 1
        hCombo1 = CreateWindow(L"COMBOBOX", NULL,
            CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
            10, 10, 150, 200, hWnd, (HMENU)1001, hInst, NULL);

        hConnectBtn1 = CreateWindow(L"BUTTON", L"Connect 1",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            170, 10, 90, 25, hWnd, (HMENU)1002, hInst, NULL);

        hOutput1 = CreateWindow(L"LISTBOX", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
            10, 50, 400, 150, hWnd, (HMENU)1003, hInst, NULL);

        // Combo + connect for ESP 2
        hCombo2 = CreateWindow(L"COMBOBOX", NULL,
            CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
            10, 210, 150, 200, hWnd, (HMENU)1011, hInst, NULL);

        hConnectBtn2 = CreateWindow(L"BUTTON", L"Connect 2",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            170, 210, 90, 25, hWnd, (HMENU)1012, hInst, NULL);

        hOutput2 = CreateWindow(L"LISTBOX", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
            10, 250, 400, 150, hWnd, (HMENU)1013, hInst, NULL);

        ListAvailablePorts(hCombo1);
        ListAvailablePorts(hCombo2);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1002) { // Connect 1
            if (!keepRunning1) {
                int sel = SendMessage(hCombo1, CB_GETCURSEL, 0, 0);
                if (sel != CB_ERR) {
                    wchar_t portName[20];
                    SendMessage(hCombo1, CB_GETLBTEXT, sel, (LPARAM)portName);

                    keepRunning1 = true;
                    serialThread1 = std::thread(SerialReader, std::wstring(portName), hOutput1, std::ref(keepRunning1));
                    SetWindowText(hConnectBtn1, L"Disconnect 1");
                }
            }
            else { // Disconnect 1
                keepRunning1 = false;
                if (serialThread1.joinable()) serialThread1.join();
                SetWindowText(hConnectBtn1, L"Connect 1");
            }
        }
        else if (LOWORD(wParam) == 1012) { // Connect 2
            if (!keepRunning2) {
                int sel = SendMessage(hCombo2, CB_GETCURSEL, 0, 0);
                if (sel != CB_ERR) {
                    wchar_t portName[20];
                    SendMessage(hCombo2, CB_GETLBTEXT, sel, (LPARAM)portName);

                    keepRunning2 = true;
                    serialThread2 = std::thread(SerialReader, std::wstring(portName), hOutput2, std::ref(keepRunning2));
                    SetWindowText(hConnectBtn2, L"Disconnect 2");
                }
            }
            else { // Disconnect 2
                keepRunning2 = false;
                if (serialThread2.joinable()) serialThread2.join();
                SetWindowText(hConnectBtn2, L"Connect 2");
            }
        }
        break;

    case WM_DESTROY:
        keepRunning1 = false;
        keepRunning2 = false;
        if (serialThread1.joinable()) serialThread1.join();
        if (serialThread2.joinable()) serialThread2.join();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine, int nCmdShow) {

    hInst = hInstance;
    WNDCLASS wc = { };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ESPWinAppDual";
    RegisterClass(&wc);

    HWND hWnd = CreateWindow(L"ESPWinAppDual", L"Lobter Finder - Dual",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 440, 470,
        NULL, NULL, hInstance, NULL);

    if (!hWnd) return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
