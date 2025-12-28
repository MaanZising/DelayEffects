#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class MyLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // Pre-defined colours
    juce::Colour lightGrey {juce::Colour (195, 195, 195)};
    juce::Colour midGrey {juce::Colour (120, 120, 120)};
    juce::Colour darkGrey {juce::Colour (60, 60, 60)};
    juce::Colour orange {juce::Colour (225, 145, 10)};
    juce::Colour blue {juce::Colour (80, 125, 185)};

    MyLookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, orange);
        setColour (juce::Slider::rotarySliderFillColourId, blue);
        setColour (juce::Slider::rotarySliderOutlineColourId, darkGrey);

        setColour (juce::Slider::textBoxBackgroundColourId, lightGrey);
        setColour (juce::Slider::textBoxOutlineColourId, lightGrey);
        setColour (juce::Slider::textBoxTextColourId, darkGrey);
        setColour (juce::Slider::textBoxHighlightColourId, darkGrey);

        setColour (juce::TextEditor::highlightedTextColourId, lightGrey);
        setColour (juce::TextEditor::focusedOutlineColourId, lightGrey);

        setColour (juce::Label::textColourId, darkGrey);
        setColour (juce::Label::textWhenEditingColourId, darkGrey);
    }

    juce::Font getLabelFont(juce::Label&) override
    {
        return juce::Font ("sans-serif", 14.0f, juce::Font::bold);
    }

    void drawTabButton (juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown) override
    {
        juce::ignoreUnused (isMouseDown);
        g.setFont (juce::Font ("sans-serif", 20.0f, juce::Font::bold));

        if (button.isFrontTab())
        {
            g.fillAll (orange);
            g.setColour (darkGrey);
            g.drawFittedText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
        }
        else if (isMouseOver)
        {
            g.fillAll (darkGrey);
            g.setColour (orange);
            g.drawFittedText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
        }
        else
        {
            g.fillAll (darkGrey);
            g.setColour (lightGrey);
            g.drawFittedText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
        }
    }
};