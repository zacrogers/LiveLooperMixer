#include "ChannelStrip.h"


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
	volumeSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
	getLookAndFeel().setColour(Slider::thumbColourId, Colours::aquamarine);

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
		clipBtns[btn].setColour(TextButton::buttonColourId, Colours::green);
		clipBtns[btn].onClick = [this, btn] { clipBtnClicked(btn); };
	}
}

void ChannelStrip::paint(Graphics&)
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

	auto file = File(clipPath);
	auto* reader = formatManager.createReaderFor(file);

	if (reader != nullptr)
	{
		std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
		transportSource[0].setSource(newSource.get(), 0, nullptr, reader->sampleRate);
		/*
		mixer.addInputSource(&transportSource[0], true);
		playButton.setEnabled(true);
		readerSource[slot].reset(newSource.release());
		*/
	}
}


void ChannelStrip::addToMixer(MixerAudioSource* mixer)
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
