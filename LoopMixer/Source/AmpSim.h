/*
  ==============================================================================

    AmpSim.h
    Created: 7 May 2023 11:12:34pm
    Author:  Zachary Rogers

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


namespace z_lib
{
class AmpSim
{
    enum
    {
        waveshaperIndex                // [2]
    };
    juce::dsp::ProcessorChain<juce::dsp::WaveShaper<float>> processorChain;
//    juce::dsp::ProcessorChain<juce::dsp::WaveShaper<float>> processorChain; // [1]
    
public:
    
};
}
