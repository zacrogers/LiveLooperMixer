#pragma once

#include <JuceHeader.h>
#include <stdio.h>
#include <string.h>

#include "ChannelStrip.h"
#include "myEnums.h"

#define NUM_CHANNELS 6


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public juce::AudioAppComponent,
                        public juce::ChangeListener,
                        public juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

	void timerCallback() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::String basePath;
	//std::vector<ChannelStrip> strips;
//	ChannelStrip strips[NUM_CHANNELS];

    juce::TextButton loadButton;
    juce::TextButton loadButton2;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    juce::TextButton play1;
    juce::TextButton play2;

    juce::Slider volumeSliders[NUM_CHANNELS];

    juce::MixerAudioSource mixer;
    TransportState state;
    juce::AudioFormatManager formatManager;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource[NUM_CHANNELS];
    juce::AudioTransportSource transportSource[NUM_CHANNELS];

	void initGuiElements();

	void changeState(TransportState newState);
	void loadBtnClicked(int chan);
	void playBtnClicked();
	void stopBtnClicked();

	void volumeChanged(int channel);

	/* Temporary methods */
	void autoLoadClip(std::string clipName, int channel);
	void playClip(int clipNum);
	void recordClip();

	void loadClips();

    juce::String getExePath();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
