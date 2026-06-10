#include "FlangerComponent.h"

//==============================================================================
FlangerComponent::FlangerComponent(AudioPluginAudioProcessor& p) : processorRef (p) 
{
    //setSize (600, 400);
    
    delayTimeDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.parameters, FLANGER_DELAY_TIME_ID, delayTimeDial);
    delayTimeDial.setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    delayTimeDial.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 100, 20);
    delayTimeDial.setTextBoxIsEditable (true);
    delayTimeDial.setTextValueSuffix (" secs");
    delayTimeDial.setLookAndFeel (&lkfFlanger);
    addAndMakeVisible (delayTimeDial);

    lfoFreqDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.parameters, FLANGER_LFO_FREQ_ID, lfoFreqDial);
    lfoFreqDial.setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    lfoFreqDial.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 100, 20);
    lfoFreqDial.setTextBoxIsEditable (true);
    lfoFreqDial.setTextValueSuffix (" Hz");
    lfoFreqDial.setLookAndFeel (&lkfFlanger);
    addAndMakeVisible (lfoFreqDial);

    lfoDepthDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.parameters, FLANGER_LFO_DEPTH_ID, lfoDepthDial);
    lfoDepthDial.setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    lfoDepthDial.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 100, 20);
    lfoDepthDial.setTextBoxIsEditable (true);
    lfoDepthDial.setLookAndFeel (&lkfFlanger);
    addAndMakeVisible (lfoDepthDial);

    lfoOffsetDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.parameters, FLANGER_LFO_OFFSET_ID, lfoOffsetDial);
    lfoOffsetDial.setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    lfoOffsetDial.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 100, 20);
    lfoOffsetDial.setTextBoxIsEditable (true);
    lfoOffsetDial.setTextValueSuffix (" T");
    lfoOffsetDial.setLookAndFeel (&lkfFlanger);
    addAndMakeVisible (lfoOffsetDial);
    
    feedbackDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.parameters, FLANGER_FEEDBACK_ID, feedbackDial);
    feedbackDial.setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    feedbackDial.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 100, 20);
    feedbackDial.setTextBoxIsEditable (true);
    feedbackDial.setLookAndFeel (&lkfFlanger);
    addAndMakeVisible (feedbackDial);

    feedbackToggleAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processorRef.parameters, FLANGER_FEEDBACK_TOGGLE_ID, feedbackToggle);
    feedbackToggle.setLookAndFeel (&lkfFlanger);
    addAndMakeVisible (feedbackToggle);
}

FlangerComponent::~FlangerComponent()
{
}

//==============================================================================
void FlangerComponent::paint (juce::Graphics& g)
{
    g.setColour (lkfFlanger.darkGrey);
    g.setFont (lkfFlanger.boldFont);
    g.drawFittedText ("Delay Time", fisrstColumnX, firstRowY / 2, width, firstRowY / 2, juce::Justification::centred, 1);
    g.drawFittedText ("LFO Freq", width + fisrstColumnX, firstRowY / 2, width, firstRowY / 2, juce::Justification::centred, 1);
    g.drawFittedText ("LFO Depth", width * 2 + fisrstColumnX, firstRowY / 2, width, firstRowY / 2, juce::Justification::centred, 1);
    g.drawFittedText ("LFO Offset", fisrstColumnX, height + (firstRowY * 3 / 2), width, firstRowY / 2, juce::Justification::centred, 1);
    g.drawFittedText ("Feedback", width + fisrstColumnX, height + (firstRowY * 3 / 2), width, firstRowY / 2, juce::Justification::centred, 1);
}

void FlangerComponent::resized()
{
    delayTimeDial.setBounds (fisrstColumnX, firstRowY, width, height);
    lfoFreqDial.setBounds (width + fisrstColumnX, firstRowY, width, height);
    lfoDepthDial.setBounds (width * 2 + fisrstColumnX, firstRowY, width, height);
    lfoOffsetDial.setBounds (fisrstColumnX, height + (firstRowY * 2), width, height);
    feedbackDial.setBounds (width + fisrstColumnX, height + (firstRowY * 2), width, height);

    feedbackToggle.setBounds (width + fisrstColumnX, height + (firstRowY * 2), 25, 20);
}