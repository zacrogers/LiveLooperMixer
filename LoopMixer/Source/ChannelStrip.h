#pragma once
#include <JuceHeader.h>
#include <string.h>
#include <windows.h>

#include "myEnums.h"
//#include "AudioRecorder.h"

const int NUM_CLIPS = 2;

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
	void changeState(State::Transport newState);
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
	State::Transport state;

	std::uint8_t nClipsPopulated = 0;

	AudioFormatManager formatManager;
	std::unique_ptr<AudioFormatReaderSource> readerSource[NUM_CLIPS];
	AudioTransportSource transportSource[NUM_CLIPS];

	int channelNum = 0;
	std::string basePath;

	int currentClip = 0;
	float gain = 0.0;

	bool isPlaying = false;
	bool isMuted   = false;
	bool isSoloed  = false;
	bool isArmed   = false;
	
	State::Track trackState = State::Track::STOPPED;

	void initGuiElements();

	void loadClip(int slot, std::string clipName);

	void clipBtnClicked(int clip);
	void stopBtnClicked();
	void muteBtnClicked();
	void soloBtnClicked();
	void recBtnClicked();

	void recordClip();

	bool clipsEmpty(void);

	std::string getBasePath();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelStrip)
};