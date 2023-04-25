/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
#ifdef DEVELOPMENT
    juce::String filePath = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userDocumentsDirectory).getFullPathName();
    filePath = filePath + "/juce_looper_audio";
#else
    juce::String filePath = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile).getFullPathName();
#endif
    mProjectsPath = filePath + "/projects";
    
    
    for (juce::uint8 channel = 0; channel < numChannels; ++channel)
    {
        mChannelStrips[channel].init(&mAudioDeviceManager, &mRecorder);
        mChannelStrips[channel].addChangeListener(this);
        mChannelStrips[channel].updateClipsPath(mChannelPath(channel));
        mMixer.addInputSource((juce::PositionableAudioSource*)mChannelStrips[channel].pAudioClip(), true);
    }
    
	mInitGuiElements();

	formatManager.registerBasicFormats();
//	mMixer.addInputSource(&transportSource[0], true);
    
    juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                  [this] (bool granted)
                                  {
                                      int numInputChannels = granted ? 2 : 0;
                                      mAudioDeviceManager.initialise (numInputChannels, 2, nullptr, true, {}, nullptr);
                                  });
    mAudioDeviceManager.addAudioCallback(&mRecorder);
    setAudioChannels(1, 2);
    mMixer.addInputSource(&mAudioThru, true);
}


MainComponent::~MainComponent()
{
    shutdownAudio();// This shuts down the audio device and clears the audio source.
}


/**************************************
======== AudioAppComponent Overrides ========
*************************************************************/
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mMixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}


void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
//    if (mInputMuted)
//    {
//        bufferToFill.clearActiveBufferRegion();
//        return;
//    }

    
//    if (readerSource[0].get() == nullptr)
//    {
//        bufferToFill.clearActiveBufferRegion();
//        return;
//    }

    //transportSource.getNextAudioBlock(bufferToFill);
    mMixer.getNextAudioBlock(bufferToFill);
}


void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}


void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkblue);
}


void MainComponent::resized()
{
    
    auto area         = getLocalBounds();
    int  areaWidth    = area.getWidth();
    
    int  metroWidth   = 280;
    int  metroHeight  = 250;
    int  buttonSize   = 50;
    int  btnBuffer    = 15;
    int  sliderBuffer = 20;
    int  csBuffer     = 10;

    int  stripWidth   = areaWidth / 12;
    
    // Place items: left -> right, top -> bottom
    
    for (int strip = 0; strip < numChannels; ++strip)
    {
        mChannelStrips[strip].setBounds((csBuffer + strip) + (stripWidth * strip),
                                        0,
                                        stripWidth,
                                        area.getHeight());
    }
    mMetronome.setBounds(areaWidth-metroWidth-5, 5, metroWidth, metroHeight);
    
    // Button positions
    int tButtonX = areaWidth - buttonSize - btnBuffer; // Transport buttons x pos
    #define tButtonY(p) (metroHeight + (buttonSize * p) + (btnBuffer * p) + (btnBuffer * 2))
    
    mPlayButton.setBounds(tButtonX, tButtonY(0), buttonSize, buttonSize);
    mStopButton.setBounds(tButtonX, tButtonY(1), buttonSize, buttonSize);
    mArmRecordButton.setBounds(tButtonX, tButtonY(2), buttonSize, buttonSize);
    mInputMuteButton.setBounds(tButtonX, tButtonY(3), buttonSize, buttonSize);
    
    // Slider positions
    #define sliderX(pos) (tButtonX - (buttonSize * pos) - (sliderBuffer * pos)) //start pos from 1
    
    mInputTrimSlider.setBounds(sliderX(1), tButtonY(0), 50, 250);
    mMasterVolSlider.setBounds(sliderX(2), tButtonY(0), 50, 250);
    mNumRecBarsSlider.setBounds(sliderX(3), tButtonY(0), 50, 250);

}


/***********************************
========= ChangeListener Override =========
*********************************************************/
void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    DBG("Change listener");
    for(int i = 0; i < numChannels; ++i)
    {
//        if(source == &mChannelStrips[i])
//        {
//            DBG("***** Channel strip callback *****");
//
//            if(mChannelStrips[i].state() == z_lib::ChannelStrip::State::PreparingToPlay)
//            {
//                DBG("Preparing play:" << juce::String(i));
//            }
//            else if(mChannelStrips[i].state() == z_lib::ChannelStrip::State::PreparingToStop)
//            {
//                DBG("Preparing stop:" << juce::String(i));
//            }
//            else if(mChannelStrips[i].state() == z_lib::ChannelStrip::State::PreparingToRecord)
//            {
//                DBG("Preparing rec:" << juce::String(i));
//            }
//            else if(mChannelStrips[i].state() == z_lib::ChannelStrip::State::Stopped)
//            {
//                DBG("Stopped:" << juce::String(i));
//            }
//            else if(mChannelStrips[i].state() == z_lib::ChannelStrip::State::Playing)
//            {
//                DBG("Playing:" << juce::String(i));
//            }
//            else if(mChannelStrips[i].state() == z_lib::ChannelStrip::State::Recording)
//            {
//                DBG("Recording:" << juce::String(i));
//            }
//            DBG("*******************");
//        }
    }
}


