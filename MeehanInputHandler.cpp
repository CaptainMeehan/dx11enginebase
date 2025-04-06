#pragma once
#include "MeehanInputHandler.h"
#include <Windows.h>
#include <optional>


CommonUtilities::InputHandler::InputHandler()
{
    // you can #include <hidusage.h> for these defines
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

    RAWINPUTDEVICE Rid[1];
    Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    Rid[0].dwFlags = RIDEV_INPUTSINK;
    Rid[0].hwndTarget = GetHWND();
    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

    myCurrentMouseWheelDelta = 0;
    myUpdatedMouseWheelDelta = 0;
}

void CommonUtilities::InputHandler::Update()
{
    POINT point = GetMousePosition(GetHWND());
    myUpdatedMousePos = point;

    myLastMousePos = myCurrentMousePos;
    myCurrentMousePos = myUpdatedMousePos;

    myLastMouseDelta = myCurrentMouseDelta;
    myCurrentMouseDelta = { 0, 0 };
    myCurrentMouseDelta.x = myLastMousePos.x - myCurrentMousePos.x;
    myCurrentMouseDelta.y = myLastMousePos.y - myCurrentMousePos.y;

    myCurrentMouseWheelDelta = myUpdatedMouseWheelDelta == 0.0f ? 0.0f : myUpdatedMouseWheelDelta / abs(myUpdatedMouseWheelDelta);
    myUpdatedMouseWheelDelta = 0;

    myLastInputState = myCurrentInputState;
    myCurrentInputState = myUpdatedInputState;
}

bool CommonUtilities::InputHandler::UpdateEvents(UINT aMessage, WPARAM aWParam, LPARAM aLParam)
{
    //MSLLHOOKSTRUCT* mouselparam = (MSLLHOOKSTRUCT*)aLParam;
    switch (aMessage)
    {
    case WM_CLOSE:
    {
        DestroyWindow(GetHWND());
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        break;
        //return true;
    }
    case WM_MOUSEWHEEL:
    {
        myUpdatedMouseWheelDelta = GET_WHEEL_DELTA_WPARAM(aWParam); // or HIWORD(mouselparam->mouseData);?
        return true;
    }
    case WM_LBUTTONDOWN:
    {
        myUpdatedInputState[VK_LBUTTON] = true;
        return true;
    }
    case WM_LBUTTONUP:
    {
        myUpdatedInputState[VK_LBUTTON] = false;
        return true;
    }
    case WM_RBUTTONDOWN:
    {
        myUpdatedInputState[VK_RBUTTON] = true;
        return true;
    }
    case WM_RBUTTONUP:
    {
        myUpdatedInputState[VK_RBUTTON] = false;
        return true;
    }
    case WM_MBUTTONDOWN:
    {
        myUpdatedInputState[VK_MBUTTON] = true;
        return true;
    }
    case WM_MBUTTONUP:
    {
        myUpdatedInputState[VK_MBUTTON] = false;
        return true;
    }
    case WM_XBUTTONDOWN:
    {
        break;
    }
    case WM_XBUTTONUP:
    {
        break;
    }
    case WM_SYSKEYUP:
    {
        myUpdatedInputState[aWParam] = false;
        return true;
    }
    case WM_SYSKEYDOWN:
    {
        myUpdatedInputState[aWParam] = true;
        return true;
    }
    case WM_KEYUP:
    {
        myUpdatedInputState[aWParam] = false;
        return true;
    }
    case WM_KEYDOWN:
    {
        myUpdatedInputState[aWParam] = true;
        return true;
    }
    case WM_INPUT:
    {
        //UINT dwSize;

        //GetRawInputData((HRAWINPUT)aLParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
        //LPBYTE lpb = new BYTE[dwSize];
        //if (lpb == NULL)
        //{
        //    return 0;
        //}

        //if (GetRawInputData((HRAWINPUT)aLParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
        //    OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

        //RAWINPUT* raw = (RAWINPUT*)lpb;

        //
        //if (raw->header.dwType == RIM_TYPEMOUSE)
        //{
        //    myUpdatedMousePos.x = raw->data.mouse.lLastX;
        //    myUpdatedMousePos.y = raw->data.mouse.lLastY;

        //    /*if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
        //        myUpdatedMouseWheelDelta = (*(short*)&raw->data.mouse.usButtonData) / WHEEL_DELTA;*/
        //}

        //delete[] lpb;
        //return true;
    }
    default:
    {
        return false;
    }
    }
    return false;
}

