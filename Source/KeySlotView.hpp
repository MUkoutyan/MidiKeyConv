/*
  ==============================================================================

    KeySlotView.h
    Created: 6 Jun 2021 4:42:00pm
    Author:  Koyo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "KeySequence.h"

//==============================================================================
/*
*/
class KeySlotView  : public juce::AnimatedAppComponent
{
public:
    KeySlotView()
        : labelFont("Meiryo UI", 14, 0)
        , currentIndex(-1)
        , currentCCValue(0)
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        setFramesPerSecond(60);
    }

    ~KeySlotView() override
    {
    }

    void SetCCKey(const CCKey& ccKey)
    {
        this->SetSequenceType(ccKey.seq.seqType);
        this->setIndex(ccKey.seq.currentIndex);
        this->SetKeys(ccKey.keys);
        this->SetCurrentCCValue(ccKey.seq.range.beforeValue);
    }

    void SetSequenceType(SequenceType type)
    {
        this->seqType = type;
    }

    void SetKeys(std::vector<juce::KeyPress> _keys)
    {
        this->keys = std::move(_keys);

        UpdateKeysArea();

        if(keys.size() <= currentIndex){
            currentIndex = juce::jmax(0, int(keys.size()) - 1);
        }
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        auto bounds = this->getLocalBounds();
        g.setColour(isAssignMode ? juce::Colour(0x66660000) : juce::Colour(0x66000000));
        g.fillRect(bounds);
        g.setColour(juce::Colours::grey);
        g.drawRect(bounds, 1.f);

        if(seqType == SequenceType::Range){
            float x = juce::jmap(this->currentCCValue, 0, 127, 0, bounds.getWidth());
            g.setColour(juce::Colours::cyan);
            g.drawLine(x, 0.f, x, bounds.getHeight(), 2.f);
        }

        for(int index = 0; auto & key : keys)
        {
            auto b = (seqType == SequenceType::Range) ? rangeTypeKeysArea[index] 
                                                      : listTypeKeysArea[index];

            g.setColour(juce::Colours::white);
            g.drawRect(b);

            auto isContain = b.contains(currentMousePos);
            if(index == currentIndex && isAssignMode == false)
            {
                auto color = juce::Colours::darksalmon;
                if(isContain){ color = color.brighter(); }

                const auto currentFlame = getFrameCounter() % 80;
                g.setColour(color.withAlpha(juce::jmap(float(currentFlame), 0.f, 80.f, 0.5f, 1.f)));
                g.fillRect(b);
            }
            else if(isContain)
            {
                g.setColour(juce::Colours::darkorange);
                g.fillRect(b);
            }

            g.setColour(juce::Colours::white);
            auto text = key.getTextDescription();
            g.drawText(text, b, juce::Justification(juce::Justification::centred));
            index++;
        }
    }

    void update() override {
    }

    void toNext()
    {
        if(keys.empty()){ return; }
        ++currentIndex;
        currentIndex %= keys.size();
    }

    void toPrev()
    {
        if(keys.empty()){ return; }
        --currentIndex;
        currentIndex = currentIndex < 0 ? (int(keys.size()) - 1) : currentIndex;
    }

    void setIndex(int index)
    {
        if(keys.empty()){ return; }
        currentIndex = juce::jlimit(-1, int(keys.size()) - 1, index);
    }

    //void mouseDoubleClick(const juce::MouseEvent& event) override
    //{
    //    if(this->getLocalBounds().contains(event.getMouseDownPosition())){
    //        isAssignMode = !isAssignMode;
    //    }
    //}

    void mouseUp(const juce::MouseEvent& e) override
    {
        if(e.mods.isRightButtonDown() && isAssignMode)
        {
            for(int index = 0; const auto & area : listTypeKeysArea)
            {
                if(area.contains(e.getMouseDownPosition()))
                {
                    if(rightClickKeyIndex)
                    {
                        rightClickKeyIndex(index);
                        return;
                    }
                }
                index++;
            }
        }
        else if(e.mods.isLeftButtonDown())
        {
            for(int index = 0; const auto & area : listTypeKeysArea)
            {
                if(area.contains(e.getMouseDownPosition()))
                {
                    currentIndex = index;
                    return;
                }
                index++;
            }
        }
    }

    void mouseMove(const juce::MouseEvent& e) override
    {
        currentMousePos = e.getPosition();
    }

    void resized() override
    {
        this->UpdateKeysArea();
    }

    void SetCurrentCCValue(int value) { currentCCValue = value; }

    void SetAssignMode(bool assign) { isAssignMode = assign; }
    bool IsAssignMode() const noexcept { return isAssignMode; }

    std::function<void(int)> rightClickKeyIndex;

private:

    void UpdateKeysArea()
    {
        listTypeKeysArea.clear();
        rangeTypeKeysArea.clear();
        if(keys.empty()){ return; }

        auto listBounds  = this->getLocalBounds();
        auto rangeBounds = this->getLocalBounds();
        const auto rangeWidth = rangeBounds.getWidth() / keys.size();
        for(int index = 0; auto & key : keys)
        {
            //List
            {
                auto width = labelFont.getStringWidth(key.getTextDescription()) * 2;
                auto b = listBounds.removeFromLeft(std::max(32, width));
                b.reduce(2, 2);
                listTypeKeysArea.emplace_back(std::move(b));
            }
            //Range
            {
                auto b = rangeBounds.removeFromLeft(rangeWidth);
                b.reduce(2, 2);
                rangeTypeKeysArea.emplace_back(std::move(b));
            }
        }
    }

    juce::Font labelFont;
    std::vector<juce::KeyPress> keys;
    std::vector<juce::Rectangle<int>> listTypeKeysArea;
    std::vector<juce::Rectangle<int>> rangeTypeKeysArea;
    juce::Point<int> currentMousePos;
    int currentIndex;
    int currentCCValue;
    SequenceType seqType;
    bool isAssignMode = false;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeySlotView)
};
