#pragma once

#include <JuceHeader.h>

class AudioRecorder : public AudioIODeviceCallback
{
public:
	AudioRecorder();
	~AudioRecorder() override;

	void startRecording(const File& file);
	void stop();
	bool isRecording() const
	{
		return activeWriter.load() != nullptr;
	}
	
	void audioDeviceAboutToStart(AudioIODevice* device) override;
	void audioDeviceStopped() override;
	void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
		float** outputChannelData, int numOutputChannels, int numSamples) override;

private:
	TimeSliceThread backgroundThread{ "Audio Recorder Thread" }; // the thread that will write our audio data to disk
	std::unique_ptr<AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
	double sampleRate = 0.0;
	int64 nextSampleNum = 0;

	CriticalSection writerLock;
	std::atomic<AudioFormatWriter::ThreadedWriter*> activeWriter{ nullptr };
};