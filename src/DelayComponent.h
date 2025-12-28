#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "MyLookAndFeel.h"

class DelayComponent : public juce::Component
{
public:
    DelayComponent (AudioPluginAudioProcessor& p);
    ~DelayComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AudioPluginAudioProcessor& processorRef;

    MyLookAndFeel lkfDelay;

    juce::Slider delayTimeDial;
    juce::Slider feedbackDial;

    const int fisrstColumnX { 20 };
    const int firstRowY { 40 };
    const int width { 120 };
    const int height { 100 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayComponent)

public:
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeDialAttach;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> feedbackDialAttach;
};