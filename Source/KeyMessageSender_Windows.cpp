/*
  ==============================================================================

    HookWrapper.cpp
    Created: 31 May 2021 11:02:23pm
    Author:  Koyo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "KeyMessageSender.h"
#include <string>
#include <functional>

#include <Windows.h>

BOOL CALLBACK speichereFenster(HWND hwnd, LPARAM lParam)
{
    if(!IsWindowVisible(hwnd)){ return TRUE; }

    if(GetWindowTextLengthW(hwnd) == 0) { return TRUE; }

    const DWORD TITLE_SIZE = 1024;
    WCHAR windowTitle[TITLE_SIZE];
    GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

    std::wstring title(&windowTitle[0]);
    if(title == L"Program Manager"){ return TRUE; }

    std::map<std::uintptr_t, juce::String>& titles = *reinterpret_cast<std::map<std::uintptr_t, juce::String>*>(lParam);
    titles[(std::uintptr_t)hwnd] = juce::String(title.c_str());

    return TRUE;
}

KeyMessageSender::KeyMessageSender()
{
}

KeyMessageSender::~KeyMessageSender()
{}

void KeyMessageSender::SendDownKey(const juce::KeyPress& key)
{
    INPUT inp = {};
    inp.type            = INPUT_KEYBOARD;
    inp.ki.wVk          = VkKeyScan(key.getKeyCode());
    inp.ki.wScan        = MapVirtualKey(inp.ki.wVk, 0);
    inp.ki.dwFlags      = 0;    //0でキー押下
    inp.ki.dwExtraInfo  = 0;
    inp.ki.time = 0;

    SendInput(1, &inp, sizeof(INPUT));
}

void KeyMessageSender::SendReleaseKey(const juce::KeyPress& key)
{
    INPUT inp = {};
    inp.type            = INPUT_KEYBOARD;
    inp.ki.wVk          = VkKeyScan(key.getKeyCode());
    inp.ki.wScan        = MapVirtualKey(inp.ki.wVk, 0);
    inp.ki.dwFlags      = KEYEVENTF_KEYUP;
    inp.ki.dwExtraInfo  = 0;
    inp.ki.time = 0;

    SendInput(1, &inp, sizeof(INPUT));
}
