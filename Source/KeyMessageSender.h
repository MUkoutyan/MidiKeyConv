/*
  ==============================================================================

    HookWrapper.h
    Created: 31 May 2021 11:02:23pm
    Author:  Koyo

  ==============================================================================
*/

#include <JuceHeader.h>

class KeyMessageSender
{
public:
    KeyMessageSender();
    ~KeyMessageSender();

    void SendDownKey(const juce::KeyPress& key);
    void SendReleaseKey(const juce::KeyPress& key);
};

#pragma once