/********************************
===========  Timer Override ===========
***************************************************/
void MainComponent::timerCallback()
{
    
    if (mRecorder.isRecording() && !mMetronome.countingIn())
    {
        for(juce::uint8 i = 0; i < numChannels; ++i)
        {
            if(mChannelStrips[i].isArmed())
            {
                mChannelStrips[i].record();
            }
        }
        
        DBG("Start recording");
    }
    
    mMetronome.advance();
    
    if (mPeriodEnded())
    {
        DBG("Period ended");
        for(juce::uint8 i = 0; i < numChannels; ++i)
        {
            if(mChannelStrips[i].state() == z_lib::ChannelStrip::State::PreparingToPlay)
            {
                mChannelStrips[i].play();
            }
            else if(mChannelStrips[i].state() == z_lib::ChannelStrip::State::PreparingToStop)
            {
                mChannelStrips[i].stop();
            }
            else if(mChannelStrips[i].state() == z_lib::ChannelStrip::State::PreparingToRecord)
            {
                mChannelStrips[i].record();
            }
            else if(mChannelStrips[i].state() == z_lib::ChannelStrip::State::Playing)
            {
                mChannelStrips[i].pAudioClip()->restart();
                mChannelStrips[i].pAudioClip()->start();
            }
            else if(mChannelStrips[i].state() == z_lib::ChannelStrip::State::Recording)
            {
                mChannelStrips[i].play();
            }
        }
    }
    
    
    if ((mRecordArmed && mRecorder.isRecording()))
    {
        if (mMetronome.numBars() == mNumRecBarsSlider.getValue())
        {
            mRecorder.stopRecording();

        }
    }
}


/********************************
========== Member Functions ==========
*****************************************************/
void MainComponent::mInitGuiElements()
{
    /** Buttons */
    addAndMakeVisible(&mPlayButton);
    mPlayButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    mPlayButton.onClick = [this] () { mPlayButtonClicked(); };
    
    addAndMakeVisible(&mStopButton);
    mStopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    mStopButton.setEnabled(false);
    mStopButton.onClick = [this] () { mStopButtonClicked(); };
    
    addAndMakeVisible(&mArmRecordButton);
    mArmRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    mArmRecordButton.onClick = [this] () { mRecordButtonClicked(); };
    
    addAndMakeVisible(&mInputMuteButton);
    mInputMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslateblue);
    mInputMuteButton.onClick = [this](){ mMuteButtonClicked(); };

    
    /** Sliders */
    addAndMakeVisible(&mInputTrimSlider);
    mInputTrimSlider.setRange(0, 1);
    mInputTrimSlider.setValue(0.0);
    mInputTrimSlider.onValueChange = [this] () { mAudioThru.setTrim(mInputTrimSlider.getValue()); };
    
    addAndMakeVisible(&mMasterVolSlider);
    mMasterVolSlider.setRange(0, 1);
    mMasterVolSlider.setValue(0.0);
    
    addAndMakeVisible(mNumRecBarsSlider);
    mNumRecBarsSlider.setRange(mMinRecordBars, mMaxRecordBars, 1);
    mNumRecBarsSlider.setValue(4);
    
    
    /** Labels */
    addAndMakeVisible(&mInputTrimLabel);
    mInputTrimLabel.attachToComponent(&mInputTrimSlider, false);
    
    addAndMakeVisible(&mMasterVolLabel);
    mMasterVolLabel.attachToComponent(&mMasterVolSlider, false);
    
    addAndMakeVisible(&mNumRecBarsLabel);
    mNumRecBarsLabel.attachToComponent(&mNumRecBarsSlider, false);
    
    /** Other stuff */
    addAndMakeVisible(mMetronome);
    
    for (juce::uint8 strip = 0; strip < numChannels; ++strip)
    {
        addAndMakeVisible(mChannelStrips[strip]);
    }

	setSize(900, 600);
}


