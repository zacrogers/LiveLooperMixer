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
        Stopped,
        Muted,
        Empty,
        PreparingToPlay,
        PreparingToRecord,
        PreparingToStop,
        NumStates
    };
    
    ClipButton()  {}
    ~ClipButton() {}
    
    void setState    (State state)   { mState = state;       _setColour(); }
    void setSelected (bool selected) { mSelected = selected; _setColour(); }
    bool isSelected  () const        { return mSelected; }

    
private:
    State mState    { State::Empty };
    bool  mSelected { false };
    
    const juce::Colour mStateColours[static_cast<int>(State::NumStates)] = {
        juce::Colours::green,     // Playing
        juce::Colours::red,       // Recording
        juce::Colours::darkblue,  // Stopped
        juce::Colours::blue,      // Muted
        juce::Colours::slateblue, // Empty
        juce::Colours::darkgreen, // Preparing to play
        juce::Colours::darkred,   // Preparing to record
        juce::Colours::cyan       // Preparing to stop
    };
    
    juce::Colour getStateColour(State state) const { return mStateColours[static_cast<int>(state)]; }
    void         _setColour() { setColour(juce::TextButton::buttonColourId, getStateColour(mState)); };
};

}
