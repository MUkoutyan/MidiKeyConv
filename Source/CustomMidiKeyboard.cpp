/*
  ==============================================================================

    CustomMidiKeyboard.cpp
    Created: 1 Jun 2021 1:27:21am
    Author:  Koyo

  ==============================================================================
*/

#include "CustomMidiKeyboard.h"
#include "KeyMessageSender.h"

using namespace juce;



CustomMidiKeyboard::CustomMidiKeyboard(KeyMessageSender* keySender, juce::MidiKeyboardState& state, juce::MidiKeyboardComponent::Orientation orientation)
    : MidiKeyboardComponent(state, orientation)
    , keySender(keySender)
    , selectedKeys()
    , assignKeys()
    , isAssignMode(false)
{
    this->setName("MIDIKeyboard");
    std::fill(selectedKeys.begin(), selectedKeys.end(), false);
    this->setKeyWidth(32.f);

    Desktop::getInstance().addFocusChangeListener(this);
}

void CustomMidiKeyboard::SetAssignMode(bool assign)
{
    this->isAssignMode = assign;
}

void CustomMidiKeyboard::AssignKey(int assignNote, const juce::KeyPress& key)
{
    if(assignNote < 0 || 126 < assignNote){ return; }
    this->assignKeys[assignNote] = key;
}

std::vector<int> CustomMidiKeyboard::SelectedKeys() const
{
    std::vector<int> result;
    int index = 0;
    for(bool stat : selectedKeys){
        if(stat){ result.emplace_back(index); }
        index++;
    }
    return result;
}

const std::array<juce::KeyPress, 128>& CustomMidiKeyboard::GetAssignedKey() const
{
    return assignKeys;
}

void CustomMidiKeyboard::handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    if(isAssignMode == false && this->assignKeys[midiNoteNumber].isValid()){
        keySender->SendDownKey(this->assignKeys[midiNoteNumber]);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void CustomMidiKeyboard::handleNoteOff(juce::MidiKeyboardState * source, int midiChannel, int midiNoteNumber, float velocity)
{
    if(isAssignMode == false && this->assignKeys[midiNoteNumber].isValid()){
        keySender->SendReleaseKey(this->assignKeys[midiNoteNumber]);
    }
}

void CustomMidiKeyboard::drawWhiteNote(int midiNoteNumber, Graphics& g, Rectangle<float> area, bool isDown, bool isOver, Colour lineColour, Colour textColour)
{
    if(isAssignMode){
        MidiKeyboardComponent::drawWhiteNote(midiNoteNumber, g, area, this->selectedKeys[midiNoteNumber], false, lineColour, textColour);
    }
    else{
        MidiKeyboardComponent::drawWhiteNote(midiNoteNumber, g, area, isDown, isOver, lineColour, textColour);
    }
}

void CustomMidiKeyboard::drawBlackNote(int midiNoteNumber, Graphics & g, Rectangle<float> area, bool isDown, bool isOver, Colour noteFillColour)
{
    if(isAssignMode){
        MidiKeyboardComponent::drawBlackNote(midiNoteNumber, g, area, this->selectedKeys[midiNoteNumber], false, noteFillColour);
    }
    else{
        MidiKeyboardComponent::drawBlackNote(midiNoteNumber, g, area, isDown, isOver, noteFillColour);
    }

    auto text = getBlackNoteText(midiNoteNumber);

    if(text.isNotEmpty())
    {
        auto fontHeight = jmin(12.0f, this->getKeyWidth() * 0.9f);

        g.setColour(Colours::white);
        g.setFont(Font(fontHeight).withHorizontalScale(0.8f));

        switch(this->getOrientation())
        {
            case horizontalKeyboard:            g.drawText(text, area.withTrimmedLeft(1.0f).withTrimmedBottom(2.0f), Justification::centredBottom, false); break;
            case verticalKeyboardFacingLeft:    g.drawText(text, area.reduced(2.0f), Justification::centredLeft, false); break;
            case verticalKeyboardFacingRight:   g.drawText(text, area.reduced(2.0f), Justification::centredRight, false); break;
            default: break;
        }
    }
}

bool CustomMidiKeyboard::mouseDownOnKey(int midiNoteNumber, const MouseEvent& e)
{
    if(isAssignMode == false){
        return MidiKeyboardComponent::mouseDownOnKey(midiNoteNumber, e);
    }

    if(midiNoteNumber < 0 || 126 < midiNoteNumber){ return false; }
    if(this->selectedKeys[midiNoteNumber] == false){
        std::fill(this->selectedKeys.begin(), this->selectedKeys.end(), false);
    }

    this->selectedKeys[midiNoteNumber] = !this->selectedKeys[midiNoteNumber];

    if(e.mods.isRightButtonDown()){
        assignKeys[midiNoteNumber] = KeyPress();
    }

    this->repaint();
    return false;
}

void CustomMidiKeyboard::globalFocusChanged(juce::Component* focusedComponent)
{
    if(focusedComponent != this){
        std::fill(this->selectedKeys.begin(), this->selectedKeys.end(), false);
        this->repaint();
    }
}

void CustomMidiKeyboard::receivKeyPressed(const KeyPress& e)
{
    if(isAssignMode == false){
        return;
    }

    auto currentSelectedKey = std::find(selectedKeys.begin(), selectedKeys.end(), true);
    if(currentSelectedKey != selectedKeys.end())
    {
        auto index = std::distance(selectedKeys.begin(), currentSelectedKey);
        assignKeys[index] = e;
        this->repaint();
    }
    return;
}

String CustomMidiKeyboard::getWhiteNoteText(int midiNoteNumber)
{
    if(assignKeys[midiNoteNumber].isValid()){
        return String(assignKeys[midiNoteNumber].getTextDescription());
    }
    return "";
}

String CustomMidiKeyboard::getBlackNoteText(int midiNoteNumber)
{
    if(assignKeys[midiNoteNumber].isValid()){
        return String(assignKeys[midiNoteNumber].getTextDescription());
    }
    return "";
}
