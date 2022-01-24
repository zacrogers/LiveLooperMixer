/*
  ==============================================================================

    Recorder.cpp
    Created: 7 Jan 2022 2:27:31pm
    Author:  Zachary Rogers

  ==============================================================================
*/

#include "Recorder.h"


Recorder::Recorder()
{
    backgroundThread.startThread();
}


Recorder::~Recorder()
{
    stopRecording();
}


void Recorder::audioDeviceAboutToStart (juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
}


void Recorder::audioDeviceStopped()
{
    sampleRate = 0;
}
 

void Recorder::audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                            float** outputChannelData, int numOutputChannels,
                            int numSamples)
{
    const juce::ScopedLock sl (writerLock);

    if (activeWriter.load() != nullptr)// && numInputChannels >= thumbnail.getNumChannels())
    {
        activeWriter.load()->write (inputChannelData, numSamples);

        // Create an AudioBuffer to wrap our incoming data, note that this does no allocations or copies, it simply references our input data
        juce::AudioBuffer<float> buffer (const_cast<float**> (inputChannelData), 1, numSamples);

//        thumbnail.addBlock (nextSampleNum, buffer, 0, numSamples);
        nextSampleNum += numSamples;
    }

    // We need to clear the output buffers, in case they're full of junk..
    for (int i = 0; i < numOutputChannels; ++i)
        if (outputChannelData[i] != nullptr)
            juce::FloatVectorOperations::clear (outputChannelData[i], numSamples);
}


void Recorder::startRecording(const juce::File& file)
{
    stopRecording();
    
    if (sampleRate > 0)
    {
        // Create an OutputStream to write to our destination file...
        file.deleteFile();

        if (auto fileStream = std::unique_ptr<juce::FileOutputStream> (file.createOutputStream()))
        {
            // Now create a WAV writer object that writes to our output stream...
            juce::WavAudioFormat wavFormat;

            if (auto writer = wavFormat.createWriterFor (fileStream.get(), sampleRate, 1, 16, {}, 0))
            {
                fileStream.release(); // (passes responsibility for deleting the stream to the writer object that is now using it)

                // Now we'll create one of these helper objects which will act as a FIFO buffer, and will
                // write the data to disk on our background thread.
                threadedWriter.reset (new juce::AudioFormatWriter::ThreadedWriter (writer, backgroundThread, 32768));

                // Reset our recording thumbnail
//                thumbnail.reset (writer->getNumChannels(), writer->getSampleRate());
                nextSampleNum = 0;

                // And now, swap over our active writer pointer so that the audio callback will start using it..
                const juce::ScopedLock sl (writerLock);
                activeWriter = threadedWriter.get();
            }
        }
    }
}


void Recorder::stopRecording()
{
    {
        const juce::ScopedLock s1 (writerLock);
        activeWriter = nullptr;
    }
    threadedWriter.reset();
}


bool Recorder::isRecording() const
{
    return activeWriter.load() != nullptr;
}

