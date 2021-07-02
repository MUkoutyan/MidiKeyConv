/*
  ==============================================================================

    CCAssignComponent.cpp
    Created: 6 Jun 2021 3:05:07am
    Author:  Koyo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CCAssignComponent.h"
#include "KeyMessageSender.h"
#include <chrono>

using namespace juce;

//==============================================================================
CCAssignComponent::CCAssignComponent(KeyMessageSender* keySender, juce::Component* parent)
    : keySender(keySender)
    , learnButton("Learn")
    , ccSelector()
    , transitionMethod({"List", "Range"})
    , slotView()
    , assignKeys({})
    , isLearn(false)
{
    this->addAndMakeVisible(learnButton);
    this->addAndMakeVisible(ccSelector);
    this->addAndMakeVisible(transitionMethod);
    this->addAndMakeVisible(slotView);

    learnButton.onStateChange = [this](){
        this->isLearn = learnButton.getToggleState();
    };

    for(int i = 0; i < 128; ++i){
        ccSelector.addItem("CC : " + String(i), i+1);
    }
    ccSelector.onChange = [this](){
        auto currentIndex = ccSelector.getSelectedItemIndex();
        if(currentIndex == -1){ return; }
        ChangeCC(currentIndex);
    };

    transitionMethod.onStateChange = [this]()
    {
        auto currentIndex = ccSelector.getSelectedItemIndex();
        if(currentIndex == -1){ return; }

        auto seqType = static_cast<SequenceType>(transitionMethod.getCurrentState());
        auto& assign = assignKeys[currentIndex];
        assign.seq.seqType = seqType;

        slotView.SetSequenceType(seqType);

        if(seqType == SequenceType::List){
            assign.seq.list.keep = false;
            assign.seq.currentIndex = -1;
            slotView.setIndex(-1);
        }
        else if(seqType == SequenceType::Range){
            assign.seq.currentIndex = assign.seq.range.beforeValue;
            slotView.setIndex(assign.seq.currentIndex);
        }
    };

    slotView.rightClickKeyIndex = [this](int index)
    {
        auto currentIndex = ccSelector.getSelectedItemIndex();
        if(currentIndex == -1){ return; }

        auto& assign = assignKeys[currentIndex];
        if(index < assign.keys.size()){
            assign.keys.erase(assign.keys.begin() + index);
        }
        slotView.SetKeys(assign.keys);
    };
}

CCAssignComponent::~CCAssignComponent()
{
}

void CCAssignComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
}

void CCAssignComponent::resized()
{
    auto bounds = this->getBounds();
    bounds.setHeight(24);

    bounds.removeFromLeft(12);
    learnButton.setBounds(bounds.removeFromLeft(65));
    bounds.removeFromLeft(12);
    ccSelector.setBounds(bounds.removeFromLeft(120));
    bounds.removeFromLeft(12);
    transitionMethod.setBounds(bounds.removeFromLeft(70));
    bounds.removeFromLeft(24); 
    bounds.removeFromRight(36);
    slotView.setBounds(bounds);
}

int CCAssignComponent::LastSelectedCC() const
{
    return ccSelector.getSelectedItemIndex();
}

void CCAssignComponent::SetAssignMode(bool assignMode)
{
    slotView.SetAssignMode(assignMode);
}

void CCAssignComponent::receivKeyPressed(const juce::KeyPress& k)
{
    if(slotView.IsAssignMode() == false){ return; }

    auto currentIndex = ccSelector.getSelectedItemIndex();
    if(currentIndex == -1){ return; }

    auto& assign = assignKeys[currentIndex];
    if(assign.keys.size() < 128)
    {
        assign.keys.emplace_back(k);
        slotView.SetKeys(assign.keys);
    }
    return;
}

void CCAssignComponent::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    if(message.isController() == false){ return; }

    auto ccIndex = message.getControllerNumber();
    if(isLearn){
        juce::MessageManagerLock locker(juce::Thread::getCurrentThread());
        ccSelector.setSelectedItemIndex(ccIndex, false);
        ChangeCC(ccIndex);
        return;
    }

    auto& assign = assignKeys[ccIndex];
    if(assign.keys.empty()){ return; }
    if(assign.keys[0].getKeyCode() == 0){ return; }

    const auto value = message.getControllerValue();
    const bool isShowingCC = (ccIndex == ccSelector.getSelectedItemIndex());
    if(isShowingCC){
        slotView.SetCurrentCCValue(value);
    }
    
    if(assign.seq.seqType == SequenceType::List)
    {
        if(value == 127)
        {
            assign.seq.currentIndex = assign.seq.currentIndex < 0 ? 0 : assign.seq.currentIndex;
            if(assign.seq.list.keep == false)
            {
                SendKey(assign.keys[assign.seq.currentIndex]);
                assign.seq.list.keep = true;
                assign.toNext();

                if(isShowingCC){
                    slotView.toNext();
                }
            }
        }
        else if(value == 0){
            assign.seq.list.keep = false;
        }
    }
    else if(assign.seq.seqType == SequenceType::Range)
    {
        const auto currentSize = int(assign.keys.size());
        auto currentValue = juce::jmin(juce::jmap(value, 0, 127, 0, currentSize), currentSize-1);
        juce::Logger::outputDebugString(String::formatted("value : %d to : %d", value, currentValue));
        if(currentValue != assign.seq.range.beforeValue)
        {
            assign.seq.currentIndex = currentValue;
            SendKey(assign.keys[assign.seq.currentIndex]);
            assign.seq.range.beforeValue = assign.seq.currentIndex;

            if(isShowingCC){
                slotView.setIndex(assign.seq.currentIndex);
            }
        }
    }
}

void CCAssignComponent::SendKey(const juce::KeyPress& keyPress)
{
    keySender->SendDownKey(keyPress);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    keySender->SendReleaseKey(keyPress);
}

void CCAssignComponent::ChangeCC(int ccNo)
{
    const auto& assign = assignKeys[ccNo];
    transitionMethod.setCurrentState(assign.seq.seqType);
    slotView.SetCCKey(assign);
}
