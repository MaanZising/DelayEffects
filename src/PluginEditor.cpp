#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p)//,
      //tabs (juce::TabbedButtonBar::TabsAtLeft)
{
    juce::ignoreUnused (processorRef);
    setSize (600, 300);
    setLookAndFeel (&lkfEditor);

    tabs.addTab ("Delay", lkfEditor.lightGrey, &delayPanel, false);
    tabs.addTab ("Flanger", lkfEditor.lightGrey, &flangerPanel, false);
    tabs.setCurrentTabIndex (processorRef.parameters.getRawParameterValue("tab_index")->load());
    tabs.onTabChanged = [this] (int index)
    { 
        getTabIndex();
    };
    tabs.setOutline (0);
    tabs.setTabBarDepth (100);
    addAndMakeVisible (tabs);
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
    tabs.setBounds (0, 0, getWidth(), getHeight());
    delayPanel.setBounds (100, 0, getWidth() - 100, getHeight());
    flangerPanel.setBounds (100, 0, getWidth() - 100, getHeight());
}

void AudioPluginAudioProcessorEditor::getTabIndex()
{
    processorRef.parameters.getParameter("tab_index")->setValueNotifyingHost(tabs.getCurrentTabIndex());
    processorRef.clearBuffer = true;
}
