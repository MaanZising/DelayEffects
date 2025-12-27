#include "Delay.h"

void Delay::init (double& sampleRateRef, int& writePositionRef, juce::AudioProcessorValueTreeState& parametersRef, juce::AudioBuffer<float>& delayBufferRef)
{
    sampleRate = sampleRateRef;
    writePosition = &writePositionRef;
    parameters = &parametersRef;
    delayBuffer = &delayBufferRef;
}

void Delay::readFromDelayBufferByBlock (int channel, juce::AudioBuffer<float>& buffer)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer->getNumSamples();

    // length of audio from in the past
    auto delayTime = parameters->getRawParameterValue(DELAY_DELAY_TIME_ID)->load();
    auto readPosition = *writePosition - (sampleRate * delayTime);
    if (readPosition < 0)
        readPosition += delayBufferSize;

    float g = 0.7f;
    if (readPosition + bufferSize < delayBufferSize)
    {
        buffer.addFromWithRamp (channel, 0, delayBuffer->getReadPointer (channel, readPosition), bufferSize, g, g);
    }
    else
    {           
        auto numSamplesToEnd = delayBufferSize - readPosition;
        buffer.addFromWithRamp (channel, 0, delayBuffer->getReadPointer (channel, readPosition), numSamplesToEnd, g, g);
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        buffer.addFromWithRamp (channel, numSamplesToEnd, delayBuffer->getReadPointer (channel, 0), numSamplesAtStart, g, g);
    }
}