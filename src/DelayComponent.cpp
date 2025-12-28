#include "DelayComponent.h"

//==============================================================================
DelayComponent::DelayComponent(AudioPluginAudioProcessor& p) : processorRef (p) 
{
    //setSize (600, 400);
    
    delayTimeDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.parameters, DELAY_DELAY_TIME_ID, delayTimeDial);
    delayTimeDial.setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    delayTimeDial.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 100, 20);
    delayTimeDial.setTextBoxIsEditable (true);
    delayTimeDial.setTextValueSuffix (" secs");
    delayTimeDial.setLookAndFeel (&lkfDelay);
    addAndMakeVisible (delayTimeDial);

    feedbackDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.parameters, DELAY_FEEDBACK_ID, feedbackDial);
    feedbackDial.setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    feedbackDial.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 100, 20);
    feedbackDial.setTextBoxIsEditable (true);
    feedbackDial.setLookAndFeel (&lkfDelay);
    addAndMakeVisible (feedbackDial);
}

DelayComponent::~DelayComponent()
{
}

//==============================================================================
void DelayComponent::paint (juce::Graphics& g)
{
    g.setColour (lkfDelay.darkGrey);
    g.setFont (juce::Font ("sans-serif", 16.0f, juce::Font::bold));
    g.drawFittedText ("Delay Time", fisrstColumnX, firstRowY / 2, width, firstRowY / 2, juce::Justification::centred, 1);
    g.drawFittedText ("Feedback", fisrstColumnX + width, firstRowY / 2, width, firstRowY / 2, juce::Justification::centred, 1);
}

void DelayComponent::resized()
{
    delayTimeDial.setBounds (fisrstColumnX, firstRowY, width, height);
    feedbackDial.setBounds (fisrstColumnX + width, firstRowY, width, height);
}
