/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

/**
    Main structure:
        : button press -> click handler -> set state -> state handler
 
        - the click handler is responsible for setting the state based on current conditions.
        - the state handler is responsible for the state logic, and updating the state of the channel strips.
 */


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
        mChannelStrips[channel].init(&mAudioDeviceManager, &mRecorder, &mState, channelPath(channel));
        mChannelStrips[channel].addChangeListener(this);
        mChannelStrips[channel].updateClipsPath(channelPath(channel));
        mMixer.addInputSource((juce::PositionableAudioSource*)mChannelStrips[channel].pAudioClip(), true);
    }
    
	initGuiElements();

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
    if (bufferToFill.buffer == nullptr)
    {
        return;
    }
    mMixer.getNextAudioBlock(bufferToFill);
    
    /** Set  master gain */
    auto level = (float) mMasterVolSlider.getValue();
    
    for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        auto* inBuffer = bufferToFill.buffer->getReadPointer (channel, bufferToFill.startSample);
        auto* outBuffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);
        
        for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            outBuffer[sample] = inBuffer[sample] * level;
        }
    }
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
//    DBG("Change listener");
    for(juce::uint8 chan = 0; chan < numChannels; ++chan)
    {
        if(source == &mChannelStrips[chan])
        {
            if(mChannelStrips[chan].isArmed())
            {
                for(juce::uint8 _chan = 0; _chan < numChannels; ++_chan)
                {
                    if(chan != _chan)
                    {
                        mChannelStrips[_chan].setArmed(false);
                    }
                }
            }
        }
    }
}


/********************************
===========  Timer Override ===========
***************************************************/
void MainComponent::timerCallback()
{
    
    if(mTimerFirstFire)
    {
        DBG("Timer: First fire");
        updateChannelsState();
        mTimerFirstFire = false;
    }
    
    if (periodEnded())
    {
        DBG("Timer: Period ended");
        handlePeriodEnded();
        updateChannelsState();
    }
    
    mMetronome.advance();
}


/********************************
========== Member Functions ==========
*****************************************************/
void MainComponent::initGuiElements()
{
    /** Buttons */
    addAndMakeVisible(&mPlayButton);
    mPlayButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    mPlayButton.onClick = [this] () { playButtonClicked(); };
    
    addAndMakeVisible(&mStopButton);
    mStopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    mStopButton.setEnabled(false);
    mStopButton.onClick = [this] () { stopButtonClicked(); };
    
    addAndMakeVisible(&mArmRecordButton);
    mArmRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    mArmRecordButton.onClick = [this] () { recordButtonClicked(); };
    
    addAndMakeVisible(&mInputMuteButton);
    mInputMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslateblue);
    mInputMuteButton.onClick = [this](){ muteButtonClicked(); };

    
    /** Sliders */
    addAndMakeVisible(&mInputTrimSlider);
    mInputTrimSlider.setRange(0, 1);
    mInputTrimSlider.setValue(0.0);
    mInputTrimSlider.onValueChange = [this] () { mAudioThru.setTrim(mInputTrimSlider.getValue());};
    
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


void MainComponent::updateChannelsState()
{
    for(juce::uint8 chan = 0; chan < numChannels; ++chan)
    {
        mChannelStrips[chan].updateState();
    }
}


void MainComponent::changeState(State newState)
{
    if (mState != newState)
    {
        mState = newState;
        DBG("New state: " << juce::String(static_cast<int>(mState)));
        switch (mState)
        {
            case State::Playing:
                statePlaying();
                break;
            case State::Recording:
                stateRecording();
                break;
            case State::Stopped:
                stateStopped();
                break;
            case State::PreparingToPlay:
                statePrepareToPlay();
                break;
            case State::PreparingToStop:
                statePrepareToStop();
                break;
            case State::PreparingToRecord:
                statePrepareToRecord();
                break;
        }
    }
}


bool MainComponent::periodEnded()
{
    bool ended { false };
    
    if((mMetronome.previousBeat() == mMetronome.finalBeat()) &&
       (mMetronome.currentBeat()  == mMetronome.startBeat()))
    {
        ended = true;
    }
    
    return ended;
}


void MainComponent::handlePeriodEnded()
{
    switch (mState)
    {
        case State::Recording:         changeState(State::Playing);   break;
        case State::PreparingToPlay:   changeState(State::Playing);   break;
        case State::PreparingToStop:   changeState(State::Stopped);   break;
        case State::PreparingToRecord: changeState(State::Recording); break;
        case State::Playing:           changeState(State::Playing);   break;
        case State::Stopped: break;
    }
}


juce::String MainComponent::channelPath(juce::uint8 channel) const
{
    return mProjectsPath + "/default_project" + "/channel_" + juce::String(channel) + "/";
}


juce::uint8 MainComponent::getNumArmed()
{
    juce::uint8 nArmed = 0;
    
    for (juce::uint8 chan = 0; chan < numChannels; ++chan)
    {
        if(mChannelStrips[chan].isArmed())
        {
            nArmed++;
        }
    }
    
    return nArmed;
}


