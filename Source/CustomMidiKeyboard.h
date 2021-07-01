/*
  ==============================================================================

    CustomMidiKeyboard.h
    Created: 1 Jun 2021 1:27:21am
    Author:  Koyo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class KeyMessageSender;
class CustomMidiKeyboard : public juce::MidiKeyboardComponent
                         , public juce::MidiKeyboardStateListener
{
public:
    explicit CustomMidiKeyboard(KeyMessageSender* keySender, juce::MidiKeyboardState& state, juce::MidiKeyboardComponent::Orientation orientation);

    void SetAssignMode(bool assign);
    void AssignKey(int assignNote, const juce::KeyPress& key);
    std::vector<int> SelectedKeys() const;
    const std::array<juce::KeyPress, 128>& GetAssignedKey() const;

protected:

    void handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity);

    void handleNoteOff(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity);


    void drawWhiteNote(int midiNoteNumber,
                       juce::Graphics& g, juce::Rectangle<float> area,
                       bool isDown, bool isOver,
                       juce::Colour lineColour, juce::Colour textColour) override;

    void drawBlackNote(int midiNoteNumber,
                       juce::Graphics& g, juce::Rectangle<float> area,
                       bool isDown, bool isOver,
                       juce::Colour noteFillColour) override;

    bool mouseDownOnKey(int midiNoteNumber, const juce::MouseEvent& e) override;

    bool keyPressed(const juce::KeyPress&) override; 

    juce::String getWhiteNoteText(int midiNoteNumber) override;
    juce::String getBlackNoteText(int midiNoteNumber);

private:
    KeyMessageSender* keySender;
    std::array<bool, 128> selectedKeys;
    std::array<juce::KeyPress, 128> assignKeys;
    bool isAssignMode;
};
