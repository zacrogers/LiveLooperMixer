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

class ClipButton : public juce::TextButton,
                   public juce::ChangeBroadcaster
{
    
public:
    enum class State
    {
        Playing,
        Recording,
        Muted,
        Empty,
        PreparingToPlay,
        PreparingToRecord
    };
    
    ClipButton();
    ~ClipButton();
    
    void arm(bool armed);
    void play(void);
    void stop(void);
    void setEmpty(bool empty) { mClipEmpty = empty; }
    
    void setState(State state) { mState = state; }
    void setSelected(bool selected) { mSelected = selected; }
    bool isSelected() {return mSelected; }
    
    bool playing();
    bool recording();
    
private:
    bool mRecArmed  { false };
    bool mClipEmpty { false };
    bool mPlaying   { false };
    bool mRecording { false };
    bool mSelected  { false };
    
    State mState    { State::Empty };
    
    juce::Colour mArmedColour     { juce::Colours::darkred };
    juce::Colour mRecordingColour { juce::Colours::red };
    juce::Colour mPlayColour      { juce::Colours::darkgreen };
    juce::Colour mPlayingColour   { juce::Colours::green };
    juce::Colour mEmptyColour     { juce::Colours::slateblue };
    
    void mClicked();
    void mSetColour();
    
    
};

}