/*********************************
============ State Functions ===========
*****************************************************/
void MainComponent::statePlaying()
{
    if (mMetronome.isEnabled())
    {
        mTimerFirstFire = true;

        mMetronome.start();
        startTimer(mMetronome.getIntervalMs());
        
        mPlayButton.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::green);
        mStopButton.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::white);
        
        mPlayButton.setEnabled(false);
        mStopButton.setEnabled(true);
        mArmRecordButton.setEnabled(false);
    }
}


void MainComponent::stateRecording()
{
    // If theres no channels armed, we don't really want to record, do we?
    if (getNumArmed() == 0)
    {
        changeState(State::Playing);
        return;
    }
    
    if (mMetronome.isEnabled())
    {
        mTimerFirstFire = true;

        mMetronome.start();
        startTimer(mMetronome.getIntervalMs());
        
        mPlayButton.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::white);
        mStopButton.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::white);
        
        mPlayButton.setEnabled(false);
        mStopButton.setEnabled(true);
        mArmRecordButton.setEnabled(false);
    }
}


void MainComponent::stateStopped()
{
    stopTimer();
    updateChannelsState();
    
    if ((mRecordArmed && mRecorder.isRecording()))
    {
        if (mMetronome.numBars() == mNumRecBarsSlider.getValue())
        {
            mRecorder.stopRecording();
        }
    }

    mMetronome.resetStep();
    mMetronome.stop();
    
    for(juce::uint8 chan = 0; chan < numChannels; ++chan)
    {
        if(mChannelStrips[chan].isArmed())
        {
            mChannelStrips[chan].setArmed(true);
        }
    }
    mPlayButton.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::white);
    mStopButton.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::white);
    
    mPlayButton.setEnabled(true);
    mArmRecordButton.setEnabled(true);
    mStopButton.setEnabled(false);
}


void MainComponent::statePrepareToPlay()
{
    updateChannelsState();
    
    mPlayButton.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::orange);
    mStopButton.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::white);

    
    mPlayButton.setEnabled(false);
    mArmRecordButton.setEnabled(false);
    mStopButton.setEnabled(true);
    
    changeState(State::Playing);
}


void MainComponent::statePrepareToStop()
{
    updateChannelsState();
    
    mPlayButton.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::white);
    mStopButton.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::orange);
    
    mPlayButton.setEnabled(true);
    mArmRecordButton.setEnabled(false);
    mStopButton.setEnabled(true);
}


void MainComponent::statePrepareToRecord()
{
    updateChannelsState();
    
    mPlayButton.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::red);
    mStopButton.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::white);
    
    mPlayButton.setEnabled(false);
    mArmRecordButton.setEnabled(false);
    mStopButton.setEnabled(true);
    
    changeState(State::Recording);
}


/*******************************
************* * Button Callbacks *************
************************************************/
void MainComponent::playButtonClicked()
{
    if(mRecordArmed)
    {
        changeState(State::PreparingToRecord);
    }
    else if(State::Recording == mState ||
            State::Stopped   == mState)
    {
        changeState(State::PreparingToPlay);
    }
    else if(State::PreparingToStop == mState)
    {
        changeState(State::Playing);
    }
}


void MainComponent::stopButtonClicked()
{
    if(State::Playing == mState || State::Recording == mState)
    {
        changeState(State::PreparingToStop);
    }
    else if(State::PreparingToPlay    == mState ||
            State::PreparingToStop    == mState ||
            State::PreparingToRecord  == mState)
    {
        changeState(State::Stopped);
    }
}


void MainComponent::recordButtonClicked()
{
    if(State::Recording == mState)
    {
        changeState(State::PreparingToPlay);
    }
    else if(State::PreparingToPlay == mState)
    {
        changeState(State::PreparingToRecord);
    }
    else if(State::Stopped == mState)
    {
        mRecordArmed = !mRecordArmed;
        
        if (mRecordArmed)
        {
            mArmRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
            mArmRecordButton.setColour(juce::ComboBox::outlineColourId,  juce::Colours::red);
        }
        else
        {
            mArmRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
            mArmRecordButton.setColour(juce::ComboBox::outlineColourId,  juce::Colours::darkred);
        }

    }
    resized();
}


void MainComponent::muteButtonClicked()
{
    mInputMuted = !mInputMuted;
    
    if (mInputMuted)
    {
        mInputMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blueviolet);
    }
    else
    {
        mInputMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::purple);
        mInputMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::purple);
    }
}


/***************************
======== Slider Callbacks ========
*******************************************/
void MainComponent::inputTrimChanged()
{
    mAudioThru.setTrim(mInputTrimSlider.getValue());
}


void MainComponent::masterVolChanged()
{
    
}


void MainComponent::volumeChanged(int channel)
{
//    if (channel < NUM_CHANNELS)
//    {
//        mChannelStrips[channel].pAudioClip()->setGain(this->mInputTrimSlider.getValue());
//    }
}
