#pragma once
#include <JuceHeader.h>
#include <string.h>
#include <cstdint>

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
    
    enum class State
    {
        Playing,
        Recording,
        Stopped,
        PreparingToPlay,
        PreparingToStop,
        PreparingToRecord
    };
    
    struct Snapshot
    {
        std::uint8_t currentClip;
        std::uint8_t nextClip;
        State        state;
    };

    // Component methods
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    // Change Listener
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    // Channel Strip
    ChannelStrip();
    ChannelStrip(juce::AudioDeviceManager &deviceManager);
    ChannelStrip(juce::AudioDeviceManager &deviceManager, Recorder &recorder);
    ~ChannelStrip();
    
    void init(const juce::AudioDeviceManager *deviceManager, Recorder *recorder);
    
    void setDeviceManager();
    void setRecorder();
    
    /** These should be used for external control */
    void play();
    void stop();
    void record();
    
    void setState(State state) { mChangeState(state); }
    
    void setArmed(bool armed);
    bool isArmed() const { return mRecordArmed; }
    
    struct Snapshot snapshot() const { return mSnapshot; };
    
    void prepareToRecord();
    // needs to be called if project is reloaded because the project id will change
    // TODO: find a better way to manage the file paths/locations
    void updateClipsPath(juce::String path) { mClipsPath = path; DBG("Clips path: " << path);};
    
    State state() const { return mState; }
    
    AudioClip *pAudioClip() { return &mAudioClip; };
    
private:
    AudioClip mAudioClip;
    juce::String mClipsPath;
    
    juce::String mClipPath(juce::uint8 clip) const { return mClipsPath + "clip_" + juce::String(clip) + ".wav"; }
    
    
    // Member variables
    State mState          { State::Stopped };
    
    struct Snapshot mSnapshot = { 0 };

    bool mMuted           { false };
    bool mSoloed          { false };
    bool mRecordArmed     { false };

    double mVolume        { 0 };
    double mPan           { 0 };
    
    int mClipSelected     { 0 };
    int mLastClipSelected { 0 };
    
    const juce::AudioDeviceManager *pDeviceManager;
    Recorder                 *pRecorder;

    juce::File               mLastRecording;
    juce::File               mCurrentClip;
    
    // Gui elements
    juce::TextButton         mRecordArmButton { "R" };
    juce::TextButton         mStopButton      { "S" };
    juce::TextButton         mMuteButton      { "M" };
    z_lib::ClipButton        mClipButtons[numClips];
    
    juce::Slider             mVolumeSlider { juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::NoTextBox };
    juce::Slider             mPanSlider;
    
    // Member functions
    void mInitGuiElements();
    
    void mChangeState(State state);
    
    void mRecordArmButtonClicked();
    void mStopButtonClicked();
    void mMuteButtonClicked();
    void mClipClicked(int clipNum);
    
    void mStartPlaying(juce::uint8 clipNum);
    void mStopPlaying();
    void mStartRecording();
    void mStopRecording();
    void mLoadClip(juce::uint8 clip);
    
    // clip functions
    void setClipsColour();
    void mSetSelectedClip(int clipNum);
    void mSetUnselectedClips();
    int  mGetSelectedClip();
    void mRefreshClipStates();
    bool mClipExists(juce::uint8 clipNum);
    
    // Gui state setters
    void setPlayingState();
    void setRecordingState();
    void setStoppedState();
    void setP2PlayState();
    void setP2RecordState();
    void setP2StopState();
    
};
} //z_lib
