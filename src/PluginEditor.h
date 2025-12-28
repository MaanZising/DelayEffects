#pragma once

#include "PluginProcessor.h"
#include "DelayComponent.h"
#include "FlangerComponent.h"
#include "MyTabComponent.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void getTabIndex(int index);

private:
    AudioPluginAudioProcessor& processorRef;
    DelayComponent delayPanel { processorRef };
    FlangerComponent flangerPanel { processorRef };

    juce::Slider gainSlider;
    juce::Slider mixSlider;

    //juce::TabbedComponent tabs;
    MyTabComponent tabs;

    MyLookAndFeel lkfEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)

public:
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttach;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttach;
};
