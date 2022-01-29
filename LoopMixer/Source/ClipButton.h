/*
  ==============================================================================

    ClipButton.h
    Created: 29 Jan 2022 9:20:48pm
    Author:  Zachary Rogers

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace z_lib
{

class ClipButton : public juce::TextButton
{
    
public:
    ClipButton();
    ~ClipButton();
    
    void arm(bool armed);
    void play(void);
    void stop(void);
    
    bool isArmed();
    bool isPlaying();
    bool isRecording();
    
private:
    bool mRecArmed  { false };
    bool mClipEmpty { false };
    bool mPlaying   { false };
    bool mRecording { false };
    
    juce::Colour mArmedColour     { juce::Colours::red };
    juce::Colour mPlayColour      { juce::Colours::darkgreen };
    juce::Colour mPlayingColour   { juce::Colours::green };
    juce::Colour mEmptyColour     { juce::Colours::slateblue };
    
    
};

};
