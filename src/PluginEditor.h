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
    void getTabIndex();

private:
    AudioPluginAudioProcessor& processorRef;
    DelayComponent delayPanel { processorRef };
    FlangerComponent flangerPanel { processorRef };

    //juce::TabbedComponent tabs;
    MyTabComponent tabs;

    MyLookAndFeel lkfEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
