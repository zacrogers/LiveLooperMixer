/*
  ==============================================================================

    AudioClip.cpp
    Created: 20 Apr 2023 10:32:09pm
    Author:  Zachary Rogers

  ==============================================================================
*/

#include "AudioClip.h"
namespace z_lib
{

AudioClip::AudioClip(): mGain(0.5), mFileLoaded(false)
{
    mFormatManager.registerBasicFormats();
}

AudioClip::~AudioClip() {}

void AudioClip::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    if (mSource) {
        mSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
}


void AudioClip::setSource(std::unique_ptr<PositionableAudioSource>& newSource)
{
    mSource.reset(newSource.release());
}

void AudioClip::setGain(float gain)
{
    mGain = gain;
}

void AudioClip::setNextReadPosition(juce::int64 newPosition)
{
    mSource->setNextReadPosition(newPosition);
}

juce::int64 AudioClip::getNextReadPosition() const
{
    return mSource->getNextReadPosition();
}

juce::int64 AudioClip::getTotalLength() const
{
    return mSource->getTotalLength();
}

bool AudioClip::isLooping() const
{
    return mSource->isLooping();
}

void AudioClip::releaseResources()
{
    if (mSource) mSource->releaseResources();
}

void AudioClip::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (mSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    mSource->getNextAudioBlock (bufferToFill);
}

void AudioClip::stop()
{
    mState = State::Stopped;
    mSource->setNextReadPosition(mSource->getTotalLength());
    sendChangeMessage();
}

void AudioClip::start()
{
    mState = State::Playing;
    restart();
    sendChangeMessage();
}

void AudioClip::restart()
{
    mSource->setNextReadPosition(0);
}

void AudioClip::load(juce::File file)
{
    auto* reader = mFormatManager.createReaderFor(file);
    
    if (reader != nullptr)
    {
        //If the selected file is valid, we create a temporary reader source
        std::unique_ptr<juce::PositionableAudioSource> tmpSource(new juce::AudioFormatReaderSource(reader, true));
        setSource(tmpSource);

        // Everything went fine, we can transfer the ownership and end the opening
        mFileLoaded = true;
    }
}


bool AudioClip::isPlaying()
{
    return mState == State::Playing;
}


}
