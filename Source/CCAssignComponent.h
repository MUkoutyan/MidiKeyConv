/*
  ==============================================================================

    CCAssignComponent.h
    Created: 6 Jun 2021 3:05:07am
    Author:  Koyo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StateTextButton.h"
#include "KeySequence.h"
#include "KeySlotView.hpp"

//==============================================================================
/*
*/
class KeyMessageSender;
class CCAssignComponent  : public juce::Component
                         , public juce::MidiInputCallback
                         , public juce::KeyListener
{
public:

    CCAssignComponent(KeyMessageSender* keySender, juce::Component* parent);
    ~CCAssignComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    int LastSelectedCC() const;
    void SetAssignMode(bool assignMode);

    void SetAssignCCKeys(const std::array<CCKey, 128>& keys) { assignKeys = keys; }
    const std::array<CCKey, 128>& GetCurrentAssignCCKeys() const noexcept { return assignKeys; }

private:

    bool keyPressed(const juce::KeyPress& k, juce::Component* originatingComponent) override;

    KeyMessageSender* keySender;
    juce::ComboBox   ccSelector;
    StateTextButton  transitionMethod;
    KeySlotView slotView;

    std::array<CCKey, 128> assignKeys;

    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
    void SendKey(const juce::KeyPress& keyPress);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CCAssignComponent)
};
