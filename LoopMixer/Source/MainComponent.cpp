/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
//    for (int i = 0; i < 4; ++i)
//    {
//        mClipButtons.add(std::unique_ptr<juce::TextButton*>());
//    }
    
    
	initGuiElements();

	formatManager.registerBasicFormats();
	transportSource[0].addChangeListener(this);
	transportSource[1].addChangeListener(this);
    
    mChannelStrip.addChangeListener(this);
    
    juce::String filePath = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile).getFullPathName();
    mClipsPath = filePath + "/clips";

//	basePath = getExePath();
       
//	autoLoadClip("lmdrum", 0);
//	autoLoadClip("lmbass", 1);

//	mixer.addInputSource(&transportSource[0], true);

//    // Some platforms require permissions to open input channels so request that here
//    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
//        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
//    {
//        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
//                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
//    }
//    else
//    {
//        // Specify the number of input and output channels that we want to open
//        setAudioChannels (2, 2);
//    }
    juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                  [this] (bool granted)
                                  {
                                      int numInputChannels = granted ? 2 : 0;
                                      mAudioDeviceManager.initialise (numInputChannels, 2, nullptr, true, {}, nullptr);
                                  });
     mAudioDeviceManager.addAudioCallback(&mRecorder);
    
     
    setAudioChannels(1, 2);
    
    mixer.addInputSource(&mAudioThru, true); 
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void MainComponent::volumeChanged(int channel)
{
	if (channel < NUM_CHANNELS)
	{
		transportSource[channel].setGain(volumeSliders[channel].getValue());
	}
}



void MainComponent::initGuiElements()
{
//	strips[0].clipBtns[0].onClick = [this] { playBtnClicked(); };
//	strips[0].recBtn.onClick = [this] { recordClip(); };

	for (int chan = 0; chan < NUM_CHANNELS; ++chan)
	{
		//addAndMakeVisible(&strips[chan]);
		//strips[chan].setChannelNum(chan);

		addAndMakeVisible(&volumeSliders[chan]);
		volumeSliders[chan].setRange(0, 1);
		volumeSliders[chan].setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		volumeSliders[chan].onValueChange = [this, chan]{ volumeChanged(chan); };

	}
    
    int channel = 1;
    
    for(int clip = 0; clip < 4; ++clip)
    {
        addAndMakeVisible(mClipButtons[clip]);
        mClipButtons[clip].setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        mClipButtons[clip].onClick = [this, channel, clip] ()
        {
            mHandleClipButton(channel, clip);
        };
    }
    
    
//	addAndMakeVisible(&loadButton);
//	loadButton.setButtonText("Load");
//	loadButton.onClick = [this] { loadBtnClicked(0); };
//
//	addAndMakeVisible(&playButton);
//	playButton.setButtonText("Play");
//	playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
//	playButton.onClick = [this] { playBtnClicked(); };
//
//	addAndMakeVisible(&stopButton);
//	stopButton.setButtonText("Stop");
//	stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
//	stopButton.onClick = [this] { stopBtnClicked(); };
//
//	addAndMakeVisible(&play1);
//	play1.setButtonText("Play1");
//	play1.onClick = [this] { playClip(0); };
//
//	addAndMakeVisible(&play2);
//	play2.setButtonText("Play2");
//	play2.onClick = [this] { playClip(1); };
    
    /** Buttons */
    addAndMakeVisible(&mPlayButton);
    mPlayButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    mPlayButton.onClick = [this] () { mStartPlaying(); };
    
    addAndMakeVisible(&mStopButton);
    mStopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    mStopButton.setEnabled(false);
    mStopButton.onClick = [this] () { mStopPlaying(); };
    
    addAndMakeVisible(&mArmRecordButton);
    mArmRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    mArmRecordButton.onClick = [this] ()
    {
        mRecordArmed = !mRecordArmed;
        
        if(mRecordArmed)
        {
            mArmRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
            
            for(int i = 0; i < numClips; ++i)
            {
                if(mClipEmpty[i])
                {
                    mClipButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::red);
                }
                else
                {
                    mClipButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::green);
                }
            }
        }
        else{
            mArmRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    
            for(int i = 0; i < numClips; ++i)
            {
                if(mClipEmpty[i])
                {
                    mClipButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::blue );
                }
                else
                {
                    mClipButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::green);
                }
            }
        }
    };
    
    addAndMakeVisible(&mInputMuteButton);
    mInputMuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslateblue);
    mInputMuteButton.onClick = [this]()
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
    };
    
    /** Sliders */
    addAndMakeVisible(&mInputTrimSlider);
    mInputTrimSlider.setRange(0, 1);
    mInputTrimSlider.setValue(0.0);
    mInputTrimSlider.onValueChange = [this] ()
    {
        mAudioThru.setTrim(mInputTrimSlider.getValue());
    };
    
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
    
    addAndMakeVisible(mChannelStrip);

	setSize(900, 600);
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
	mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
