#include "ChannelStrip.h"


namespace z_lib
{
/* *===============================
        Component Overrides
=================================* */

void ChannelStrip::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkblue);
    
    /// Draw border
    g.setColour(juce::Colours::darkslategrey);
    g.drawRect(getLocalBounds());
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



/* *===============================
     Change Listener Override
=================================* */
void ChannelStrip::changeListenerCallback(juce::ChangeBroadcaster* source)
{

}


/* *===============================
           Channel Strip
=================================* */

/* *================ Public =================* */
ChannelStrip::ChannelStrip(){}

ChannelStrip::~ChannelStrip(){}


void ChannelStrip::init(const juce::AudioDeviceManager *deviceManager, z_lib::Recorder *recorder, State *state, juce::String clipsPath)
{
    pDeviceManager = deviceManager;
    pRecorder      = recorder;
    pState         = state;
    mClipsPath     = clipsPath;
    
    initGuiElements();
    for(juce::uint8 clip = 0; clip < numClips; ++clip)
    {
        mClipButtons[clip].onClick = [this, clip] () { clipClicked(clip); };
        
        mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::chartreuse);
        if(clipExists(clip))
        {
            mClipButtons[clip].setEnabled(true);
        }
        else
        {
            mClipButtons[clip].setEnabled(false);
        }
    }
    refreshClipStates();
}


void ChannelStrip::updateState()
{
    updateState(*pState);
}


/* *================= Private ================* */

void ChannelStrip::initGuiElements()
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
    mRecordArmButton.onClick = [this] () { recordArmButtonClicked(); };
    if(mRecordArmed)
    {
        mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    }
    else
    {
        mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    }
    
    addAndMakeVisible(mMuteButton);
    mMuteButton.onClick = [this] () { muteButtonClicked(); };
    mMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
    
    addAndMakeVisible(mStopButton);
    mStopButton.onClick = [this] () { stopButtonClicked(); };
    
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
        DBG("VOL:" << juce::String(mVolume));
        mAudioClip.setGain(mVolume);
        
    };
    
    setSize(10, 600);
}


void ChannelStrip::updateState(State state)
{
    sendChangeMessage();
    DBG("Chan state:" << juce::String(static_cast<int>(state)));
        
    switch (state)
    {
        case State::Playing:
            playingState();
            break;
            
        case State::Stopped:
            stoppedState();
            break;
            
        case State::Recording:
            recordingState();
            break;
            
        case State::PreparingToPlay:
            prepareToPlayState();
            break;
            
        case State::PreparingToStop:
            prepareToStopState();
            break;
            
        case State::PreparingToRecord:
            prepareToRecordState();
            break;
            
        default:
            break;
    }
}


/* *========== Button Click handlers ==========* */

void ChannelStrip::recordArmButtonClicked()
{
    mRecordArmed = !mRecordArmed;
    
//    mRefreshClipStates();
    if(State::Stopped == *pState)
    {
        if(mRecordArmed)
        {
            for(juce::uint8 clip = 0; clip < numClips; ++clip)
            {
                if(clipExists(clip))
                {
                    mClipButtons[clip].setEnabled(true);
                    mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::chartreuse);
                }
                else if(clip == getSelectedClip()) { }
                else
                {
                    mClipButtons[clip].setEnabled(true);
                    mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::orange);
                }
            }
            
            mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
            mClipButtons[getSelectedClip()].setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        }
        else
        {
            for(juce::uint8 clip = 0; clip < numClips; ++clip)
            {
                if(clipExists(clip))
                {
                    mClipButtons[clip].setEnabled(true);
                    mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::chartreuse);
                }
                else if(clip == getSelectedClip()) { }
                else
                {
                    mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::chartreuse);
                    mClipButtons[clip].setEnabled(false);
                }
            }
            
            mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
            mClipButtons[getSelectedClip()].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
        }
    }
    
    
//    if(State::PreparingToRecord == *pState ||
//       State::Recording == *pState)
//    {
////        mUpdateState(State::PreparingToStop);
//        mRecordArmed = false;
//    }
//    else if (State::PreparingToPlay == *pState ||
//             State::PreparingToStop == *pState ||
//             State::Playing         == *pState ||
//             State::Stopped         == *pState)
//    {
////        mUpdateState(State::PreparingToRecord);
//        mRecordArmed = true;
//    }
//    
    sendChangeMessage();
}


void ChannelStrip::stopButtonClicked()
{
    if (State::Playing           == *pState ||
        State::Recording         == *pState ||
        State::PreparingToPlay   == *pState ||
        State::PreparingToRecord == *pState)
    {
//        mUpdateState(State::PreparingToStop);
    }
    else if(State::PreparingToStop == *pState)
    {
//        mUpdateState(State::Stopped);
    }

    sendChangeMessage();
}


void ChannelStrip::muteButtonClicked()
{
    mMuted =! mMuted;
    
    mAudioClip.setMuted(mMuted);
    
    if (mMuted)
    {
        mMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    }
    else
    {
        mMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
    }
    refreshClipStates();
    sendChangeMessage();
}


