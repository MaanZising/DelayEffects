#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#define FLANGER_LFO_FREQ_ID "flanger_lfo_freq"
#define FLANGER_LFO_FREQ_NAME "Flanger LFO Frequency"

#define FLANGER_LFO_OFFSET_ID "flanger_lfo_offset"
#define FLANGER_LFO_OFFSET_NAME "Flanger LFO Offset"

#define FLANGER_DELAY_TIME_ID "flanger_delay_time"
#define FLANGER_DELAY_TIME_NAME "Flanger Delay Time"

#define FLANGER_LFO_DEPTH_ID "flanger_lfo_depth"
#define FLANGER_LFO_DEPTH_NAME "Flanger LFO Depth"

class Flanger
{
public:
    void init (double& sampleRateRef, int& writePositionRef, juce::AudioProcessorValueTreeState& parametersRef, juce::AudioBuffer<float>& delayBufferRef);
    void readFromDelayBuffer (int channel, juce::AudioBuffer<float>& buffer);
    void updateAngleDelta();
    float getSineWaveData (int channel);
    std::vector<double> currentAngle;
private:
    double sampleRate;
    double angleDelta { 0.0 };
    int* writePosition;
    juce::AudioBuffer<float>* delayBuffer;
    juce::AudioProcessorValueTreeState* parameters;
};