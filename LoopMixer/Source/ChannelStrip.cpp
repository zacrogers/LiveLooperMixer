#include "ChannelStrip.h"


namespace z_lib
{

ChannelStrip::ChannelStrip(juce::AudioDeviceManager &deviceManager, Recorder &recorder)
{
    pDeviceManager = &deviceManager;
    pRecorder = &recorder;
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
    
    mVolumeSlider.setBounds(0,
                            (area.getHeight() - (buttonHeight * 8) - (vertBuffer * 2)),
                            buttonWidth,
                            buttonHeight * 4.5);
    
    mMuteButton.setBounds(0,
                          (area.getHeight() - buttonHeight - vertBuffer),
                          buttonWidth,
                          buttonHeight);
    
    mStopButton.setBounds(0,
                          (area.getHeight() - (buttonHeight * 2) - (vertBuffer * 2)),
                          buttonWidth,
                          buttonHeight);
    
    mRecordArmButton.setBounds(0,
                               (area.getHeight() - (buttonHeight * 3) - (vertBuffer * 3)),
                               buttonWidth,
                               buttonHeight);
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
    if (mState == State::Playing || mState == State::PreparingToStop)
    {
        mStopPlaying();
        mState = State::Stopped;
    }
}


void ChannelStrip::record()
{
    if(mState == State::PreparingToRecord)
    {
        mStartRecording(mChannelNum, mLastClipSelected);
        mState = State::Recording;
    }
}


void ChannelStrip::mInitGuiElements()
{
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

void ChannelStrip::mChangeState()
{
    sendChangeMessage();
    switch (mState)
    {
        case State::Playing :
            mStopButton.setEnabled(true);
            break;
        case State::Stopped :
            mStopButton.setEnabled(false);
            break;
        case State::Recording :
            mStopButton.setEnabled(true);
            break;
        case State::PreparingToPlay :
            mStopButton.setEnabled(true);
            break;
        case State::PreparingToStop :
            mStopButton.setEnabled(false);
            break;

        default:
            break;
    }
}


void ChannelStrip::mRecordArmButtonClicked()
{
    if(mState == State::Stopped)
    {
        mRecordArmed = !mRecordArmed;
        
        if(mRecordArmed)
        {
            mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        }
        else
        {
            mRecordArmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        }
    }
}


void ChannelStrip::mStopButtonClicked()
{
    if(mState == State::Playing)
    {
        mState = State::PreparingToStop;
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
}


void ChannelStrip::mStartPlaying(int clipNum)
{
    sendChangeMessage();
}


void ChannelStrip::mStopPlaying()
{
    sendChangeMessage();
}


void ChannelStrip::mStartRecording(int channelNum, int clipNum)
{
    sendChangeMessage();
}


void ChannelStrip::mStopRecording()
{
    sendChangeMessage();
}


void ChannelStrip::mLoadClip()
{
    
}


}



ChannelStrip::ChannelStrip()
{
//	basePath = getBasePath();
	initGuiElements();
}

ChannelStrip::~ChannelStrip()
{

}

void ChannelStrip::initGuiElements()
{
	setSize(40, 600);

	/* Setup volume slider */
	addAndMakeVisible(&volumeSlider);
	volumeSlider.setRange(0, 100);
	volumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::aquamarine);

	/* Setup buttons */
	addAndMakeVisible(&stopBtn);
	stopBtn.setButtonText("Stop");
	stopBtn.onClick = [this] { stopBtnClicked(); };

	addAndMakeVisible(&muteBtn);
	muteBtn.setButtonText("Mute");
	muteBtn.onClick = [this] { muteBtnClicked(); };

	addAndMakeVisible(&recBtn);
	recBtn.setButtonText("Rec");
	recBtn.onClick = [this] { recBtnClicked(); };

	addAndMakeVisible(&soloBtn);
	soloBtn.setButtonText("Solo");
	soloBtn.onClick = [this] { soloBtnClicked(); };

	for (int btn = 0; btn < NUM_CLIPS; ++btn)
	{
		addAndMakeVisible(&clipBtns[btn]);
		clipBtns[btn].setColour(juce::TextButton::buttonColourId, juce::Colours::green);
		clipBtns[btn].onClick = [this, btn] { clipBtnClicked(btn); };
	}
}

void ChannelStrip::paint(juce::Graphics&)
{

}

void ChannelStrip::resized()
{
	int btnHeight = 30;
	int btnOffset = 5;

	for (int btn = 0; btn < NUM_CLIPS; ++btn)
	{
		clipBtns[btn].setBounds(0, (btnHeight * btn) + (btnOffset * btn), getWidth(), btnHeight);
	}

	volumeSlider.setBounds(0, (btnHeight * NUM_CLIPS) + (btnOffset + NUM_CLIPS) + 10, 
		                   getWidth(), getHeight() - btnHeight * (4 + NUM_CLIPS) - btnOffset * (4 + NUM_CLIPS));

	stopBtn.setBounds(0, getHeight() - btnHeight, getWidth(), btnHeight);
	muteBtn.setBounds(0, getHeight() - (btnHeight * 2) - btnOffset, getWidth(), btnHeight);
	recBtn.setBounds(0, getHeight() - (btnHeight * 3) - (btnOffset * 2), getWidth(), btnHeight);
	soloBtn.setBounds(0, getHeight() - (btnHeight * 4) - (btnOffset * 3), getWidth(), btnHeight);
}


void ChannelStrip::changeState(TransportState newState)
{
	if (state != newState)
	{
		state = newState;

		switch (state)
		{
		case TransportState::STOPPED:

			break;

		case TransportState::STARTING:

			break;

		case TransportState::PLAYING:

			break;

		case TransportState::STOPPING:

			break;
		}
	}
}

void ChannelStrip::loadClip(int slot, std::string clipName)
{
	auto slotStr = std::to_string(slot);
	auto chanNumStr = std::to_string(slot);
	auto clipPath = basePath + "\\Clips\\" + chanNumStr + clipName + slotStr + ".wav";

	std::cout << clipPath << std::endl;

	auto file = juce::File(clipPath);
	auto* reader = formatManager.createReaderFor(file);

	if (reader != nullptr)
	{
		std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
		transportSource[0].setSource(newSource.get(), 0, nullptr, reader->sampleRate);
		/*
		mixer.addInputSource(&transportSource[0], true);
		playButton.setEnabled(true);
		readerSource[slot].reset(newSource.release());
		*/
	}
}


void ChannelStrip::addToMixer(juce::MixerAudioSource* mixer)
{
	for (int clip = 0; clip < NUM_CLIPS; ++clip)
	{
		mixer->addInputSource(&transportSource[clip], true);
	}
}


void ChannelStrip::clipBtnClicked(int clip)
{
	if (clip != currentClip)
	{

	}
}

void ChannelStrip::stopBtnClicked()
{

}

void ChannelStrip::muteBtnClicked()
{

}

void ChannelStrip::soloBtnClicked()
{

}

void ChannelStrip::recBtnClicked()
{

}

//std::string ChannelStrip::getBasePath()
//{
////	char fullPathToExe[MAX_PATH]; // Contains executable file name
////	auto spath = GetModuleFileName(NULL, fullPathToExe, MAX_PATH);
////	auto finalSlashIndex = ((std::string)fullPathToExe).find_last_of("\\");
////
////	return ((std::string)fullPathToExe).substr(0, finalSlashIndex);
//    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
//}