void ChannelStrip::clipClicked(juce::uint8 clipNum)
{
    if(clipNum == mClipSelected) return;
    
    mClipChanged = true;
    
    DBG("Clip pressed: " << juce::String(clipNum));

    clearSelectedClip();
    setSelectedClip(clipNum);
    
    for(juce::uint8 clip = 0; clip < numClips; ++clip)
    {
        if(clipExists(clip))
        {
            mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::yellow);
        }
        else if(clip == getSelectedClip()) { }
        else
        {
            mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::chartreuse);
        }
    }

    if(State::Playing == *pState)
    {
    }
    else if(State::Stopped == *pState)
    {
        if(mRecordArmed && clipExists(getSelectedClip()))
        {
            mClipButtons[getSelectedClip()].setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        }
        else
        {
            mClipButtons[getSelectedClip()].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
        }
    }
    
    sendChangeMessage();
}


/* *================= Actions =================* */

void ChannelStrip::startPlaying(juce::uint8 clipNum)
{
    if(!mAudioClip.isLoaded() || (mClipSelected != mLastClipSelected))
    {
        loadClip(clipNum);
        
    }
    if(mAudioClip.isLoaded())
    {
        mAudioClip.start();
    }
    sendChangeMessage();
}


void ChannelStrip::stopPlaying()
{
    sendChangeMessage();
}


void ChannelStrip::startRecording()
{
    sendChangeMessage();

    if (! juce::RuntimePermissions::isGranted(juce::RuntimePermissions::writeExternalStorage))
    {
        SafePointer<ChannelStrip> safeThis (this);

        juce::RuntimePermissions::request(juce::RuntimePermissions::writeExternalStorage,
                                          [safeThis] (bool granted) mutable {
            if (granted)
            {
                safeThis->startRecording();
            }
        });

        return;
    }

    
    auto fname = clipPath(mClipSelected);
    juce::File file = juce::File(fname);
    file.deleteFile();
    mLastRecording = juce::File(fname);

    DBG("START REC: "<< fname);
    DBG("START: " << mLastRecording.getFullPathName());

    pRecorder->startRecording(mLastRecording);
}


void ChannelStrip::stopRecording()
{
    pRecorder->stopRecording();
    mLastRecording = juce::File();
    sendChangeMessage();
}


/* *=============== Clip Functions ===============* */

void ChannelStrip::setSelectedClip(int clipNum)
{
    if(mClipSelected == clipNum) {
        return;
    }
    mLastClipSelected = mClipSelected;
    mClipSelected = clipNum;
    
    switch (*pState) {
        case State::PreparingToPlay:
        {
            if(clipExists(mClipSelected))
            {
                 mClipButtons[mClipSelected].setState(ClipButton::State::PreparingToPlay);
            }
            else
            {
                mClipButtons[mClipSelected].setState(ClipButton::State::Empty);
            }
            break;
        }
//        case State::Playing:           mClipButtons[clipNum].setState(ClipButton::State::Playing);
//        case State::Recording:         mClipButtons[clipNum].setState(ClipButton::State::Recording);
//        case State::PreparingToRecord: mClipButtons[clipNum].setState(ClipButton::State::PreparingToRecord);
//        case State::PreparingToStop:   mClipButtons[clipNum].setState(ClipButton::State::PreparingToStop);
            
        default: break;
    }
    
    mClipButtons[clipNum].setSelected(true);
}


int ChannelStrip::getSelectedClip()
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


void ChannelStrip::clearSelectedClip()
{
    for (juce::uint8 channel = 0; channel < numClips; ++channel)
    {
        mClipButtons[channel].setSelected(false);
    }
}


void ChannelStrip::loadClip(juce::uint8 clip)
{
    DBG("LOADING: " << clipPath(clip));
    mAudioClip.load(juce::File(clipPath(clip)));
}


void ChannelStrip::refreshClipStates()
{
    for(juce::uint8 clip = 0; clip < numClips; ++clip)
    {

        if((getSelectedClip() == clip))
        {
            mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
            mClipButtons[getSelectedClip()].setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        }
        else
        {
            mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
            mClipButtons[getSelectedClip()].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
        }
    }
}


bool ChannelStrip::clipExists(juce::uint8 clipNum)
{
    auto file = juce::File(clipPath(clipNum));
    return file.exists();
}


/* *============== State Handlers ==============* */

void ChannelStrip::playingState()
{
    setSelectedClip(mClipSelected);
    startPlaying(mClipSelected);

    mStopButton.setEnabled(true);
    mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    
    //mSetUnselectedClip(mClipSelected);
}


void ChannelStrip::recordingState()
{
    if(mRecordArmed)
    {
        DBG("Armed and playing");
        startRecording();
        mStopButton.setEnabled(true);
        mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    }
    else
    {
        DBG("unarmed, should be playing");
        playingState();
    }
}


void ChannelStrip::stoppedState()
{
    //    if (State::PreparingToRecord == *pState || State::PreparingToStop == *pState)
    //    {
    //        mStopPlaying();
    //        mUpdateState(State::Stopped);
    //    }
    //    else if (State::Playing == *pState)
    //    {
    //        mUpdateState(State::PreparingToStop);
    //    }
    stopRecording();
    mAudioClip.restart();
    mAudioClip.stop();
    mStopButton.setEnabled(false);
    mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
}


void ChannelStrip::prepareToPlayState()
{
    mStopButton.setEnabled(true);
    mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
}


void ChannelStrip::prepareToRecordState()
{
    mStopButton.setEnabled(true);
    mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
}


void ChannelStrip::prepareToStopState()
{
    mStopButton.setEnabled(false);
    mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
}


} // z_lib