//    if (mInputMuted)
//    {
//        bufferToFill.clearActiveBufferRegion();
//        return;
//    }

    
//	if (readerSource[0].get() == nullptr)
//	{
//		bufferToFill.clearActiveBufferRegion();
//		return;
//	}

	//transportSource.getNextAudioBlock(bufferToFill);
	mixer.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkblue);
}

void MainComponent::resized()
{
    
    auto area = getLocalBounds();
    int areaWidth = area.getWidth();
    
    int metroWidth = 280;
    int metroHeight = 250;
    int buttonSize = 50;
    int btnBuffer = 15;
    int sliderBuffer = 20;
    int csBuffer = 10;
    
    // Place items: left -> right, top -> bottom
    
    mChannelStrip.setBounds(csBuffer, 0, areaWidth / 12, area.getHeight());
    
//    for (int chan = 0; chan < NUM_CHANNELS; ++chan)
//    {
//        volumeSliders[chan].setBounds(0 + (chan * 60), 0, 40, getHeight());
//        //strips[chan].setBounds(0 + (chan * 60), 0, 40, getHeight());
//    }

//    loadButton.setBounds(NUM_CHANNELS * 60, 50, 80, 40);
//    playButton.setBounds(NUM_CHANNELS * 60, 110, 80, 40);
//    stopButton.setBounds(NUM_CHANNELS * 60, 170, 80, 40);
//
//    loadButton2.setBounds(NUM_CHANNELS * 60, 220, 80, 40);
    
    
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
    
    
    for(int i = 0; i < 4; ++i)
    {
        mClipButtons[i].setBounds(sliderX(4), i*50, 50, 50);
    }
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (source == &transportSource[0])
	{
		if (transportSource[0].isPlaying())
		{
			changeState(TransportState::PLAYING);
		}
		else
		{
			changeState(TransportState::STOPPED);
		}
	}
    
    if(source == &mChannelStrip)
    {
        DBG("***** Channel strip callback *****");
        if(mChannelStrip.state() == z_lib::ChannelStrip::State::PreparingToPlay)
        {
            DBG("Preparing play");
        }
        else if(mChannelStrip.state() == z_lib::ChannelStrip::State::PreparingToStop)
        {
            DBG("Preparing stop");
        }
        else if(mChannelStrip.state() == z_lib::ChannelStrip::State::PreparingToRecord)
        {
            DBG("Preparing rec");
        }
        else if(mChannelStrip.state() == z_lib::ChannelStrip::State::Stopped)
        {
            DBG("Stopped");
        }
        else if(mChannelStrip.state() == z_lib::ChannelStrip::State::Playing)
        {
            DBG("Playing");
        }
        else if(mChannelStrip.state() == z_lib::ChannelStrip::State::Recording)
        {
            DBG("Recording");
        }
        DBG("*******************");

    }
}


void MainComponent::mStartPlaying()
{
    if (mMetronome.isEnabled())
    {
        mMetronome.start();
        startTimer(mMetronome.getIntervalMs());
        mPlayButton.setEnabled(false);
        mStopButton.setEnabled(true);
        mArmRecordButton.setEnabled(false);
    }
}

void MainComponent::mStopPlaying()
{
//    if (mRecorder.isRecording())
//    {
//    }
    
    mStopRecording();
    stopTimer();
    mMetronome.resetStep();
    mMetronome.stop();
    mPlayButton.setEnabled(true);
    mArmRecordButton.setEnabled(true);
}