//constexpr std::optional<int> FindInBitSet(std::bitset<256> aKeyCode)
//{
//    for (int i = aKeyCode.size() - 1; i > -1; --i)
//    {
//        if (aKeyCode.test(i))
//        {
//            return aKeyCode[i];
//        }
//    }
//    return std::nullopt;
//}

    // Returns true if a button was NOT held last time but is now held
bool CommonUtilities::InputHandler::GetKeyDown(const int aKeyCode) const
{
    return myCurrentInputState[aKeyCode] && !myLastInputState[aKeyCode];
}

// Returns true if a button was held last time and is also held now (without release in between)
bool CommonUtilities::InputHandler::GetKeyHeld(const int aKeyCode) const
{
    return myCurrentInputState[aKeyCode] && myLastInputState[aKeyCode];
}

// Returns true if a button was held last time but is now released
bool CommonUtilities::InputHandler::GetKeyUp(const int aKeyCode) const
{
    return !myCurrentInputState[aKeyCode] && myLastInputState[aKeyCode];
}

// Check in which state the mouse keys are
int CommonUtilities::InputHandler::GetButtonStateMouse(const int aKeyCode) const
{
    switch (aKeyCode)
    {
        // Left click
    case 0x01:
    {
        return aKeyCode;
    }
    // Right click
    case 0x02:
    {
        return aKeyCode;
    }
    // Middle mouse
    case 0x04:
    {
        return aKeyCode;
    }
    }
    return 0;
}

// Absolute mouse pos, on screen
POINT CommonUtilities::InputHandler::GetAbsoluteMousePosition() const
{
    POINT point;
    if (GetCursorPos(&point))
    {
        return point;
    }
    else
    {
        point.x = 0;
        point.y = 0;
        return point;
    }
}

// Mouse pos relative to client area / window
POINT CommonUtilities::InputHandler::GetMousePosition(HWND aWindowHandle) const
{
    POINT point = GetAbsoluteMousePosition();
    if (ScreenToClient(aWindowHandle, &point))
    {
        return point;
    }
    else
    {
        point.x = 0;
        point.y = 0;
        return point;
    }
}

POINT CommonUtilities::InputHandler::GetDeltaMousePosition() const
{
    return myCurrentMouseDelta;
}

float CommonUtilities::InputHandler::GetDeltaMouseWheel() const
{
    return myCurrentMouseWheelDelta;
}

void CommonUtilities::InputHandler::SetHWND(HWND aWindowHandle)
{
    myWindowHandle = aWindowHandle;
}

HWND CommonUtilities::InputHandler::GetHWND() const
{
    return myWindowHandle;
}

void CommonUtilities::InputHandler::SetMousePos(int x, int y)
{
    SetCursorPos(x, y);
}

void CommonUtilities::InputHandler::CaptureMouse() const
{
    RECT clipRect;

    GetClientRect(GetHWND(), &clipRect);

    POINT upLeft;
    upLeft.x = clipRect.left;
    upLeft.y = clipRect.top;

    POINT lowRight;
    lowRight.x = clipRect.right;
    lowRight.y = clipRect.bottom;

    MapWindowPoints(GetHWND(), nullptr, &upLeft, 1);
    MapWindowPoints(GetHWND(), nullptr, &lowRight, 1);

    clipRect.left = upLeft.x;
    clipRect.top = upLeft.y;
    clipRect.right = lowRight.x;
    clipRect.bottom = lowRight.y;

    ClipCursor(&clipRect);
}

void CommonUtilities::InputHandler::ReleaseMouse() const
{
    ClipCursor(nullptr);
}

//RECT InputHandler::GetLocalCoordinates() const
//{
//    RECT Rect;
//    HWND hwndFound;
//    char windowTitle[1024];
//
//    // Fetch current window title.
//    GetConsoleTitle(windowTitle, 1024);
//    hwndFound = FindWindow(NULL, windowTitle);
//    GetWindowRect(hwndFound, &Rect);
//    MapWindowPoints(HWND_DESKTOP, GetParent(hwndFound), (LPPOINT)&Rect, 2);
//    return Rect;
//}