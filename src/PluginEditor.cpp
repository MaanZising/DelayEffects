#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p)//,
      //tabs (juce::TabbedButtonBar::TabsAtLeft)
{
    juce::ignoreUnused (processorRef);
    setSize (650, 300);
    setLookAndFeel (&lkfEditor);

    tabs.addTab ("Delay", lkfEditor.lightGrey, &delayPanel, false);
    tabs.addTab ("Flanger", lkfEditor.lightGrey, &flangerPanel, false);
    tabs.setCurrentTabIndex (processorRef.parameters.getRawParameterValue("tab_index")->load());
    tabs.onTabChanged = [this] (int index)
    { 
        getTabIndex(index);
    };
    tabs.setOutline (0);
    tabs.setTabBarDepth (100);
    addAndMakeVisible (tabs);

    gainSliderAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.parameters, GAIN_ID, gainSlider);
    gainSlider.setSliderStyle (juce::Slider::LinearVertical);
    gainSlider.setPopupDisplayEnabled (true, false, this);
    addAndMakeVisible (gainSlider);

    mixSliderAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.parameters, MIX_ID, mixSlider);
    mixSlider.setSliderStyle (juce::Slider::LinearVertical);
    //mixSlider.text
    addAndMakeVisible (mixSlider);
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
    g.setColour (lkfEditor.lightGrey);
    g.fillRect (100, 0, getWidth() - 100, getHeight());

    g.setColour (lkfEditor.darkGrey);
    g.setFont (lkfEditor.boldFont);

    g.drawFittedText ("Mix", 490, 15, 50, 30, juce::Justification::centred, 1);
    g.drawFittedText ("Gain", 565, 15, 50, 30, juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    tabs.setBounds (0, 0, getWidth() - 180, getHeight());
    delayPanel.setBounds (100, 0, getWidth() - 200, getHeight());
    flangerPanel.setBounds (100, 0, getWidth() - 200, getHeight());

    mixSlider.setBounds (500, 40, 30, 220);
    gainSlider.setBounds (575, 40, 30, 220);
}

void AudioPluginAudioProcessorEditor::getTabIndex(int index)
{
    processorRef.parameters.getParameter("tab_index")->setValueNotifyingHost(index);
    processorRef.clearBuffer = true;
}