void MainComponent::mStartRecording()
{
    if (! juce::RuntimePermissions::isGranted(juce::RuntimePermissions::writeExternalStorage))
    {
        SafePointer<MainComponent> safeThis (this);
        
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
    
    mLastRecording = parentDir.getNonexistentChildFile("LoopMixerFile", ".wav");
    
    DBG("START: " << mLastRecording.getFullPathName());
    
    mRecorder.startRecording(mLastRecording);
}

void MainComponent::mRecordClip(int channelNum, int clipNum)
{
    if (! juce::RuntimePermissions::isGranted(juce::RuntimePermissions::writeExternalStorage))
    {
        SafePointer<MainComponent> safeThis (this);
        
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
    
    juce::String fName { "channel_" + juce::String(channelNum) + "_clip_" + juce::String(clipNum) };
    
    mLastRecording = parentDir.getNonexistentChildFile(fName, ".wav");
    
    DBG("SAVING: " << mLastRecording.getFullPathName());
    mRecorder.startRecording(mLastRecording);
}

void MainComponent::mStopRecording()
{
    mRecordArmed = false;
    mArmRecordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    mRecorder.stopRecording();
    mLoadRecordedClip();
    mLastRecording = juce::File();
    
    DBG("Stopped recording");
}

void MainComponent::changeState(TransportState newState)
{
	if (state != newState)
	{
		state = newState;

		switch (state)
		{
		case TransportState::STOPPED:
			stopButton.setEnabled(false);
			playButton.setEnabled(true);
			for (int chan = 0; chan < NUM_CHANNELS; chan++)
			{
				transportSource[chan].setPosition(0.0);
			}
			break;

		case TransportState::STARTING:
			playButton.setEnabled(false);
			
			for (int chan = 0; chan < NUM_CHANNELS; chan++)
			{
				transportSource[chan].start();
			}
			
			break;

		case TransportState::PLAYING:
			stopButton.setEnabled(true);
			break;

		case TransportState::STOPPING:
			for (int chan = 0; chan < NUM_CHANNELS; chan++)
			{
				transportSource[chan].stop();
			}
			break;
		}
	}
}


void MainComponent::loadBtnClicked(int chan)
{
//	FileChooser chooser("Select a Wave file to play...", {}, "*.wav");
//
//	if (chooser.browseForFileToOpen())
//	{
//		auto file = chooser.getResult();
//		auto* reader = formatManager.createReaderFor(file);
//
//		if (reader != nullptr)
//		{
//			std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
//			transportSource[chan].setSource(newSource.get(), 0, nullptr, reader->sampleRate);
//			mixer.addInputSource(&transportSource[chan], true);
//			playButton.setEnabled(true);
//			readerSource[chan].reset(newSource.release());
//		}
//	}
}

void MainComponent::mLoadRecordedClip()
{
    DBG("LOADING: " << mLastRecording.getFullPathName());
    
    auto* reader = formatManager.createReaderFor(mLastRecording);
    
    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
        transportSource[0].setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        mixer.addInputSource(&transportSource[0], true);
        readerSource[0].reset(newSource.release());
        transportSource[0].start();
    }
    
}


void MainComponent::autoLoadClip(std::string clipName, int channel)
{
	auto clipPath = basePath + "\\Clips\\" + clipName + ".wav";

	std::cout << clipPath << std::endl;

	auto file = juce::File(clipPath);
	auto* reader = formatManager.createReaderFor(file);

	if (reader != nullptr)
	{
		std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
		transportSource[channel].setSource(newSource.get(), 0, nullptr, reader->sampleRate);
		mixer.addInputSource(&transportSource[channel], true);
		playButton.setEnabled(true);
		readerSource[channel].reset(newSource.release());
	}
}

void MainComponent::playClip(int clipNum)
{

}

void MainComponent::playBtnClicked()
{
	
	if (readerSource[0].get() != nullptr)
		readerSource[0]->setLooping(true);

	if (readerSource[1].get() != nullptr)
		readerSource[1]->setLooping(true);
	
	startTimer(2400);

	changeState(TransportState::STARTING);
}

void MainComponent::recordClip()
{}

void MainComponent::stopBtnClicked()
{
	changeState(TransportState::STOPPING);
	stopTimer();
}


void MainComponent::mHandleClipButton(int channelNum, int clipNum)
{
    
    
}


void MainComponent::loadClips()
{
	for(int channel = 0; channel < NUM_CHANNELS; ++channel)
	{ 
//		strips[channel].addToMixer(&mixer);
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


void MainComponent::timerCallback()
{
    if (!mRecorder.isRecording() && mRecordArmed && !mMetronome.countingIn())
    {
        mStartRecording();
        DBG("Start recording");
    }
    
    mMetronome.advance();
    
    if (mPeriodEnded())
    {
        DBG("Period ended");
        if(mChannelStrip.state() == z_lib::ChannelStrip::State::PreparingToPlay)
        {
            mChannelStrip.play();
        }
        else if(mChannelStrip.state() == z_lib::ChannelStrip::State::PreparingToStop)
        {
            mChannelStrip.stop();
        }
        else if(mChannelStrip.state() == z_lib::ChannelStrip::State::PreparingToRecord)
        {
            mChannelStrip.record();
        }
    }
    
    
    if ((mRecordArmed && mRecorder.isRecording()))
    {
        if (mMetronome.numBars() == mNumRecBarsSlider.getValue())
        {
            mStopRecording();
            
        }
    }
//	for (auto &source : transportSource)
//	{
//		if (source.isPlaying())
//		{
//			source.stop();
//			source.setPosition(0.0);
//		}
//		source.start();
//	}
}
