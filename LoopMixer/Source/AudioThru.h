/*
  ==============================================================================

    AudioThru.h
    Created: 30 Jan 2022 11:18:16am
    Author:  Zachary Rogers

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace z_lib
{
class AudioThru : public juce::AudioSource
{

public:
    AudioThru(juce::AudioDeviceManager &deviceManager);
    ~AudioThru();
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    void setTrim(float value);
    void setPan(float value);
    
private:
    juce::AudioDeviceManager *mAudioDeviceManager;
    
    double mTrimValue { 0.5 };
    double mPanValue  { 0.9 };
    bool mMuted       { false };
    
};
};


