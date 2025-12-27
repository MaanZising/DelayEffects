#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#define DELAY_DELAY_TIME_ID "delay_delay_time"
#define DELAY_DELAY_TIME_NAME "Delay Delay Time"

#define DELAY_FEEDBACK_ID "delay_feedback"
#define DELAY_FEEDBACK_NAME "Delay Feedback"

class Delay
{
public:
    void init (double& sampleRateRef, int& writePositionRef, juce::AudioProcessorValueTreeState& parametersRef, juce::AudioBuffer<float>& delayBufferRef);
    void readFromDelayBuffer (int channel, juce::AudioBuffer<float>& buffer);
private:
    double sampleRate;
    int* writePosition;
    juce::AudioBuffer<float>* delayBuffer;
    juce::AudioProcessorValueTreeState* parameters;
};