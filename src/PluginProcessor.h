#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

#define LFO_FREQ_ID "lfo_freq"
#define LFO_FREQ_NAME "LFO Frequency"

#define LFO_OFFSET_ID "lfo_offset"
#define LFO_OFFSET_NAME "LFO Offset"

#define DELAY_TIME_ID "delay_time"
#define DELAY_TIME_NAME "Delay Time"

#define LFO_DEPTH_ID "lfo_depth"
#define LFO_DEPTH_NAME "LFO Depth"

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;

private:
    int writePosition { 0 };
    void updateWritePosition (juce::AudioBuffer<float>& buffer);

    juce::AudioBuffer<float> delayBuffer;
    //double delayTime { 0.02 };
    void fillDelayBuffer (int channel, juce::AudioBuffer<float>& buffer);
    void readFromDelayBufferByBlock (int channel, juce::AudioBuffer<float>& buffer);
    void readFromDelayBufferByFrame (int channel, juce::AudioBuffer<float>& buffer);

    //double frequency {0.4};
    double currentSampleRate { 0.0 };
    std::vector<double> currentAngle;
    double angleDelta { 0.0 };
    void updateAngleDelta();
    float getSineWaveData (int channel);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
