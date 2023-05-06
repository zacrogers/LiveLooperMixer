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
    
    mInitGuiElements();
    for(juce::uint8 clip = 0; clip < numClips; ++clip)
    {
        mClipButtons[clip].onClick = [this, clip] () { mClipClicked(clip); };
        
        mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::chartreuse);
        if(mClipExists(clip))
        {
            mClipButtons[clip].setEnabled(true);
        }
        else
        {
            mClipButtons[clip].setEnabled(false);
        }
    }
    mRefreshClipStates();
}


void ChannelStrip::updateState()
{
    mUpdateState(*pState);
}


/* *================= Private ================* */

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
        DBG("VOL:" << juce::String(mVolume));
        mAudioClip.setGain(mVolume);
        
    };
    
    setSize(10, 600);
}


void ChannelStrip::mUpdateState(State state)
{
    sendChangeMessage();
    DBG("Chan state:" << juce::String(static_cast<int>(state)));
        
    switch (state)
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


/* *========== Button Click handlers ==========* */

void ChannelStrip::mRecordArmButtonClicked()
{
    mRecordArmed = !mRecordArmed;
    
//    mRefreshClipStates();
    if(State::Stopped == *pState)
    {
        if(mRecordArmed)
        {
            for(juce::uint8 clip = 0; clip < numClips; ++clip)
            {
                if(mClipExists(clip))
                {
                    mClipButtons[clip].setEnabled(true);
                    mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::chartreuse);
                }
                else if(clip == mGetSelectedClip()) { }
                else
                {
                    mClipButtons[clip].setEnabled(true);
                    mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::orange);
                }
            }
            
            mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
            mClipButtons[mGetSelectedClip()].setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        }
        else
        {
            for(juce::uint8 clip = 0; clip < numClips; ++clip)
            {
                if(mClipExists(clip))
                {
                    mClipButtons[clip].setEnabled(true);
                    mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::chartreuse);
                }
                else if(clip == mGetSelectedClip()) { }
                else
                {
                    mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::chartreuse);
                    mClipButtons[clip].setEnabled(false);
                }
            }
            
            mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
            mClipButtons[mGetSelectedClip()].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
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


void ChannelStrip::mStopButtonClicked()
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


void ChannelStrip::mMuteButtonClicked()
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
    mRefreshClipStates();
    sendChangeMessage();
}


void ChannelStrip::mClipClicked(juce::uint8 clipNum)
{
    if(clipNum == mClipSelected) return;
    
    mClipChanged = true;
    
    DBG("Clip pressed: " << juce::String(clipNum));

    mClearSelectedClip();
    mSetSelectedClip(clipNum);
    
    for(juce::uint8 clip = 0; clip < numClips; ++clip)
    {
        if(mClipExists(clip))
        {
            mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::yellow);
        }
        else if(clip == mGetSelectedClip()) { }
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
        if(mRecordArmed && mClipExists(mGetSelectedClip()))
        {
            mClipButtons[mGetSelectedClip()].setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        }
        else
        {
            mClipButtons[mGetSelectedClip()].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
        }
    }
    
    sendChangeMessage();
}


/* *================= Actions =================* */

void ChannelStrip::mStartPlaying(juce::uint8 clipNum)
{
    if(!mAudioClip.isLoaded() || (mClipSelected != mLastClipSelected))
    {
        mLoadClip(clipNum);
        
    }
    if(mAudioClip.isLoaded())
    {
        mAudioClip.start();
    }
    sendChangeMessage();
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
    juce::File file = juce::File(fname);
    file.deleteFile();
    mLastRecording = juce::File(fname);

    DBG("START REC: "<< fname);
    DBG("START: " << mLastRecording.getFullPathName());

    pRecorder->startRecording(mLastRecording);
}


void ChannelStrip::mStopRecording()
{
    pRecorder->stopRecording();
    mLastRecording = juce::File();
    sendChangeMessage();
}


/* *=============== Clip Functions ===============* */

void ChannelStrip::mSetSelectedClip(int clipNum)
{
    if(mClipSelected == clipNum) {
        return;
    }
    mLastClipSelected = mClipSelected;
    mClipSelected = clipNum;
    
    switch (*pState) {
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
//        case State::Playing:           mClipButtons[clipNum].setState(ClipButton::State::Playing);
//        case State::Recording:         mClipButtons[clipNum].setState(ClipButton::State::Recording);
//        case State::PreparingToRecord: mClipButtons[clipNum].setState(ClipButton::State::PreparingToRecord);
//        case State::PreparingToStop:   mClipButtons[clipNum].setState(ClipButton::State::PreparingToStop);
            
        default: break;
    }
    
    mClipButtons[clipNum].setSelected(true);
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


void ChannelStrip::mClearSelectedClip()
{
    for (juce::uint8 channel = 0; channel < numClips; ++channel)
    {
        mClipButtons[channel].setSelected(false);
    }
}


void ChannelStrip::mLoadClip(juce::uint8 clip)
{
    DBG("LOADING: " << mClipPath(clip));
    mAudioClip.load(juce::File(mClipPath(clip)));
}


void ChannelStrip::mRefreshClipStates()
{
    for(juce::uint8 clip = 0; clip < numClips; ++clip)
    {

        if((mGetSelectedClip() == clip))
        {
            mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
            mClipButtons[mGetSelectedClip()].setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        }
        else
        {
            mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
            mClipButtons[mGetSelectedClip()].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
        }
    }
}


bool ChannelStrip::mClipExists(juce::uint8 clipNum)
{
    auto file = juce::File(mClipPath(clipNum));
    return file.exists();
}


/* *============== State Handlers ==============* */

void ChannelStrip::mPlayingState()
{
    mSetSelectedClip(mClipSelected);
    mStartPlaying(mClipSelected);

    mStopButton.setEnabled(true);
    mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    
    //mSetUnselectedClip(mClipSelected);
}


void ChannelStrip::mRecordingState()
{
    if(mRecordArmed)
    {
        DBG("Armed and playing");
        mStartRecording();
        mStopButton.setEnabled(true);
        mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    }
    else
    {
        DBG("unarmed, should be playing");
        mPlayingState();
    }
}


void ChannelStrip::mStoppedState()
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
    mStopRecording();
    mAudioClip.restart();
    mAudioClip.stop();
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
