#include "ChannelStrip.h"


namespace z_lib
{


ChannelStrip::ChannelStrip()
{
    for (int i = 0; i < numClips; ++i)
    {
        mClipButtons[i].addChangeListener(this);
        mClipButtons[i].onClick = [this, i] () { mClipClicked(i); };
    }
    mInitGuiElements();
}

ChannelStrip::ChannelStrip(juce::AudioDeviceManager &deviceManager, Recorder &recorder)
{
    pDeviceManager = &deviceManager;
    pRecorder = &recorder;
    
    for (int i = 0; i < numClips; ++i)
    {
        mClipButtons[i].addChangeListener(this);
        mClipButtons[i].onClick = [this, i] () { mClipClicked(i); };
    }
    
    mInitGuiElements();
}


ChannelStrip::~ChannelStrip()
{
    
}


void ChannelStrip::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    
}


void ChannelStrip::releaseResources()
{
    
}


void ChannelStrip::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    
}


void ChannelStrip::paint (juce::Graphics& g)
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

void ChannelStrip::init(const juce::AudioDeviceManager &deviceManager, z_lib::Recorder &recorder, int channelNum, int numClips)
{
    
}


void ChannelStrip::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    for (int i = 0; i < numClips; ++i)
    {
        if (source == &mClipButtons[i])
        {
            mSetSelectedClip(i);
        }
    }
}


void ChannelStrip::play()
{
    if (mState == State::Stopped || mState == State::PreparingToPlay)
    {   
        mStartPlaying(mLastClipSelected);
        mState = State::Playing;
    }
}


void ChannelStrip::stop()
{
//    if (mState == State::Playing || mState == State::PreparingToStop)
//    {
        mStopPlaying();
        mChangeState(State::Stopped);
//    }
}


void ChannelStrip::record()
{
    if(mState == State::PreparingToRecord)
    {
        // set clip num
        mStartRecording();
        mState = State::Recording;
    }
}


void ChannelStrip::setArmed(bool armed)
{
    mRecordArmed = armed;
    
    if(mRecordArmed)
    {
        mState = State::PreparingToRecord;
        mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    }
    else
    {
        mState = State::Stopped;
        mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    }
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
    mVolumeSlider.setValue(mVolume);
    mVolumeSlider.onValueChange = [this] ()
    {
        mVolume = mVolumeSlider.getValue();
    };
    
    setSize(10, 600);
}

void ChannelStrip::mChangeState(State state)
{
//    DBG("CS state changed");
    
    if (state != mState)
    {
        mState = state;
        sendChangeMessage();
        
        switch (mState)
        {
            case State::Playing:
                mStopButton.setEnabled(true);
                break;
                
            case State::Stopped:
                mStopButton.setEnabled(false);
                break;
                
            case State::Recording:
                mStopButton.setEnabled(true);
                break;
                
            case State::PreparingToPlay:
                mStopButton.setEnabled(true);
                break;
                
            case State::PreparingToStop:
                mStopButton.setEnabled(false);
                break;
                
            default:
                break;
        }
    }
}


void ChannelStrip::mRecordArmButtonClicked()
{
//    mRecordArmed = !mRecordArmed;
    
    
    if(State::PreparingToRecord == mState ||
       State::Recording == mState)
    {
        mChangeState(State::PreparingToStop);
        mRecordArmed = false;
    }
    else if (State::PreparingToPlay == mState ||
             State::PreparingToStop == mState ||
             State::Playing         == mState ||
             State::Stopped         == mState)
    {
        mChangeState(State::PreparingToRecord);
        mRecordArmed = true;
    }
    
//    DBG("Rec : Record armed: " << (mRecordArmed ? "true" : "false"));
//    DBG("Rec : State:" << juce::String(static_cast<int>(mState)));
    
    setClipsColour();
}


void ChannelStrip::mStopButtonClicked()
{
    if (State::Playing           == mState ||
        State::Recording         == mState ||
        State::PreparingToPlay   == mState ||
        State::PreparingToRecord == mState)
    {
        mChangeState(State::PreparingToStop);
    }
    else if(State::PreparingToStop == mState)
    {
        mChangeState(State::Stopped);
    }

//    DBG("Stop : State:" << juce::String(static_cast<int>(mState)));
    sendChangeMessage();
    setClipsColour();
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
    setClipsColour();
}

void ChannelStrip::mClipClicked(int clipNum)
{
    mClipSelected = clipNum;
    sendChangeMessage();
}


void ChannelStrip::mStartPlaying(int clipNum)
{
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

    auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    mLastRecording = parentDir.getNonexistentChildFile("CHANNEL_" + juce::String(mChannelNum) + "_" + juce::String(mClipSelected), ".wav");

    DBG("START: " << mLastRecording.getFullPathName());

    pRecorder->startRecording(mLastRecording);
}

//void MainComponent::mStartRecording()
//{
//    if (! juce::RuntimePermissions::isGranted(juce::RuntimePermissions::writeExternalStorage))
//    {
//        SafePointer<MainComponent> safeThis (this);
//
//        juce::RuntimePermissions::request(juce::RuntimePermissions::writeExternalStorage,
//                                          [safeThis] (bool granted) mutable {
//            if (granted)
//            {
//                safeThis->mStartRecording();
//            }
//        });
//
//        return;
//    }
//
//    auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
//
//    mLastRecording = parentDir.getNonexistentChildFile("LoopMixerFile", ".wav");
//
//    DBG("START: " << mLastRecording.getFullPathName());
//
//    mRecorder.startRecording(mLastRecording);
//}


void ChannelStrip::mStopRecording()
{
    sendChangeMessage();
}


void ChannelStrip::mLoadClip()
{
    
}


void ChannelStrip::setClipsColour()
{
    for(int i = 0; i < numClips; ++i)
    {
        if (mMuted)
        {
            mClipButtons[i].setState(z_lib::ClipButton::State::Muted);
        }
        else
        {
            if(mState == State::Recording)
            {
                mClipButtons[i].setState(z_lib::ClipButton::State::Recording);
            }
            else if(mState == State::PreparingToRecord)
            {
                mClipButtons[i].setState(z_lib::ClipButton::State::PreparingToRecord);
            }
            else if(mState == State::Playing)
            {
                mClipButtons[i].setState(z_lib::ClipButton::State::Playing);
            }
            else if(mState == State::PreparingToPlay)
            {
                mClipButtons[i].setState(z_lib::ClipButton::State::PreparingToPlay);
            }
        }
    }
    
}

void ChannelStrip::mSetSelectedClip(int clipNum)
{
    for (int i = 0; i < numClips; ++i)
    {
        if (i == clipNum)
        {
            mClipButtons[i].setSelected(true);
        }
        else
        {
            mClipButtons[i].setSelected(false);
        }
    }
}


int ChannelStrip::mGetSelectedClip()
{
    int clipNum = 0;
    
    for (int i = 0; i < numClips; ++i)
    {
        if(mClipButtons[i].isSelected())
        {
            clipNum = i;
        }
    }
    return clipNum;
}


} // z_lib

//std::string ChannelStrip::getBasePath()
//{
////	char fullPathToExe[MAX_PATH]; // Contains executable file name
////	auto spath = GetModuleFileName(NULL, fullPathToExe, MAX_PATH);
////	auto finalSlashIndex = ((std::string)fullPathToExe).find_last_of("\\");
////
////	return ((std::string)fullPathToExe).substr(0, finalSlashIndex);
//    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
//}
