#include "Delay.h"

void Delay::init (double& sampleRateRef, int& writePositionRef, juce::AudioProcessorValueTreeState& parametersRef, juce::AudioBuffer<float>& delayBufferRef)
{
    sampleRate = sampleRateRef;
    writePosition = &writePositionRef;
    parameters = &parametersRef;
    delayBuffer = &delayBufferRef;
}

void Delay::readFromDelayBuffer (int channel, juce::AudioBuffer<float>& buffer)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer->getNumSamples();
    auto feedback = parameters->getRawParameterValue(DELAY_FEEDBACK_ID)->load();
    auto delayTime = parameters->getRawParameterValue(DELAY_DELAY_TIME_ID)->load();

    // length of audio from in the past
    auto readPosition = *writePosition - (sampleRate * delayTime);
    if (readPosition < 0)
        readPosition += delayBufferSize;

    if (readPosition + bufferSize < delayBufferSize)
    {
        buffer.addFromWithRamp (channel, 0, delayBuffer->getReadPointer (channel, readPosition), bufferSize, feedback, feedback);
    }
    else
    {           
        auto numSamplesToEnd = delayBufferSize - readPosition;
        buffer.addFromWithRamp (channel, 0, delayBuffer->getReadPointer (channel, readPosition), numSamplesToEnd, feedback, feedback);
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        buffer.addFromWithRamp (channel, numSamplesToEnd, delayBuffer->getReadPointer (channel, 0), numSamplesAtStart, feedback, feedback);
    }
}