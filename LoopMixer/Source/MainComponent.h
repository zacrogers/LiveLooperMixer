#pragma once

#include <JuceHeader.h>
#include <stdio.h>
#include <string.h>

#include "ChannelStrip.h"
#include "AudioThru.h"
#include "Recorder.h"
#include "VisualMetronome.h"
#include "myEnums.h"

#define NUM_CHANNELS 6

constexpr int numClips    = 4;
constexpr int numChannels = 2;
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
    /** New stuff */
    const int mMinRecordBars { 1 };
    const int mMaxRecordBars { 8 };
    
    int mBarsToRecord        { 4 };
    bool mRecordArmed        { false };
    bool mInputMuted         { false };
    
    int mLastPlayedIndex     { 0 };
    
    juce::Colour     mDisabledColour  { juce::Colours::darkslateblue };
    
    // Buttons
    juce::TextButton mPlayButton      { "P" };
    juce::TextButton mStopButton      { "S" };
    juce::TextButton mArmRecordButton { "R" };
    juce::TextButton mInputMuteButton { "M" };
    
//    juce::TextButton mClipButtons[numClips];
    
    // Sliders
    juce::Slider     mInputTrimSlider  { juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextBoxAbove };
    juce::Slider     mMasterVolSlider  { juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextBoxAbove };
//    juce::Slider     mBpmSlider       { juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextBoxAbove };
    juce::Slider     mNumRecBarsSlider { juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextBoxAbove };
    
    // Labels
    juce::Label     mInputTrimLabel   { "mInputTrimLabel", "Trim" };
    juce::Label     mMasterVolLabel   { "mMasterVolLabel", "Master" };
    juce::Label     mBpmLabel         { "mBpmLabel", "BPM" };
    juce::Label     mNumRecBarsLabel  { "mNumRecBarsLabel", "Bars" };

    VisualMetronome mMetronome        { true };
    
    
    juce::File mLastRecording;
    juce::AudioDeviceManager mAudioDeviceManager;

    z_lib::AudioThru mAudioThru { mAudioDeviceManager };
    z_lib::Recorder mRecorder;
    z_lib::ChannelStrip mChannelStrip { mAudioDeviceManager, mRecorder };
    
    z_lib::ChannelStrip mChannelStrips[numChannels];
    
    void mChangeState();
    
    void mStartPlaying();
    void mStopPlaying();
    void mStartRecording();
    void mStopRecording();
    
    bool mClipEmpty[numClips]     = { true, true, true, true };
    bool mClipArmed[numClips]     = { false, false, false, false };
    bool mClipRecording[numClips] = { false, false, false, false };
    bool mClipStopping[numClips]  = { false, false, false, false };
    bool mClipPlaying[numClips]   = { false, false, false, false };
    
    void mHandleClipButton(int channelNum, int clipNum);
    void mHandleRecordArmButton();
    void mRecordClip(int channelNum, int clipNum);
    void mPlayClip(int channelNum, int clipNum);
    void mStopClip(int channelNum, int clipNum);
    void mStopRecordClip(int channelNum, int clipNum);
    void mLoadClip(int channelNum, int clipNum);
    
    void mCheckOneChannelArmed();
    
    
    
    /** old stuff */
    juce::String basePath;
    //std::vector<ChannelStrip> strips;
//    ChannelStrip strips[NUM_CHANNELS];

    /** Old gui components */
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
    
    void mLoadRecordedClip();
    
    bool mPeriodEnded(); // Checks when bar or period is ending 

	/* Temporary methods */
	void autoLoadClip(std::string clipName, int channel);
	void playClip(int clipNum);
	void recordClip();

	void loadClips();

    juce::String getExePath();
    
    juce::String mClipsPath;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
