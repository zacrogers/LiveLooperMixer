#pragma once
#include <JuceHeader.h>
#include <string.h>
//#include <windows.h>

#include "myEnums.h"
//#include "AudioRecorder.h"

#define NUM_CLIPS 2
#define MAX_PATH 50

class ChannelStrip : public Component
{
public:
	//==============================================================================
	ChannelStrip();
	~ChannelStrip();

	/* Component methods */
	void paint(Graphics&) override;
	void resized() override;

	/* Channelstrip methods */
	void changeState(TransportState newState);
	void addToMixer(MixerAudioSource* mixer);

	AudioTransportSource getTransport();

	void setChannelNum(int num) { channelNum = num; };

	/* Gui elements */
	TextButton stopBtn;
	TextButton muteBtn;
	TextButton recBtn;
	TextButton soloBtn;

	TextButton clipBtns[NUM_CLIPS];

	Slider     volumeSlider;
	Slider     filterKnob;

private:
	TransportState state;

	AudioFormatManager formatManager;
	std::unique_ptr<AudioFormatReaderSource> readerSource[NUM_CLIPS];
	AudioTransportSource transportSource[NUM_CLIPS];

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
