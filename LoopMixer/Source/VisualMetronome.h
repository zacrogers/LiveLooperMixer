/*
  ==============================================================================

    VisualMetronome.h
    Created: 11 Jan 2022 8:44:45pm
    Author:  Zachary Rogers

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class VisualMetronome : public juce::Component, public juce::Timer
{
public:
    enum class Orientation
    {
        Vertical,
        Horizontal
    };
    
    const double bpmMax   { 200.0 };
    const double bpmMin { 50.0 };
    
    
    VisualMetronome(bool externallyTriggered = false, Orientation orientation = Orientation::Horizontal);
    ~VisualMetronome() override;
    
    void start();
    void stop();
    void resetStep();
    void advance();
    void toggleExternalTrigger();
    void setExternallyTriggered(bool externallyTriggered) { mExternalTrigger = externallyTriggered; }
    bool externallyTriggered() const { return mExternalTrigger; }
    int  currentBeat() const { return mCurrentBeat; }
    bool countingIn() const { return mCountingIn; }
    
    bool isEnabled() const { return mEnabled; };
    void setIsPlaying(bool isPlaying) { mIsPlaying = isPlaying; }
    
    int numBars() const { return mNumBars; }
    
    
    double getIntervalMs() { return mBpmToMs(mBpm); }
    
    /* Component functions */
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /* Timer functions */
    void timerCallback () override;
    
private:
    /* Variables */
    bool mEnabled             { false };
    bool mExternalTrigger     { false };
    bool mCountInEnabled      { false };
    bool mCountingIn          { false };
    bool mIsPlaying           { false };
    
    double mBpm               { 60.0 };
    double mBpmMax            { 200.0 };
    double mBpmMin            { 50.0 };
    
    int mStartBeat            { 1 };
    int mCurrentBeat          { mStartBeat };
    int mFinalBeat            { 8 };
    int mNcountInBeats        { 4 };
    int mNumBars              { 0 };
    
    int mIndicatorSize        { 30 };
    int mIndicatorBufferSize  { 2 };

    juce::Array<int> mOnBeats { 1, 3, 5, 7 };
    
    /* Colours */
    juce::Colour mBgColour         { juce::Colours::darkblue };
    
    juce::Colour mBeatOnColour     { juce::Colours::red };
    juce::Colour mBeatOffColour    { juce::Colours::green };
    juce::Colour mCountInColour    { juce::Colours::yellow };
    
    juce::Colour mEnabledColour    { juce::Colours::green };
    juce::Colour mNotEnabledColour { juce::Colours::red };
    
    /* Gui elements */
    juce::TextButton mEnabledButton { "E" };
    juce::TextButton mCountInButton { "C" };
    juce::Slider mBpmSlider         { juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxAbove };
    juce::Label mExternalTriggerLabel;
    juce::Label mBpmSliderLabel { "mBpmSliderLabel", "  BPM" };
    
    juce::Slider mCountInSlider;
    
    juce::Label mNumBarsLabel;
    
    /* Functions */
    void mToggleEnabled();
    void mToggleCountIn();
    int mBpmToMs(float bpm) { return 60000 / bpm; };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualMetronome)
};
