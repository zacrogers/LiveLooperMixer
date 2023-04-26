#include "ChannelStrip.h"


namespace z_lib
{

ChannelStrip::ChannelStrip(){}

ChannelStrip::~ChannelStrip(){}


void ChannelStrip::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkblue);
    
    /// Draw border
    g.setColour(juce::Colours::darkslategrey);
    g.drawRect(getLocalBounds());
}


void ChannelStrip::updateState()
{
    mUpdateState(*pState);
}


void ChannelStrip::resized()
{
    auto area = getLocalBounds();
    
    int buttonHeight = area.getHeight() / 12;
    int buttonWidth = area.getWidth();
    
    int vertBuffer = area.getHeight() / 80;
    
    for(int i = 0; i < numClips; ++i)
    {
        mClipButtons[i].setBounds(0, (buttonHeight * i), buttonWidth, buttonHeight);
    }
    
    
    mVolumeSlider.setBounds(0,
                            (area.getHeight() - (buttonHeight * 8) + (vertBuffer * 2)),
                            buttonWidth,
                            buttonHeight * 4);
    
    mRecordArmButton.setBounds(0,
                          (area.getHeight() - buttonHeight - vertBuffer),
                          buttonWidth,
                          buttonHeight);
    
    mStopButton.setBounds(0,
                          (area.getHeight() - (buttonHeight * 2) - (vertBuffer * 2)),
                          buttonWidth,
                          buttonHeight);
    
    mMuteButton.setBounds(0,
                         (area.getHeight() - (buttonHeight * 3) - (vertBuffer * 3)),
                         buttonWidth,
                         buttonHeight);
}


void ChannelStrip::init(const juce::AudioDeviceManager *deviceManager, z_lib::Recorder *recorder, State *state)
{
    pDeviceManager = deviceManager;
    pRecorder      = recorder;
    pState         = state;
    
    for (juce::uint8 i = 0; i < numClips; ++i)
    {
        mClipButtons[i].onClick = [this, i] () { mClipClicked(i); };
    }
    
    mInitGuiElements();
}


void ChannelStrip::changeListenerCallback(juce::ChangeBroadcaster* source)
{

}


void ChannelStrip::play()
{
    if (mState == State::Stopped || mState == State::PreparingToPlay)
    {   
        mState = State::Playing;
        mStartPlaying(mLastClipSelected);
    }
}


void ChannelStrip::stop()
{
    if (State::PreparingToRecord == mState || State::PreparingToStop == mState)
    {
        mStopPlaying();
        mUpdateState(State::Stopped);
    }
    else if (State::Playing == mState)
    {
        mUpdateState(State::PreparingToStop);
    }
}


void ChannelStrip::record()
{
    if(mState == State::PreparingToRecord)
    {
        // set clip num
        mClipSelected = 0;
        mStartRecording();
        mState = State::Recording;
    }
}


void ChannelStrip::prepareToPlay()
{
    
}


void ChannelStrip::prepareToStop()
{
    
}


void ChannelStrip::prepareToRecord()
{
    if((mState == State::PreparingToRecord) && (mRecordArmed == true))
    {
        mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    }
}



void ChannelStrip::mInitGuiElements()
{
//    if (pRecorder == nullptr)
//    {
//        throw "Channel strip recorder is null";
//    }
//    
//    if (pDeviceManager == nullptr)
//    {
//        throw "Channel strip device manager is null";
//    }
    
    /** Buttons */
    addAndMakeVisible(mRecordArmButton);
    mRecordArmButton.onClick = [this] () { mRecordArmButtonClicked(); };
    if(mRecordArmed)
    {
        mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    }
    else
    {
        mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    }
    
    addAndMakeVisible(mMuteButton);
    mMuteButton.onClick = [this] () { mMuteButtonClicked(); };
    mMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
    
    addAndMakeVisible(mStopButton);
    mStopButton.onClick = [this] () { mStopButtonClicked(); };
    
    for (int i = 0; i < numClips; ++i)
    {
        addAndMakeVisible(mClipButtons[i]);
    }
    
    /** Sliders */
    addAndMakeVisible(mVolumeSlider);
    mVolumeSlider.setRange(0, 1);
    mVolumeSlider.setValue(mVolume);
    mVolumeSlider.onValueChange = [this] ()
    {
        mVolume = mVolumeSlider.getValue();
        mAudioClip.setGain(mVolume);
    };
    
    setSize(10, 600);
}


void ChannelStrip::mUpdateState(State state)
{
    if (state != mState)
    {
        mState = state;
        sendChangeMessage();
        
        switch (mState)
        {
            case State::Playing:
                mPlayingState();
                break;
                
            case State::Stopped:
                mStoppedState();
                break;
                
            case State::Recording:
                mRecordingState();
                break;
                
            case State::PreparingToPlay:
                mPrepareToPlayState();
                break;
                
            case State::PreparingToStop:
                mPrepareToStopState();
                break;
                
            case State::PreparingToRecord:
                mPrepareToRecordState();
                break;
                
            default:
                break;
        }
    }
}



void ChannelStrip::mRecordArmButtonClicked()
{
    if(State::PreparingToRecord == mState ||
       State::Recording == mState)
    {
        mUpdateState(State::PreparingToStop);
        mRecordArmed = false;
    }
    else if (State::PreparingToPlay == mState ||
             State::PreparingToStop == mState ||
             State::Playing         == mState ||
             State::Stopped         == mState)
    {
        mUpdateState(State::PreparingToRecord);
        mRecordArmed = true;
    }
    
    sendChangeMessage();
}


