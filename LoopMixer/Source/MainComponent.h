#pragma once

#include <JuceHeader.h>
#include <stdio.h>
#include <string.h>

#include "CompileFlags.h"

#include "ChannelStrip.h"
#include "AudioThru.h"
#include "Recorder.h"
#include "VisualMetronome.h"
#include "myEnums.h"

#define NUM_CHANNELS 6

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent: public juce::AudioAppComponent,
                     public juce::ChangeListener,
                     public juce::Timer
{
public:
    //==============================================================================
    MainComponent                ();
    ~MainComponent               ();

    // AudioAppComponent overrides
    void prepareToPlay           (int samplesPerBlockExpected, double sampleRate)   override;
    void getNextAudioBlock       (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources        ()                                                 override;
    void paint                   (juce::Graphics& g)                                override;
    void resized                 ()                                                 override;

    // ChangeListener overrides
    void changeListenerCallback  (juce::ChangeBroadcaster* source)                  override;
    
    // Timer override
    void timerCallback           ()                                                 override;


private:
    /** Gui Elements */
    // Colors
    juce::Colour             mDisabledColour   { juce::Colours::darkslateblue };
    
    // Buttons
    juce::TextButton         mPlayButton       { "P" };
    juce::TextButton         mStopButton       { "S" };
    juce::TextButton         mArmRecordButton  { "R" };
    juce::TextButton         mInputMuteButton  { "M" };
    
    // Sliders
    juce::Slider             mInputTrimSlider  { juce::Slider::SliderStyle::LinearBarVertical,
                                                 juce::Slider::TextBoxAbove };
    juce::Slider             mMasterVolSlider  { juce::Slider::SliderStyle::LinearBarVertical,
                                                 juce::Slider::TextBoxAbove };
    juce::Slider             mNumRecBarsSlider { juce::Slider::SliderStyle::LinearBarVertical,
                                                 juce::Slider::TextBoxAbove };
    
    // Labels
    juce::Label              mInputTrimLabel   { "mInputTrimLabel",  "Trim" };
    juce::Label              mMasterVolLabel   { "mMasterVolLabel",  "Master" };
    juce::Label              mBpmLabel         { "mBpmLabel",        "BPM" };
    juce::Label              mNumRecBarsLabel  { "mNumRecBarsLabel", "Bars" };


    // Consts
    static constexpr int     mMinRecordBars    { 1 };
    static constexpr int     mMaxRecordBars    { 8 };
    static constexpr int     numClips          { 4 };
    static constexpr int     numChannels       { 2 };
    
    // Variables
    int                      mBarsToRecord     { 4 };
    bool                     mRecordArmed      { false };
    bool                     mInputMuted       { false };
    int                      mLastPlayedIndex  { 0 };
    TransportState           state;
    
    juce::Uuid               mCurrentProjectId;
    juce::String             mProjectsPath;
    
    juce::File               mLastRecording;
    juce::AudioDeviceManager mAudioDeviceManager;
    juce::AudioFormatManager formatManager;
    juce::MixerAudioSource   mMixer;
    
    
    // Audio Sinks & Sources
    z_lib::AudioThru         mAudioThru { mAudioDeviceManager };
    z_lib::Recorder          mRecorder;
    z_lib::ChannelStrip      mChannelStrips[numChannels];
    VisualMetronome          mMetronome { true };
    
    /** Member Functions */
    void         mInitGuiElements      ();
    void         mChangeState          (TransportState newState);
    bool         mPeriodEnded          ();
    juce::String mChannelPath          (juce::uint8 channel) const;
    
    // Button callbacks
    void         mPlayButtonClicked    ();
    void         mStopButtonClicked    ();
    void         mRecordButtonClicked  ();
    void         mMuteButtonClicked    ();
    
    // Slider Callbacks
    void         mInputTrimChanged     ();
    void         mMasterVolChanged     ();
    void         volumeChanged         (int channel);
    




    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
