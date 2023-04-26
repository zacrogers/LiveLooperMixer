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
    enum class State
    {
        Playing,
        Recording,
        Muted,
        Empty,
        PreparingToPlay,
        PreparingToRecord,
        Stopped,
        PreparingToStop,
        NumStates
    };
    
    ClipButton()  {}
    ~ClipButton() {}
    
    void setState    (State state)   { mState = state;       mSetColour(); }
    void setSelected (bool selected) { mSelected = selected; mSetColour(); }
    bool isSelected  () const        { return mSelected; }

    
private:
    State mState    { State::Empty };
    bool  mSelected { false };
    
    const juce::Colour mStateColours[static_cast<int>(State::NumStates)] = {
        juce::Colours::green,     // Playing
        juce::Colours::red,       // Recording
        juce::Colours::blue,      // Muted
        juce::Colours::slateblue, // Empty
        juce::Colours::darkgreen, // Preparing to play
        juce::Colours::darkred,   // Preparing to record
        juce::Colours::blue,      // Stopped
        juce::Colours::cyan       // Preparing to stop
    };
    
    juce::Colour mGetStateColour(State state) const { return mStateColours[static_cast<int>(state)]; }
    void         mSetColour() { setColour(juce::TextButton::buttonColourId, mGetStateColour(<#State state#>)); };
};

}