void ChannelStrip::mStopButtonClicked()
{
    if (State::Playing           == mState ||
        State::Recording         == mState ||
        State::PreparingToPlay   == mState ||
        State::PreparingToRecord == mState)
    {
        mUpdateState(State::PreparingToStop);
    }
    else if(State::PreparingToStop == mState)
    {
        mUpdateState(State::Stopped);
    }

    sendChangeMessage();
}


void ChannelStrip::mMuteButtonClicked()
{
    mMuted =! mMuted;
    
    if (mMuted)
    {
        mMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    }
    else
    {
        mMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
    }
    sendChangeMessage();
}


void ChannelStrip::mClipClicked(juce::uint8  clipNum)
{
    if(clipNum == mClipSelected) return;

    mClearSelectedClip();
    mSetSelectedClip(mClipSelected);

    if(State::Playing == mState && mRecordArmed && mClipExists(clipNum))
    {
        mUpdateState(State::PreparingToRecord);
    }
    else if(State::Stopped == mState)
    {
        if(mRecordArmed)
        {
            mUpdateState(State::PreparingToRecord);
        }
        else
        {
            mUpdateState(State::PreparingToPlay);
        }
    }
    
    sendChangeMessage();
}

void ChannelStrip::mClearSelectedClip()
{
    for (juce::uint8 channel = 0; channel < numClips; ++channel)
    {
        mClipButtons[channel].setSelected(false);
    }
}


void ChannelStrip::mRefreshClipStates()
{
    // check if clips are empty
}


void ChannelStrip::mStartPlaying(juce::uint8 clipNum)
{
    if(State::Playing == mState)
    {
        mLoadClip(clipNum);
        if(mAudioClip.isLoaded())
        {
            mAudioClip.start();
        }
        sendChangeMessage();
    }
}


void ChannelStrip::mStopPlaying()
{
    sendChangeMessage();
}


void ChannelStrip::mStartRecording()
{
    sendChangeMessage();

    if (! juce::RuntimePermissions::isGranted(juce::RuntimePermissions::writeExternalStorage))
    {
        SafePointer<ChannelStrip> safeThis (this);

        juce::RuntimePermissions::request(juce::RuntimePermissions::writeExternalStorage,
                                          [safeThis] (bool granted) mutable {
            if (granted)
            {
                safeThis->mStartRecording();
            }
        });

        return;
    }

    auto fname = mClipPath(mClipSelected);
    
//    DBG("START REC: "<< fname);
//
    juce::File file = juce::File(fname);
    file.deleteFile();
//    auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
//    mLastRecording = parentDir.getNonexistentChildFile("PoopMixerFile", ".wav");
    
    mLastRecording = juce::File(fname);

    DBG("START: " << mLastRecording.getFullPathName());

    pRecorder->startRecording(mLastRecording);
}


void ChannelStrip::mStopRecording()
{
    pRecorder->stopRecording();
    mLoadClip(mClipSelected);
    mLastRecording = juce::File();
    sendChangeMessage();
}


void ChannelStrip::mLoadClip(juce::uint8 clip)
{
    mAudioClip.load(juce::File(mClipPath(clip)));
}



void ChannelStrip::mSetSelectedClip(int clipNum)
{
    mClipSelected = clipNum;
    
    switch (mState) {
        case State::PreparingToPlay:
        {
            if(mClipExists(mClipSelected))
            {
                 mClipButtons[mClipSelected].setState(ClipButton::State::PreparingToPlay);
            }
            else
            {
                mClipButtons[mClipSelected].setState(ClipButton::State::Empty);
            }
            break;
        }
        case State::Playing:           mClipButtons[clipNum].setState(ClipButton::State::Playing);
        case State::Recording:         mClipButtons[clipNum].setState(ClipButton::State::Recording);
        case State::PreparingToRecord: mClipButtons[clipNum].setState(ClipButton::State::PreparingToRecord);
        case State::PreparingToStop:   mClipButtons[clipNum].setState(ClipButton::State::PreparingToStop);
            
        default: break;
    }
    
    mClipButtons[clipNum].setSelected(true);
}



bool ChannelStrip::mClipExists(juce::uint8 clipNum)
{
    DBG("CS: Clip Exists " << mClipPath(clipNum));
    auto file = juce::File(mClipPath(clipNum));
    return file.exists();
}


int ChannelStrip::mGetSelectedClip()
{
    int clipNum = 0;
    
    for (int i = 0; i < numClips; ++i)
    {
        if(mClipButtons[i].isSelected())
        {
            clipNum = i;
            break;
        }
    }
    return clipNum;
}


void ChannelStrip::mPlayingState()
{
    if (mState == State::Stopped || mState == State::PreparingToPlay)
    {
        mState = State::Playing;
        mStartPlaying(mLastClipSelected);
    }
    mStopButton.setEnabled(true);
    mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    
    mSetSelectedClip(mClipSelected);
    //mSetUnselectedClip(mClipSelected);
}


void ChannelStrip::mRecordingState()
{
    mStopButton.setEnabled(true);
    mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
}


void ChannelStrip::mStoppedState()
{
    mStopButton.setEnabled(false);
    mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
}


void ChannelStrip::mPrepareToPlayState()
{
    mStopButton.setEnabled(true);
    mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
}


void ChannelStrip::mPrepareToRecordState()
{
    mStopButton.setEnabled(true);
    mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
}


void ChannelStrip::mPrepareToStopState()
{
    mStopButton.setEnabled(false);
    mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
}


} // z_lib
