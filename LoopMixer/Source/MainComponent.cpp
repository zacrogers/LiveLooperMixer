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

	loadButton.setBounds(NUM_CHANNELS * 60, 50, 80, 40);
	playButton.setBounds(NUM_CHANNELS * 60, 110, 80, 40);
	stopButton.setBounds(NUM_CHANNELS * 60, 170, 80, 40);

	loadButton2.setBounds(NUM_CHANNELS * 60, 220, 80, 40);
}

void MainComponent::changeListenerCallback(ChangeBroadcaster* source)
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

void MainComponent::autoLoadClip(std::string clipName, int channel)
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

void MainComponent::loadClips()
{
	for(int channel = 0; channel < NUM_CHANNELS; ++channel)
	{
		strips[channel].addToMixer(&mixer);
	}
}

void MainComponent::timerCallback()
{
	for (auto &source : transportSource)
	{
		if (source.isPlaying())
		{
			source.stop();
			source.setPosition(0.0);
		}
		source.start();
	}
}

std::string MainComponent::getExePath()
{
	char fullPathToExe[MAX_PATH]; // Contains executable file name
	auto spath = GetModuleFileName(NULL, fullPathToExe, MAX_PATH);
	auto finalSlashIndex = ((std::string)fullPathToExe).find_last_of("\\");

	return ((std::string)fullPathToExe).substr(0, finalSlashIndex);
}