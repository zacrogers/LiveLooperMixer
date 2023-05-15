#pragma once
#include <JuceHeader.h>
#include <string.h>
#include <cstdint>

#include "Common.h"
#include "myEnums.h"
#include "ClipButton.h"
#include "Recorder.h"
#include "AudioClip.h"


#define NUM_CLIPS 2
#define MAX_PATH 50

namespace z_lib
{
constexpr int numClips = 4;



class ChannelStrip : public juce::Component,
                     public juce::ChangeBroadcaster,
                     public juce::ChangeListener

{
public:
    enum class Exception
    {
        RecorderNotInitialised = 0,
        DeviceManagerNotInitialised
    };
    
    struct Snapshot
    {
        juce::uint8 currentClip;
        juce::uint8 nextClip;
        State       state;
    };
    
    /** Component overrides */
    void paint                    (juce::Graphics& g) override;
    void resized                  () override;
    
    /** Change Listener override */
    void changeListenerCallback   (juce::ChangeBroadcaster* source) override;
    
    /** Channel Strip */
    ChannelStrip();
    ~ChannelStrip();
    
    void init                     (const juce::AudioDeviceManager *deviceManager, Recorder *recorder, State *state, juce::String clipsPath);
    void updateState              (); /** Should be called whenever the parents state changes */
    void updateClipsPath          (juce::String path) { mClipsPath   = path; }; // needs to be called if project is reloaded because the project id will change
    void setArmed                 (bool armed)        { mRecordArmed = !armed; recordArmButtonClicked();}
    
    /** Getters */
    struct Snapshot  snapshot     () const { return mSnapshot; }
    const  AudioClip *pAudioClip  ()       { return &mAudioClip; }
    bool             isArmed      ()       { return mRecordArmed; }
    
private:
    // Member variables
    juce::String                   mClipsPath;
    State                          *pState;
    State                          mPrevState;
    struct Snapshot                mSnapshot                { 0 };

    bool                           mMuted                   { false };
    bool                           mSoloed                  { false };
    bool                           mRecordArmed             { false };
    bool                           mClipChanged { false };
    // Fader values
    double                         mVolume                  { 0 };
    double                         mPan                     { 0 };
    
    juce::uint8                    mClipSelected            { 0 };
    juce::uint8                    mLastClipSelected        { 0 };
    juce::File                     mLastRecording;
    juce::File                     mCurrentClip;
    
    // Audio stuff
    const juce::AudioDeviceManager *pDeviceManager;
    Recorder                       *pRecorder;
    AudioClip                      mAudioClip;
    
    /** Gui elements */
    juce::TextButton               mRecordArmButton         { "R" };
    juce::TextButton               mStopButton              { "S" };
    juce::TextButton               mMuteButton              { "M" };
    z_lib::ClipButton              mClipButtons[numClips];
    
    juce::Slider                   mVolumeSlider            { juce::Slider::SliderStyle::LinearBarVertical,
                                                             juce::Slider::NoTextBox };
    juce::Slider                   mPanSlider;
    
    /** Member functions */
    void         initGuiElements        ();
    void         updateState            (State state);
    juce::String clipPath               (juce::uint8 clip) const { return mClipsPath + "clip_" + juce::String(clip) + ".wav"; }
    
    /** Button click handlers */
    void         recordArmButtonClicked ();
    void         stopButtonClicked      ();
    void         muteButtonClicked      ();
    void         clipClicked            (juce::uint8 clipNum);
    
    /** Actions */
    void         startPlaying           (juce::uint8 clipNum);
    void         stopPlaying            ();
    void         startRecording         ();
    void         stopRecording          ();
    
    /** Clip functions */
    void         setSelectedClip        (int clipNum);
    int          getSelectedClip        ();
    void         clearSelectedClip      ();
    void         loadClip               (juce::uint8 clip);
    void         refreshClipStates      ();
    bool         clipExists             (juce::uint8 clipNum);

    /** State  handlers */
    void         playingState           ();
    void         recordingState         ();
    void         stoppedState           ();
    void         prepareToPlayState     ();
    void         prepareToRecordState   ();
    void         prepareToStopState     ();
    
};
} //z_lib
