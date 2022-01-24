/*
  ==============================================================================

    Recorder.h
    Created: 7 Jan 2022 2:11:36pm
    Author:  Zachary Rogers

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Recorder : public juce::AudioIODeviceCallback {
    
public:
    Recorder();
    ~Recorder() override;
    
    void audioDeviceAboutToStart (juce::AudioIODevice* device) override;

    void audioDeviceStopped() override;

    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels,
                                int numSamples) override;
    
    void startRecording(const juce::File& file);
    void stopRecording();
    bool isRecording() const;
    
    void toggleMuted() { mMuted = !mMuted; }
    void setMuted(bool muted) { mMuted = muted; }
    bool isMuted() { return mMuted; }
    
private:
    juce::TimeSliceThread backgroundThread {"Audio Recorder Thread"};
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter;
    
    double sampleRate = 0.0;
    juce::int64 nextSampleNum = 0;
    
    juce::CriticalSection writerLock;
    std::atomic<juce::AudioFormatWriter::ThreadedWriter*> activeWriter {nullptr};
    
    bool mMuted = true;
    
//    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Recorder)
};
