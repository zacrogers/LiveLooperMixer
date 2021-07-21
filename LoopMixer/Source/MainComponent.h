#pragma once

#include <JuceHeader.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <cstdint>

#include "ChannelStrip.h"
#include "myEnums.h"

#define NUM_CHANNELS 6

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent,
	                    public ChangeListener,
						public Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

	void changeListenerCallback(ChangeBroadcaster* source) override;

	void timerCallback() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
	std::string basePath;
	//std::vector<ChannelStrip> strips;
	ChannelStrip strips[NUM_CHANNELS];

	TextButton loadButton;
	TextButton loadButton2;
	TextButton playButton;
	TextButton stopButton;

	TextButton play1;
	TextButton play2;

	TextButton stop1;
	TextButton stop2;

	Slider volumeSliders[NUM_CHANNELS];

	MixerAudioSource mixer;
	TransportState state;
	AudioFormatManager formatManager;
	std::unique_ptr<AudioFormatReaderSource> readerSource[NUM_CHANNELS];
	AudioTransportSource transportSource[NUM_CHANNELS];

	bool trackEnabled[NUM_CHANNELS] = {false, false, false, false};

	void initGuiElements();

	void changeState(TransportState newState);
	void loadBtnClicked(int chan);
	void playBtnClicked();
	void stopBtnClicked();

	void volumeChanged(int channel);

	/* Temporary methods */
	void autoLoadClip(std::string clipName, std::uint8_t channel);
	void playClip(std::uint8_t clipNum);
	void stopClip(std::uint8_t clipNum);
	void recordClip();

	void loadClips();

	std::string getExePath();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
