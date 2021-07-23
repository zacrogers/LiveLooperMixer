#pragma once

#include <JuceHeader.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <cstdint>

#include "ChannelStrip.h"
#include "myEnums.h"

constexpr int NUM_CHANNELS = 6;
constexpr int CLIPS_PER_CHAN = 2;
constexpr int MIN_BPM = 60;
constexpr int MAX_BPM = 250;

static std::uint8_t bpmToMs(std::uint8_t bpm, Subdivision subdivision) { return 60000 / bpm * subdivision; }



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
	std::uint8_t bpm = 120;
	int beatDuration = 0;
	Subdivision subdivision = Subdivision::QUARTER;

	std::uint8_t populatedTracks = 0;
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
	Slider tempoSlider;

	MixerAudioSource mixer;
	State::Transport state;
	AudioFormatManager formatManager;
	std::unique_ptr<AudioFormatReaderSource> readerSource[NUM_CHANNELS];
	AudioTransportSource transportSource[NUM_CHANNELS];

	bool trackEnabled[NUM_CHANNELS] = {false, false, false, false};
	std::vector<State::Track> trackStates;

	void initDataVectors(void);
	void initGuiElements(void);

	void changeState(State::Transport newState);
	void loadBtnClicked(int chan);
	void playBtnClicked();
	void stopBtnClicked();

	void volumeChanged(int channel);
	void tempoChanged(void);

	/* Temporary methods */
	void autoLoadClip(std::string clipName, std::uint8_t channel);
	void playClip(std::uint8_t clipNum);
	void stopClip(std::uint8_t clipNum);
	void recordClip();

	void loadClips();

	const std::string getExePath();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
