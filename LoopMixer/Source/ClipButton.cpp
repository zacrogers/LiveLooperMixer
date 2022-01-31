/*
  ==============================================================================

    ClipButton.cpp
    Created: 29 Jan 2022 9:20:48pm
    Author:  Zachary Rogers

  ==============================================================================
*/

#include "ClipButton.h"


namespace z_lib
{

ClipButton::ClipButton()
{
    this->onClick = [this] () { mClicked(); };
    setColour(juce::TextButton::buttonColourId, mEmptyColour);
}

ClipButton::~ClipButton()
{
    
}


void ClipButton::mClicked()
{
    sendChangeMessage();
    mSetColour();
}

void ClipButton::mSetColour()
{
    switch (mState)
    {
        case State::Playing:
            setColour(juce::TextButton::buttonColourId, mPlayingColour);
            break;
            
        case State::Recording:
            setColour(juce::TextButton::buttonColourId, mRecordingColour);
            break;
            
        case State::Muted:
            setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
            break;
            
        case State::Empty:
            setColour(juce::TextButton::buttonColourId, mEmptyColour);
            break;
            
        case State::PreparingToPlay:
            setColour(juce::TextButton::buttonColourId, mPlayColour);
            break;
            
        case State::PreparingToRecord:
            setColour(juce::TextButton::buttonColourId, mArmedColour);
            break;
            
        default:
            break;
    }
    
}


}
