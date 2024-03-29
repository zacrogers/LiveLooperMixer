/*
  ==============================================================================

    AudioThru.cpp
    Created: 30 Jan 2022 11:18:16am
    Author:  Zachary Rogers

  ==============================================================================
*/

#include "AudioThru.h"

namespace z_lib
{

AudioThru::AudioThru(juce::AudioDeviceManager &deviceManager)
{
    mAudioDeviceManager = &deviceManager;
}

AudioThru::~AudioThru(){}


void AudioThru::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    
}


void AudioThru::releaseResources()
{
    
}

void AudioThru::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    auto* device = mAudioDeviceManager->getCurrentAudioDevice();
    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();

    auto maxInputChannels  = activeInputChannels .getHighestBit() + 1;
    auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;
    
    for (auto channel = 0; channel < maxOutputChannels; ++channel)
    {
        
        if ((! activeOutputChannels[channel]) || maxInputChannels == 0)
        {
            bufferToFill.buffer->clear (channel, bufferToFill.startSample, bufferToFill.numSamples);
        }
        
        else if (maxInputChannels == 1) // Mono to stereo
        {
            auto* inBuffer = bufferToFill.buffer->getReadPointer (0, bufferToFill.startSample);         
            auto* outBuffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);
            

//            if (channel == 0) // left channel
//            {
//                for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
//                    outBuffer[sample] = inBuffer[sample] * mTrimValue * (1  - mPanValue);
//            }
//            else if(channel == 1) // right channel
//            {
//                for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
//                    outBuffer[sample] = inBuffer[sample] * mTrimValue * mPanValue;
//            }
            
            for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                outBuffer[sample] = (inBuffer[sample] * mTrimValue);
        }
        
        else
        {
            auto actualInputChannel = channel % maxInputChannels; // [1]

            if (! activeInputChannels[channel]) // [2]
            {
                bufferToFill.buffer->clear (channel, bufferToFill.startSample, bufferToFill.numSamples);
            }
            else // [3]
            {
                auto* inBuffer = bufferToFill.buffer->getReadPointer (actualInputChannel, bufferToFill.startSample);
                auto* outBuffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);

                
                for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    outBuffer[sample] = (inBuffer[sample] * mTrimValue);
                }
            }
        }
    }
}


void AudioThru::setTrim(float value)
{
    if(value > 1)
    {
        mTrimValue = 1;
    }
    else if (value <= 0)
    {
        mTrimValue = 0;
    }
    else
    {
        mTrimValue = value;
    }
    
    DBG("TRIM:"<< mTrimValue);
}

void AudioThru::setPan(float value)
{
    if(value > 1)
    {
        mPanValue = 1;
    }
    else if (value <= 0)
    {
        mPanValue = 0;
    }
    else
    {
        mPanValue = value;
    }
}

}
