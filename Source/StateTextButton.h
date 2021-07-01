/*
  ==============================================================================

	StateImageButton.h
	Created: 4 Jul 2020 12:52:10am
	Author:  Koyo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class StateTextButton : public juce::TextButton, public juce::Button::Listener
{
public:
	StateTextButton(juce::StringArray texts, std::uint32_t initState = 0)
		: TextButton()
		, texts(std::move(texts))
		, currentState(initState)
	{
		addListener(this);
		attachCurrentStateText();
	}

	void setCurrentState(std::int32_t state)
	{
		if(this->currentState != state){
			this->currentState = state;
			attachCurrentStateText();
		}
	}

	std::int32_t getCurrentState() const noexcept { return currentState; }

protected:

	void attachCurrentStateText()
	{
		jassert(currentState < texts.size());
		this->setButtonText(this->texts[currentState]);
		if(this->onStateChange){ this->onStateChange(); }
	}

	void buttonClicked(Button* button)
	{
		currentState++;
		currentState %= texts.size();
		attachCurrentStateText();
	}

private:
	juce::StringArray texts;
	std::int32_t currentState;
};
