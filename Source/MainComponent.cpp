#include "MainComponent.h"

using namespace juce;

namespace PropertiesName
{
    const static char* MIDIIN_DEVICE_NAME = "MidiInDeviceName";
    const static char* ASSIGNEDNOTEKEY    = "AssignedKey";
    const static char* ASSIGNEDCCKEY      = "AssignedCCKey";    //連番をつける
}

//==============================================================================
MainComponent::MainComponent()
    : audioDeviceManager()
    , midiInSelector("MidiInSelector")
    , lastSelectedInputIndex(0)
    , receiveDeviceErrorMessage(false)
    , keySender()
    , ccAssign(&keySender, this)
    , assignModeCheckBox("assignModeCheck")
    , keyboardState()
    , midiKeyboard(&keySender, keyboardState, MidiKeyboardComponent::horizontalKeyboard)
    , isAddingFromMidiInput(false)
    , openAdvanceOption("openAdvanceOption")
    , isSuspendSaveProperty(false)
    , properties()
    , propertiesFile(nullptr)
    , currentFocusComponent(nullptr)
{
    setPropertiesOption();

    initializeUI();

    loadProperties();

    setSize (750, 280);
}

MainComponent::~MainComponent()
{
    saveProperties();
    audioDeviceManager.removeMidiInputDeviceCallback(juce::MidiInput::getAvailableDevices()[midiInSelector.getSelectedItemIndex()].identifier, this);
}

void MainComponent::initializeUI()
{
    LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Meiryo UI");

    this->addAndMakeVisible(this->midiKeyboard);
    this->addAndMakeVisible(this->midiInSelector);
    this->addAndMakeVisible(this->ccAssign);
    this->addAndMakeVisible(this->assignModeCheckBox);
    this->addAndMakeVisible(this->openAdvanceOption);

    this->addMouseListener(&this->ccAssign, true);


    //MIDIインプットが選択されていません。
    this->midiInSelector.setTextWhenNoChoicesAvailable(juce::translate("Not Select MIDI Input"));
    auto midiInputs = MidiInput::getAvailableDevices();

    StringArray midiInputNames;
    for(const auto& input : midiInputs){
        midiInputNames.add(input.name);
    }

    midiInSelector.addItemList(midiInputNames, 1);
    midiInSelector.onChange = [this] { setMidiInput(midiInSelector.getSelectedItemIndex()); };

    for(const auto& input : midiInputs)
    {
        if(audioDeviceManager.isMidiInputDeviceEnabled(input.identifier))
        {
            setMidiInput(midiInputs.indexOf(input));
            break;
        }
    }
    // if no enabled devices were found just use the first one in the list
    if(midiInSelector.getSelectedId() == 0){
        setMidiInput(0);
    }

    assignModeCheckBox.setToggleState(false, false);
    assignModeCheckBox.onStateChange = [this]
    {
        const bool state = this->assignModeCheckBox.getToggleState();
        midiKeyboard.SetAssignMode(state);
        ccAssign.SetAssignMode(state);
    };

    openAdvanceOption.onClick = [this]
    {
        openAdvanceOption.getToggleState();
    };
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    auto bounds = this->getBounds();
    auto midiInArea = bounds.removeFromTop(24);
    this->midiInSelector.setBounds(midiInArea);

    bounds.removeFromTop(8);

    this->midiKeyboard.setBounds(bounds.removeFromBottom(130));

    bounds.removeFromBottom(4);
    auto assignModeCheckBoxArea = bounds.removeFromBottom(24);
    this->assignModeCheckBox.setBounds(assignModeCheckBoxArea);

    //bounds.removeFromBottom(12); 
    //bounds.removeFromTop(12);
    bounds.setHeight(juce::jmin(bounds.getHeight(), 64));
    this->ccAssign.setBounds(bounds);
}

void MainComponent::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
    keyboardState.processNextMidiEvent(message);
}

bool MainComponent::keyPressed(const juce::KeyPress& key)
{
    auto focusComponent = this->getCurrentlyFocusedComponent();
    if(focusComponent == &midiKeyboard){
        midiKeyboard.receivKeyPressed(key);
    }
    else if(ccAssign.isParentOf(focusComponent)){
        ccAssign.receivKeyPressed(key);
    }
    return false;
}