void MainComponent::mChangeState(State newState)
{
    if (mState != newState)
    {
        mState = newState;

        switch (mState)
        {
            case State::Playing:
                mStatePlaying();
                break;
            case State::Recording:
                mStateRecording();
                break;
            case State::Stopped:
                mStateStopped();
                break;
            case State::PreparingToPlay:
                mStatePrep2Play();
                break;
            case State::PreparingToStop:
                mStatePrep2Stop();
                break;
            case State::PreparingToRecord:
                mStatePrep2Record();
                break;
        }
    }
}


bool MainComponent::mPeriodEnded()
{
    bool ended { false };
    
    if((mMetronome.previousBeat() == mMetronome.finalBeat()) &&
       (mMetronome.currentBeat() == mMetronome.startBeat()))
    {
        ended = true;
    }
    
    return ended;
}


juce::String MainComponent::mChannelPath(juce::uint8 channel) const
{
    return mProjectsPath + "/default_project" + "/channel_" + juce::String(channel) + "/";
}


/*********************************
============ State Functions ===========
*****************************************************/
void MainComponent::mStatePlaying()
{
    for(juce::uint8 channel = 0; channel < numChannels; ++channel)
    {
        mChannelStrips[channel].play();
    }
    
    startTimer(2400);

    if (mMetronome.isEnabled())
    {
        mMetronome.start();
        startTimer(mMetronome.getIntervalMs());
        mPlayButton.setEnabled(false);
        mStopButton.setEnabled(true);
        mArmRecordButton.setEnabled(false);
    }
}


void MainComponent::mStateRecording()
{
    for(juce::uint8 channel = 0; channel < numChannels; ++channel)
    {
        if(mChannelStrips[channel].isArmed())
        {
            mChannelStrips[channel].record();
            break;
        }
    }
    
    startTimer(2400);

    if (mMetronome.isEnabled())
    {
        mMetronome.start();
        startTimer(mMetronome.getIntervalMs());
        mPlayButton.setEnabled(false);
        mStopButton.setEnabled(true);
        mArmRecordButton.setEnabled(false);
    }
}


void MainComponent::mStateStopped()
{
    stopTimer();
    for(int i = 0; i < numChannels; ++i)
    {
        mChannelStrips[i].stop();
    }

    mMetronome.resetStep();
    mMetronome.stop();
    mPlayButton.setEnabled(true);
    mArmRecordButton.setEnabled(true);
}


void MainComponent::mStatePrep2Play()
{
    
}


void MainComponent::mStatePrep2Stop()
{
    
}


void MainComponent::mStatePrep2Record()
{
    
}


/*******************************
************* * Button Callbacks *************
************************************************/
void MainComponent::mPlayButtonClicked()
{
    if(State::Recording         == mState ||
       State::Stopped           == mState ||
       State::PreparingToRecord == mState)
    {
        mChangeState(State::PreparingToPlay);
    }
    else if(State::PreparingToStop == mState)
    {
        mChangeState(State::Playing);
    }
}


void MainComponent::mStopButtonClicked()
{
    if(State::Playing == mState || State::Recording == mState)
    {
        mChangeState(State::PreparingToStop);
    }
    else if(State::PreparingToPlay   == mState ||
       State::PreparingToStop   == mState ||
       State::PreparingToRecord == mState)
    {
        mChangeState(State::Stopped);
    }

}


void MainComponent::mRecordButtonClicked()
{
    if(State::Recording == mState)
    {
        mChangeState(State::PreparingToPlay);
    }
    else if(State::PreparingToPlay == mState)
    {
        mChangeState(State::PreparingToRecord);
    }
    else if(State::Stopped == mState)
    {
        mRecordArmed = !mRecordArmed;
    }
    
    
    if(mRecordArmed)
    {
        mArmRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        
        for(juce::uint8 i = 0; i < numChannels; ++i)
        {
            if(mChannelStrips[i].isArmed())
            {
                mChannelStrips[i].setState(z_lib::ChannelStrip::State::PreparingToRecord);
            }
        }
    }
    else
    {
        mArmRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    }
}

void MainComponent::mMuteButtonClicked()
{
    mInputMuted = !mInputMuted;
    
    if (mInputMuted)
    {
        mInputMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blueviolet);
    }
    else
    {
        mInputMuteButton.setColour(juce::TextButton::buttonColourId, mDisabledColour);
    }
}


/***************************
======== Slider Callbacks ========
*******************************************/
void MainComponent::mInputTrimChanged()
{
    
}


void MainComponent::mMasterVolChanged()
{
    
}


void MainComponent::volumeChanged(int channel)
{
//    if (channel < NUM_CHANNELS)
//    {
//        mChannelStrips[channel].pAudioClip()->setGain(this->mInputTrimSlider.getValue());
//    }
}
