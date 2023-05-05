/*
  ==============================================================================

    AudioClip.h
    Created: 20 Apr 2023 10:32:09pm
    Author:  Zachary Rogers

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Recorder.h"

namespace z_lib
{
class AudioClip : public juce::PositionableAudioSource, juce::ChangeBroadcaster
{
public:
    enum class State
    {
        Playing,
        Stopped
    };
    
    AudioClip();
    AudioClip(juce::AudioDeviceManager &deviceManager, Recorder &recorder);
    ~AudioClip();

    // AudioSource
    void        prepareToPlay       (int samplesPerBlockExpected, double sampleRate)   override;
    void        releaseResources    ()                                                 override;
    void        getNextAudioBlock   (const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    // PositionalAudioSource
    void        setNextReadPosition (juce::int64 newPosition)                          override;
    juce::int64 getNextReadPosition () const                                           override;
    juce::int64 getTotalLength      () const                                           override;
    bool        isLooping           () const                                           override;

    
    /** Tells the source whether you'd like it to play in a loop. */
//    void setLooping (bool shouldLoop) override;

    // AudioClip
    void        start               ();
    void        stop                ();
    void        load                (juce::File file);
    void        restart             ();
    void        setGain             (float gain);
    void        setMuted            (bool muted) { mMuted = muted; }
    void        setSource           (std::unique_ptr<PositionableAudioSource>& newSource);
    bool        isPlaying           ();
    bool        isLoaded            () const { return mFileLoaded; }
    
private:
    juce::AudioFormatManager                 mFormatManager;
    std::unique_ptr<PositionableAudioSource> mSource;
    float                                    mGain;
    State                                    mState;
    bool                                     mFileLoaded;
    bool                                     mMuted;
};

}
