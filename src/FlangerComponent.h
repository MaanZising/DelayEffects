#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "MyLookAndFeel.h"

class FlangerComponent : public juce::Component
{
public:
    FlangerComponent (AudioPluginAudioProcessor& p);
    ~FlangerComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AudioPluginAudioProcessor& processorRef;

    MyLookAndFeel lkfFlanger;

    juce::Slider delayTimeDial;
    juce::Slider lfoFreqDial;
    juce::Slider lfoDepthDial;
    juce::Slider lfoOffsetDial;
    juce::Slider feedbackDial;
    
    juce::ToggleButton feedbackToggle;

    const int fisrstColumnX { 20 };
    const int firstRowY { 40 };
    const int width { 120 };
    const int height { 100 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlangerComponent)

public:
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeDialAttach;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> lfoFreqDialAttach;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> lfoDepthDialAttach;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> lfoOffsetDialAttach;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> feedbackDialAttach;

    std::unique_ptr <juce::AudioProcessorValueTreeState::ButtonAttachment> feedbackToggleAttach;
};