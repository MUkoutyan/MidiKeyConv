/*
  ==============================================================================

    KeySequence.h
    Created: 15 Jun 2021 10:59:18pm
    Author:  Koyo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <vector>

enum SequenceType
{
    List = 0,   //0->127が入力されると一つ進む。
    Range = 1,   //入力されたキー数を0-127の範囲に割り当てる。
};

struct CCKey
{
    struct Sequence
    {
        SequenceType seqType = List;
        int currentIndex = -1;

        struct ListMode
        {
            bool keep = false;
        };
        ListMode list;

        struct RangeMode
        {
            int beforeValue = -1;
        };
        RangeMode range;
    };
    Sequence seq;
    std::vector<juce::KeyPress> keys;

    void toNext()
    {
        ++(seq.currentIndex);
        (seq.currentIndex) %= keys.size();
    }

    void toPrev()
    {
        --(seq.currentIndex);
        (seq.currentIndex) = (seq.currentIndex) < 0 ? (keys.size() - 1) : (seq.currentIndex);
    }
};
