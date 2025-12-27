#pragma once

#include "PluginProcessor.h"
#include "DelayComponent.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AudioPluginAudioProcessor& processorRef;
    DelayComponent delayPanel { processorRef };

    MyLookAndFeel lkfEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
