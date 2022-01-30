#pragma once
#include <JuceHeader.h>
#include <string.h>
//#include <windows.h>

#include "myEnums.h"
//#include "AudioRecorder.h"
#include "ClipButton.h"
#include "Recorder.h"

#define NUM_CLIPS 2
#define MAX_PATH 50

namespace z_lib
{
constexpr int numClips = 4;

class ChannelStrip : public juce::AudioAppComponent,
                     public juce::ChangeBroadcaster
{
public:
    enum class State
    {
        Playing,
        Recording,
        Stopped,
        PreparingToPlay,
        PreparingToStop,
        PreparingToRecord
    };

    ChannelStrip(juce::AudioDeviceManager &deviceManager, Recorder &recorder);
    ~ChannelStrip();
    
    // Audio source methods
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    // Component methods
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    void play();
    void stop();
    void record();
    
    State state() { return mState; }
    
private:
    // Member variables
    State mState      { State::Stopped };

    bool mMuted       { false };
    bool mRecordArmed { false };

    double mVolume    { 0 };
    double mPan       { 0 };
    
    int mChannelNum   { 0 };
    
    int mLastClipSelected { 0 };
    
    juce::File mCurrentClip;
    juce::AudioDeviceManager *pDeviceManager;
    Recorder *pRecorder;
    
    
    // Gui elements
    juce::TextButton mRecordArmButton;
    juce::TextButton mStopButton;
    juce::TextButton mMuteButton;
    
    ClipButton mClipButtons[numClips];
    
    juce::Slider mVolumeSlider { juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::NoTextBox };
    juce::Slider mPanSlider;
    
    // Member functions
    void mInitGuiElements();
    
    void mChangeState();
    
    void mRecordArmButtonClicked();
    void mStopButtonClicked();
    void mMuteButtonClicked();
    
    void mStartPlaying(int clipNum);
    void mStopPlaying();
    void mStartRecording(int channelNum, int clipNum);
    void mStopRecording();
    void mLoadClip();
    
};
}

class ChannelStrip : public juce::Component
{
public:
	//==============================================================================
	ChannelStrip();
	~ChannelStrip();

	/* Component methods */
	void paint(juce::Graphics&) override;
	void resized() override;

	/* Channelstrip methods */
	void changeState(TransportState newState);
	void addToMixer(juce::MixerAudioSource* mixer);

    juce::AudioTransportSource getTransport();

	void setChannelNum(int num) { channelNum = num; };

	/* Gui elements */
    juce::TextButton stopBtn;
    juce::TextButton muteBtn;
    juce::TextButton recBtn;
    juce::TextButton soloBtn;

    juce::TextButton clipBtns[NUM_CLIPS];

    juce::Slider     volumeSlider;
    juce::Slider     filterKnob;

private:
	TransportState state;

    juce::AudioFormatManager formatManager;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource[NUM_CLIPS];
    juce::AudioTransportSource transportSource[NUM_CLIPS];

	int channelNum = 0;
	std::string basePath = "";

	int currentClip = 0;
	float gain = 0.0;

	bool isPlaying = false;
	bool isMuted   = false;
	bool isSoloed  = false;
	bool isArmed   = false;

	void initGuiElements();

	void loadClip(int slot, std::string clipName);

	void clipBtnClicked(int clip);
	void stopBtnClicked();
	void muteBtnClicked();
	void soloBtnClicked();
	void recBtnClicked();

	void recordClip();

	std::string getBasePath();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelStrip)
};
