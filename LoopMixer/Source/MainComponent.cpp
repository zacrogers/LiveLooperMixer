/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{ 
	initGuiElements();

	formatManager.registerBasicFormats();
	transportSource[0].addChangeListener(this);
	transportSource[1].addChangeListener(this);

	basePath = getExePath();
	autoLoadClip("lmdrum", 0);
	autoLoadClip("lmbass", 1);

	mixer.addInputSource(&transportSource[0], true);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
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
	for (std::size_t i = 0; i < NUM_CHANNELS; i++)
	{
		trackStates.push_back(State::Track::STOPPED);
	}

	strips[0].clipBtns[0].onClick = [this] { playBtnClicked(); };
	strips[0].recBtn.onClick = [this] { recordClip(); };

	for (int chan = 0; chan < NUM_CHANNELS; ++chan)
	{
		//addAndMakeVisible(&strips[chan]);
		//strips[chan].setChannelNum(chan);

		auto slider = &volumeSliders[chan];

		addAndMakeVisible(&volumeSliders[chan]);
		volumeSliders[chan].setRange(0, 1);
		volumeSliders[chan].setSliderStyle(Slider::SliderStyle::LinearVertical);
		volumeSliders[chan].onValueChange = [this, chan]{ volumeChanged(chan); };

	}

	addAndMakeVisible(&loadButton);
	loadButton.setButtonText("Load");
	loadButton.onClick = [this] { loadBtnClicked(0); };

	addAndMakeVisible(&playButton);
	playButton.setButtonText("Play");
	playButton.setColour(TextButton::buttonColourId, Colours::green);
	playButton.onClick = [this] { playBtnClicked(); };

	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("Stop");
	stopButton.setColour(TextButton::buttonColourId, Colours::red);
	stopButton.onClick = [this] { stopBtnClicked(); };

	addAndMakeVisible(&play1);
	play1.setButtonText("Play1");
	play1.onClick = [this] { playClip(0); };

	addAndMakeVisible(&play2);
	play2.setButtonText("Play2");
	play2.onClick = [this] { playClip(1); };

	addAndMakeVisible(&stop1);
	stop1.setButtonText("Stop1");
	stop1.onClick = [this] { stopClip(0); };

	addAndMakeVisible(&stop2);
	stop2.setButtonText("Stop2");
	stop2.onClick = [this] { stopClip(1); };

	setSize(600, 600);
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
	mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
	if (readerSource[0].get() == nullptr)
	{
		bufferToFill.clearActiveBufferRegion();
		return;
	}

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
void MainComponent::paint (Graphics& g)
{
	g.fillAll(Colours::darkblue);
}

void MainComponent::resized()
{
	for (int chan = 0; chan < NUM_CHANNELS; ++chan)
	{
		volumeSliders[chan].setBounds(0 + (chan * 60), 0, 40, getHeight());
		//strips[chan].setBounds(0 + (chan * 60), 0, 40, getHeight());
	}

	std::uint16_t btnHeight = 40;

	loadButton.setBounds(NUM_CHANNELS * 60, btnHeight + 10, 80, btnHeight);
	playButton.setBounds(NUM_CHANNELS * 60, (btnHeight * 2) + 10, 80, btnHeight);
	stopButton.setBounds(NUM_CHANNELS * 60, (btnHeight * 3) + 10, 80, btnHeight);
	play1.setBounds(NUM_CHANNELS * 60, (btnHeight * 4) + 10, 80, btnHeight);
	play2.setBounds(NUM_CHANNELS * 60, (btnHeight * 5) + 10, 80, btnHeight);
	stop1.setBounds(NUM_CHANNELS * 60, (btnHeight * 6) + 10, 80, btnHeight);
	stop2.setBounds(NUM_CHANNELS * 60, (btnHeight * 7) + 10, 80, btnHeight);

	loadButton2.setBounds(NUM_CHANNELS * 60, (btnHeight * 8) + 10, 80, 40);
}

void MainComponent::changeListenerCallback(ChangeBroadcaster* source)
{

	if (source == &transportSource[0])
	{
		if (transportSource[0].isPlaying())
		{
			changeState(State::Transport::PLAYING);
		}
		else
		{
			changeState(State::Transport::STOPPED);
		}
	}

	if (source == &transportSource[1])
	{
		if (transportSource[1].isPlaying())
		{
			changeState(State::Transport::PLAYING);
		}
		else
		{
			changeState(State::Transport::STOPPED);
		}
	}
}

void MainComponent::changeState(State::Transport newState)
{
	if (state != newState)
	{
		state = newState;

		switch (state)
		{
		case State::Transport::STOPPED:
			stopButton.setEnabled(false);
			playButton.setEnabled(true);
			for (int chan = 0; chan < NUM_CHANNELS; chan++)
			{
				transportSource[chan].setPosition(0.0);
			}
			break;

		case State::Transport::STARTING:
			playButton.setEnabled(false);
			
			for (int chan = 0; chan < NUM_CHANNELS; chan++)
			{
				transportSource[chan].start();
			}
			
			break;

		case State::Transport::PLAYING:
			stopButton.setEnabled(true);
			break;

		case State::Transport::STOPPING:
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
	FileChooser chooser("Select a Wave file to play...", {}, "*.wav");

	if (chooser.browseForFileToOpen())
	{
		auto file = chooser.getResult();
		auto* reader = formatManager.createReaderFor(file);

		if (reader != nullptr)
		{
			std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
			transportSource[chan].setSource(newSource.get(), 0, nullptr, reader->sampleRate);
			mixer.addInputSource(&transportSource[chan], true);
			playButton.setEnabled(true);
			readerSource[chan].reset(newSource.release());
		}
	}
}

void MainComponent::autoLoadClip(std::string clipName, std::uint8_t channel)
{
	auto clipPath = basePath + "\\Clips\\" + clipName + ".wav";

	std::cout << clipPath << std::endl;

	auto file = File(clipPath);
	auto* reader = formatManager.createReaderFor(file);

	if (reader != nullptr)
	{
		std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
		transportSource[channel].setSource(newSource.get(), 0, nullptr, reader->sampleRate);
		mixer.addInputSource(&transportSource[channel], true);
		playButton.setEnabled(true);
		readerSource[channel].reset(newSource.release());
	}
}

void MainComponent::playClip(std::uint8_t clipNum)
{

	if(trackStates.at(clipNum) == State::Track::STOPPED 
		|| trackStates.at(clipNum) == State::Track::PLAYING)
	{
		trackStates.at(clipNum) = State::Track::QUEUED;
	}

	trackEnabled[clipNum] = true;

	//stopTimer();
	if (readerSource[clipNum].get() != nullptr)// && !transportSource[clipNum].isPlaying())
	{
		readerSource[clipNum]->setLooping(true);
		transportSource[clipNum].setPosition(0.0);
		//transportSource[clipNum].start();
	}

	if (!isTimerRunning())
	{
		startTimer(2400);
	}

	/*
	for (std::uint8_t i = 0; i < 2; i++)
	{
		if (i != clipNum)
		{
			transportSource[i].stop();
			transportSource[i].setPosition(0.0);
		}
	}*/

	//changeState(TransportState::STARTING);
}

void MainComponent::stopClip(std::uint8_t clipNum)
{
	trackEnabled[clipNum] = false;
	trackStates.at(clipNum) = State::Track::STOPPED;

	if (readerSource[clipNum].get() != nullptr && transportSource[clipNum].isPlaying())
	{
		//readerSource[clipNum]->setLooping(true);
		transportSource[clipNum].stop();
		transportSource[clipNum].setPosition(0.0);
	}
}

void MainComponent::playBtnClicked()
{
	stopTimer();

	for (std::size_t track = 0; track < NUM_CHANNELS; track++)
	{
		if (readerSource[track].get() != nullptr)
		{
			if (trackStates.at(track) == State::Track::PLAYING
				|| trackStates.at(track) == State::Track::QUEUED)
			{
				readerSource[track]->setLooping(true);
			}
		}

	}
	
	//if (readerSource[0].get() != nullptr)
	//	readerSource[0]->setLooping(true);

	//if (readerSource[1].get() != nullptr)
	//	readerSource[1]->setLooping(true);
	//
	startTimer(2400);

	changeState(State::Transport::STARTING);
}

void MainComponent::recordClip()
{}

void MainComponent::stopBtnClicked()
{
	changeState(State::Transport::STOPPING);
	stopTimer();
}

void MainComponent::loadClips()
{
	for(int channel = 0; channel < NUM_CHANNELS; ++channel)
	{
		strips[channel].addToMixer(&mixer);
	}
}

void MainComponent::timerCallback()
{
	//for (auto &source : transportSource)
	for(std::size_t i = 0; i < 2; i++)
	{
		if (//transportSource[i].isPlaying() && 
			trackEnabled[i] && 
			trackStates[i] == State::Track::QUEUED)
		{
			transportSource[i].stop();
			transportSource[i].setPosition(0.0);
			transportSource[i].start();
		}
	}
}

const std::string MainComponent::getExePath()
{
	char fullPathToExe[MAX_PATH]; // Contains executable file name
	auto spath = GetModuleFileName(NULL, fullPathToExe, MAX_PATH);
	auto finalSlashIndex = ((std::string)fullPathToExe).find_last_of("\\");

	return ((std::string)fullPathToExe).substr(0, finalSlashIndex);
}