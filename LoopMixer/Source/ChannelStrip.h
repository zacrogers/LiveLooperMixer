#pragma once
#include <JuceHeader.h>
#include <string.h>
//#include <windows.h>

#include "myEnums.h"
//#include "AudioRecorder.h"

#define NUM_CLIPS 2
#define MAX_PATH 50

class ChannelStrip : public juce::Component
{
public:
	//==============================================================================
	ChannelStrip();
	~ChannelStrip();

	/* Component methods */
	void paint(juce::Graphics&) override;
	void resized() override;

	/* Channelstrip methods */
	void changeState(TransportState newState);
	void addToMixer(juce::MixerAudioSource* mixer);

    juce::AudioTransportSource getTransport();

	void setChannelNum(int num) { channelNum = num; };

	/* Gui elements */
    juce::TextButton stopBtn;
    juce::TextButton muteBtn;
    juce::TextButton recBtn;
    juce::TextButton soloBtn;

    juce::TextButton clipBtns[NUM_CLIPS];

    juce::Slider     volumeSlider;
    juce::Slider     filterKnob;

private:
	TransportState state;

    juce::AudioFormatManager formatManager;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource[NUM_CLIPS];
    juce::AudioTransportSource transportSource[NUM_CLIPS];

	int channelNum = 0;
	std::string basePath = "";

	int currentClip = 0;
	float gain = 0.0;

	bool isPlaying = false;
	bool isMuted   = false;
	bool isSoloed  = false;
	bool isArmed   = false;

	void initGuiElements();

	void loadClip(int slot, std::string clipName);

	void clipBtnClicked(int clip);
	void stopBtnClicked();
	void muteBtnClicked();
	void soloBtnClicked();
	void recBtnClicked();

	void recordClip();

	std::string getBasePath();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelStrip)
};
