/*
  ==============================================================================

    Styles.h
    Created: 2 May 2023 9:52:44pm
    Author:  Zachary Rogers

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


namespace z_lib
{
namespace fader
{

class NeonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    NeonFaderLookAndFeel()
    {
        // Set the background color and outline
        setColour(juce::Slider::backgroundColourId, juce::Colours::black);
        setColour(juce::Slider::thumbColourId, juce::Colours::white);
        setColour(juce::Slider::trackColourId, juce::Colours::white.withAlpha(0.3f));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white);
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::black);
    }
    
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        // Draw the track
        g.setColour(findColour(juce::Slider::trackColourId));
        g.fillRect(x + width / 2 - 2, y, 4, height);
        
        // Draw the thumb
        g.setColour(findColour(juce::Slider::thumbColourId));
        g.fillRect(x, static_cast<int>(sliderPos * height) - 8, width, 16);
    }
};

} /** fader  **/


namespace button
{

} /** buttton  **/

} /** z_lib  **/
