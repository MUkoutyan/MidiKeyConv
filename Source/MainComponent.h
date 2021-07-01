#pragma once

#include <JuceHeader.h>
#include "KeyMessageSender.h"
#include "CustomMidiKeyboard.h"
#include "CCAssignComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component
                     , public juce::MidiInputCallback
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

protected:
    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message);

private:

    void initializeUI();
    void setMidiInput(int index);

    void setPropertiesOption();

    void saveProperties();

	void loadProperties();
	void suspendSaveProperty() noexcept;
	void resumeSaveProperty() noexcept;
    //==============================================================================
    // Your private member variables go here...
    juce::AudioDeviceManager audioDeviceManager;
    juce::ComboBox midiInSelector;
    int lastSelectedInputIndex;
    bool receiveDeviceErrorMessage;

    KeyMessageSender    keySender;

    CCAssignComponent   ccAssign;

    juce::ToggleButton assignModeCheckBox;

    juce::MidiKeyboardState keyboardState;
    CustomMidiKeyboard midiKeyboard;
    bool isAddingFromMidiInput;

    juce::ToggleButton openAdvanceOption;

	bool isSuspendSaveProperty;
	juce::ApplicationProperties properties;
	juce::PropertiesFile* propertiesFile;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
