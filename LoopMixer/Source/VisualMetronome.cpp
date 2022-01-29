/*
  ==============================================================================

    VisualMetronome.cpp
    Created: 11 Jan 2022 8:44:45pm
    Author:  Zachary Rogers

  ==============================================================================
*/

#include "VisualMetronome.h"


VisualMetronome::VisualMetronome(bool externallyTriggered, Orientation orientation)
{
    mExternalTrigger = externallyTriggered;
    
    setSize(200, 200);
    
    /** Buttons */
    addAndMakeVisible(&mEnabledButton);
    mEnabledButton.setColour(juce::TextButton::buttonColourId, mNotEnabledColour);
    mEnabledButton.onClick = [this]() { mToggleEnabled(); };
    
    addAndMakeVisible(&mCountInButton);
    mCountInButton.setColour(juce::TextButton::buttonColourId, mNotEnabledColour);
    mCountInButton.onClick = [this]() { mToggleCountIn(); };
    
    /** Sliders */
    addAndMakeVisible(&mBpmSlider);
    mBpmSlider.setRange(mBpmMin, mBpmMax);
    mBpmSlider.setValue(120.0);
    mBpmSlider.onValueChange = [this]()
    {
        mBpm = mBpmSlider.getValue();
    };
    
    addAndMakeVisible(&mCountInSlider);
    mCountInSlider.setRange(1, 8, 1);
    mCountInSlider.setValue(4);
    mCountInSlider.onValueChange = [this]()
    {
        mNcountInBeats = mCountInSlider.getValue();
    };
    
    /** Labels */
    if (mExternalTrigger)
    {
        addAndMakeVisible(&mExternalTriggerLabel);
        mExternalTriggerLabel.setColour(juce::Label::backgroundColourId, mEnabledColour);
        mExternalTriggerLabel.setJustificationType(juce::Justification::centred);
        mExternalTriggerLabel.setText((mExternalTrigger) ? "E" : "I", juce::NotificationType::dontSendNotification);
    }
    
    addAndMakeVisible(mNumBarsLabel);
    mNumBarsLabel.setText(juce::String(mNumBars), juce::NotificationType::dontSendNotification);
}


VisualMetronome::~VisualMetronome()
{
    
}


void VisualMetronome::start()
{
    if (mEnabled)
    {
        resetStep();
        mBpmSlider.setEnabled(false);
        mCountInButton.setEnabled(false);
        mEnabledButton.setEnabled(false);

    }
    repaint();
}

void VisualMetronome::stop()
{
    resetStep();
    mBpmSlider.setEnabled(true);
    mCountInButton.setEnabled(true);
    mEnabledButton.setEnabled(true);
    if (mCountInEnabled)
    {
        mCountingIn = true;
    }
    
    repaint();
}


void VisualMetronome::resetStep()
{
    mCurrentBeat = mStartBeat;
    mNumBars = 0;
    mNumBarsLabel.setText(juce::String(mNumBars), juce::NotificationType::dontSendNotification);
}


void VisualMetronome::advance()
{
    repaint();
    
    mPreviousBeat = mCurrentBeat;
    
    if (mCountingIn)
    {
        if (mCurrentBeat < mNcountInBeats)
        {
            mCurrentBeat++;
        }
        else
        {
            mCountingIn = false;
            mCurrentBeat = mStartBeat;
        }
    }
    else
    {
        if (mCurrentBeat < mFinalBeat)
        {
            mCurrentBeat++;
        }
        else
        {
            mCurrentBeat = mStartBeat;
            mNumBars++;
            mNumBarsLabel.setText(juce::String(mNumBars), juce::NotificationType::dontSendNotification);
        }
    }
    
}
 

void VisualMetronome::toggleExternalTrigger()
{
    mExternalTrigger = !mExternalTrigger;
    
    if (mExternalTrigger)
    {
        mBpmSlider.setEnabled(false);
        mExternalTriggerLabel.setText("E", juce::NotificationType::dontSendNotification);
    }
    else
    {
        mBpmSlider.setEnabled(true);
        mExternalTriggerLabel.setText("I", juce::NotificationType::dontSendNotification);
    }
}


void VisualMetronome::paint(juce::Graphics& g)
{
    g.fillAll (mBgColour);
    
    int buffer = 5;
    juce::Point<int> pos { 0, buffer };
    juce::Rectangle<int> rect { mIndicatorSize, mIndicatorSize };

    for (int beat = mStartBeat; beat < (mFinalBeat + 1); ++beat)
    {
        // Set the colour to draw
        if (mEnabled)
        {
            if (beat == mCurrentBeat)
            {
                if (mCountingIn)
                {
                    g.setColour(mCountInColour);
                }
                else
                {
                    g.setColour(mBeatOnColour);
                }
            }
            else if (mCountingIn == mNcountInBeats)
            {
                mCountingIn = false;
            }
            else
            {
                g.setColour(mBeatOffColour);
            }
        }
        else
        {
            g.setColour(juce::Colours::darkslateblue);
        }
        
        // Draw indicator
        pos.setX(mIndicatorSize * (beat - 1) + mIndicatorBufferSize + buffer);
        rect.setPosition(pos);
        g.fillRect(rect);
        
        // Draw indicator border
        g.setColour(juce::Colours::black);
        g.drawRect(rect);
    }
    
    // Draw metronome border
    g.setColour(juce::Colours::darkslategrey);
    g.drawRect(getLocalBounds());
}


void VisualMetronome::resized()
{
    auto area = getLocalBounds();
    int buttonSize { 50 };
    int pad { 15 };
    
    mNumBarsLabel.setBounds((mIndicatorSize * 8) + (mIndicatorSize / 2), 5, mIndicatorSize, mIndicatorSize);
    
    mEnabledButton.setBounds(pad, (mIndicatorSize + pad), buttonSize, buttonSize);
    mCountInButton.setBounds(pad, (buttonSize + mIndicatorSize + (pad * 2)), buttonSize, buttonSize);
//    (pad, (buttonSize + mIndicatorSize + (pad * 2)), buttonSize, buttonSize)
//    (pad, ((buttonSize * 2) + mIndicatorSize + (pad * 3)), buttonSize, buttonSize);
    mCountInSlider.setBounds(pad, ((buttonSize * 2) + mIndicatorSize + (pad * 3)), 250, buttonSize);
    
    mBpmSlider.setBounds(((area.getWidth() / 2) - pad), (mIndicatorSize + (pad * 2)), 100, 100);
}


void VisualMetronome::timerCallback()
{
    advance();
}


void VisualMetronome::mToggleEnabled()
{
    mEnabled = !mEnabled;
    
    if (mCountInEnabled && !mCountingIn)
    {
        mCountingIn = true;
    }
    
    if (mEnabled)
    {
        mEnabledButton.setColour(juce::TextButton::buttonColourId, mEnabledColour);
    }
    else
    {
        resetStep();
        mEnabledButton.setColour(juce::TextButton::buttonColourId, mNotEnabledColour);
    }
    repaint();
}


void VisualMetronome::mToggleCountIn()
{
    mCountInEnabled = !mCountInEnabled;
    mCountingIn = !mCountingIn;
    
    if (mCountInEnabled)
    {
        mCountInButton.setColour(juce::TextButton::buttonColourId, mEnabledColour);
    }
    else
    {
        mCountInButton.setColour(juce::TextButton::buttonColourId, mNotEnabledColour);
    }
    repaint();
}

