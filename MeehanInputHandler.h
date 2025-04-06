#pragma once
#include <Windows.h>
#include <Winuser.h>
#include <strsafe.h>
#include <string>
#include <stack>

#include <bitset>

namespace CommonUtilities
{
    class InputHandler
    {
    public:

        InputHandler();
        void Update();
        void CaptureMouse() const;
        void ReleaseMouse() const;
        bool UpdateEvents(UINT aMessage, WPARAM aWParam, LPARAM aLParam);
        bool GetKeyHeld( int aKeyCode) const;
        bool GetKeyDown( int aKeyCode) const;
        bool GetKeyUp( int aKeyCode) const;
        int GetButtonStateMouse( int aKeyCode) const;
        POINT GetAbsoluteMousePosition() const;
        POINT GetMousePosition(HWND aWindowHandle) const;
        POINT GetDeltaMousePosition() const;
        float GetDeltaMouseWheel() const;
        void SetHWND(HWND aWindowHandle);
        HWND GetHWND() const;
        void SetMousePos(int x, int y);
        //RECT GetLocalCoordinates() const;
    private:
        HWND myWindowHandle;
        POINT myLastMousePos{ 0 };
        POINT myCurrentMousePos{ 0 };
        POINT myUpdatedMousePos{ 0 };
        POINT myLastMouseDelta{ 0 };
        POINT myCurrentMouseDelta{ 0 };
        float myUpdatedMouseWheelDelta = 0;
        float myCurrentMouseWheelDelta = 0;
        float myLastMouseWheelDelta = 0;
        // Bitset for the input states and WPARAM to see what input is triggered
        std::bitset<256> myLastInputState{};
        std::bitset<256> myUpdatedInputState{};
        std::bitset<256> myCurrentInputState{};
    };
}
