#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    setSize (600, 300);
    setLookAndFeel (&lkfEditor);

    addAndMakeVisible (delayPanel);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll (lkfEditor.darkGrey);

    g.setColour (lkfEditor.lightGrey);
    g.fillRect (100, 0, getWidth() - 100, getHeight());
}

void AudioPluginAudioProcessorEditor::resized()
{
    delayPanel.setBounds (100, 0, getWidth() - 100, getHeight());
}
