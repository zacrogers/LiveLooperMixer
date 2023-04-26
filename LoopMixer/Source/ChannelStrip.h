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
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    /** Change Listener override */
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    /** Channel Strip */
    ChannelStrip();
    ~ChannelStrip();
    
    void init(const juce::AudioDeviceManager *deviceManager, Recorder *recorder, State *state);
    void updateState(); /** Should be called whenever the parents state changes */
    
    // needs to be called if project is reloaded because the project id will change
    // TODO: find a better way to manage the file paths/locations
    void updateClipsPath(juce::String path) { mClipsPath = path; DBG("Clips path: " << path);};
    
    /** Getters */
    struct Snapshot  snapshot    () const { return mSnapshot; }
    const  AudioClip *pAudioClip ()       { return &mAudioClip; }
    
private:
    // Member variables
    AudioClip                      mAudioClip;
    juce::String                   mClipsPath;
    
    State                          mState                   { State::Stopped };
    State                          *pState;
    
    struct Snapshot                mSnapshot                { 0 };

    bool                           mMuted                   { false };
    bool                           mSoloed                  { false };
    bool                           mRecordArmed             { false };

    double                         mVolume                  { 0 };
    double                         mPan                     { 0 };
    
    juce::uint8                    mClipSelected            { 0 };
    juce::uint8                    mLastClipSelected        { 0 };
    
    const juce::AudioDeviceManager *pDeviceManager;
    Recorder                       *pRecorder;

    juce::File                     mLastRecording;
    juce::File                     mCurrentClip;
    
    /** Gui elements */
    juce::TextButton               mRecordArmButton        { "R" };
    juce::TextButton               mStopButton             { "S" };
    juce::TextButton               mMuteButton             { "M" };
    z_lib::ClipButton              mClipButtons[numClips];
    
    juce::Slider                   mVolumeSlider           { juce::Slider::SliderStyle::LinearBarVertical,
                                                            juce::Slider::NoTextBox };
    juce::Slider                   mPanSlider;
    
    /** Member functions */
    void mInitGuiElements();
    void mUpdateState(State state);
    juce::String mClipPath(juce::uint8 clip) const { return mClipsPath + "clip_" + juce::String(clip) + ".wav"; }
    
    /** Button click handlers */
    void mRecordArmButtonClicked();
    void mStopButtonClicked();
    void mMuteButtonClicked();
    void mClipClicked(juce::uint8 clipNum);
    
    /** Actions */
    void mStartPlaying(juce::uint8 clipNum);
    void mStopPlaying();
    void mStartRecording();
    void mStopRecording();
    void mLoadClip(juce::uint8 clip);
    void mClearSelectedClip();
    
    /** Clip functions */
    void mSetSelectedClip(int clipNum);
    int  mGetSelectedClip();
    void mRefreshClipStates();
    bool mClipExists(juce::uint8 clipNum);

    /** State  handlers */
    void mPlayingState();
    void mRecordingState();
    void mStoppedState();
    void mPrepareToPlayState();
    void mPrepareToRecordState();
    void mPrepareToStopState();
    
    /* TODO: Get Rid of **/
    void play();
    void stop();
    void record();
    void prepareToPlay();
    void prepareToStop();
    void prepareToRecord();
    
};
} //z_lib