inline void MainComponent::setMidiInput(int index)
{
    auto list = juce::MidiInput::getAvailableDevices();

    audioDeviceManager.removeMidiInputDeviceCallback(list[lastSelectedInputIndex].identifier, this);
    audioDeviceManager.removeMidiInputDeviceCallback(list[lastSelectedInputIndex].identifier, &ccAssign);

    const auto& newInput = list[index];

    if(audioDeviceManager.isMidiInputDeviceEnabled(newInput.identifier) == false)
    {
        audioDeviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);
        //setMidiInputDeviceEnabledが成功していたらisMidiInputDeviceEnabledはtrueを返す。
        //(他アプリケーションで指定したMIDIデバイスが開かれていた場合に失敗する。)
        if(audioDeviceManager.isMidiInputDeviceEnabled(newInput.identifier) == false)
        {
            //他のアプリで同じMIDIデバイスを使用している可能性があります。
            NativeMessageBox::showMessageBox(AlertWindow::WarningIcon, juce::translate("Can't Open MIDI In Device : ") + newInput.name,
                                             juce::translate("It is possible that other apps are using the same MIDI device."));
            midiInSelector.setSelectedId(lastSelectedInputIndex, juce::dontSendNotification);
            return;
        }
    }

    audioDeviceManager.addMidiInputDeviceCallback(newInput.identifier, this);
    audioDeviceManager.addMidiInputDeviceCallback(newInput.identifier, &ccAssign);
    midiInSelector.setSelectedId(index + 1, juce::dontSendNotification);

    lastSelectedInputIndex = index;
}

inline void MainComponent::setPropertiesOption()
{
    PropertiesFile::Options options;
    options.applicationName = ProjectInfo::projectName;
    options.filenameSuffix = ".settings";
    options.folderName = ProjectInfo::companyName;
    properties.setStorageParameters(options);

    propertiesFile = properties.getUserSettings();
}

void MainComponent::saveProperties()
{
    if(isSuspendSaveProperty){ return; }
    auto midiInName = midiInSelector.getText();
    propertiesFile->setValue(PropertiesName::MIDIIN_DEVICE_NAME, midiInName);

    const auto& assignedNoteKeys = midiKeyboard.GetAssignedKey();
    StringArray keyCodes;
    for(const auto& key : assignedNoteKeys){
        keyCodes.add(String(key.getKeyCode()));
    }
    propertiesFile->setValue(PropertiesName::ASSIGNEDNOTEKEY, keyCodes.joinIntoString(","));

    const auto& ccKeys = ccAssign.GetCurrentAssignCCKeys();
    for(int index=0; const auto& ccKey : ccKeys)
    {
        StringArray keyCodes;
        keyCodes.add(String(ccKey.seq.seqType));
        for(const auto& key : ccKey.keys){
            keyCodes.add(String(key.getKeyCode()));
        }
        propertiesFile->setValue(PropertiesName::ASSIGNEDCCKEY+String(index++), keyCodes.joinIntoString(","));
    }

    propertiesFile->save();
}

void MainComponent::loadProperties()
{
    auto saveMidiInDeviceName = propertiesFile->getValue(PropertiesName::MIDIIN_DEVICE_NAME, "");
    if(saveMidiInDeviceName != "")
    {
        const auto numItems = midiInSelector.getNumItems();
        for(int i = 0; i < numItems; ++i){
            auto name = midiInSelector.getItemText(i);
            if(name == saveMidiInDeviceName){
                midiInSelector.setSelectedItemIndex(i);
                break;
            }
        }
    }

    {
        auto keyCodesStr = propertiesFile->getValue(PropertiesName::ASSIGNEDNOTEKEY, "");
        StringArray keyCodes;
        keyCodes.addTokens(keyCodesStr, ",", "\"");

        for(int index = 0; const auto & key : keyCodes){
            midiKeyboard.AssignKey(index, KeyPress(key.getIntValue()));
            index++;
        }
    }

    const auto& ccKeys = ccAssign.GetCurrentAssignCCKeys();
    std::array<CCKey, 128> assignKey;
    for(int i=0; i<128; ++i)
    {
        auto keyCodesStr = propertiesFile->getValue(PropertiesName::ASSIGNEDCCKEY+String(i), "");
        if(keyCodesStr.isEmpty()){ continue; }

        StringArray keyCodes;
        keyCodes.addTokens(keyCodesStr, ",", "\"");
        assignKey[i].seq.seqType = static_cast<SequenceType>(keyCodes[0].getIntValue());
        keyCodes.remove(0);
        for(const auto& key : keyCodes){
            assignKey[i].keys.emplace_back(KeyPress(key.getIntValue()));
        }
    }
    ccAssign.SetAssignCCKeys(assignKey);

    resumeSaveProperty();
}

inline void MainComponent::suspendSaveProperty() noexcept { isSuspendSaveProperty = true; }

inline void MainComponent::resumeSaveProperty() noexcept { isSuspendSaveProperty = false; }
