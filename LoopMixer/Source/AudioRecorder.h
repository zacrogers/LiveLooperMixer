#pragma once

#include <JuceHeader.h>


class AudioRecorder : public juce::AudioIODeviceCallback
{
public:
	AudioRecorder();
	~AudioRecorder() override;

	void startRecording(const juce::File& file);
	void stop();
	bool isRecording() const
	{
		return activeWriter.load() != nullptr;
	}
	
	void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
	void audioDeviceStopped() override;
	void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
		float** outputChannelData, int numOutputChannels, int numSamples) override;

private:
    juce::TimeSliceThread backgroundThread{ "Audio Recorder Thread" }; // the thread that will write our audio data to disk
	std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
	double sampleRate = 0.0;
    juce::int64 nextSampleNum = 0;

    juce::CriticalSection writerLock;
	std::atomic<juce::AudioFormatWriter::ThreadedWriter*> activeWriter{ nullptr };
};
