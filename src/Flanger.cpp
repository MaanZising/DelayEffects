#include "Flanger.h"

void Flanger::init (double& sampleRateRef, int& writePositionRef, juce::AudioProcessorValueTreeState& parametersRef, juce::AudioBuffer<float>& delayBufferRef)
{
    sampleRate = sampleRateRef;
    writePosition = &writePositionRef;
    parameters = &parametersRef;
    delayBuffer = &delayBufferRef;
}

void Flanger::readFromDelayBuffer (int channel, juce::AudioBuffer<float>& buffer)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer->getNumSamples();
    auto* bufferData = buffer.getWritePointer (channel);
    auto* delayBufferData = delayBuffer->getReadPointer (channel);
    auto delayTime = parameters->getRawParameterValue (FLANGER_DELAY_TIME_ID)->load();
    //smoothedDelayTime[channel].setTargetValue (delayTime);

    // set target delay time
    if (channel == 0)
        smoothedDelayTime.setTargetValue (delayTime);

    for (int sample = 0; sample < bufferSize; ++sample)
    {
        // write smoothed values into a buffer
        if (channel == 0)
            smoothedDelayTimeBuffer.setSample (0, sample, smoothedDelayTime.getNextValue());
        
        // length of audio from in the past
        double delayPosition { *writePosition - (sampleRate * smoothedDelayTimeBuffer.getSample (0, sample)) };

        auto lfoDepth = parameters->getRawParameterValue (FLANGER_LFO_DEPTH_ID)->load();
        auto feedback = parameters->getRawParameterValue (FLANGER_FEEDBACK_ID)->load();

        auto currentPosition = delayPosition + (getSineWaveData (channel) * sampleRate * smoothedDelayTimeBuffer.getSample (0, sample) * lfoDepth);
        if (currentPosition < 0)
            currentPosition += static_cast<double>(delayBufferSize);
        int readPosition { static_cast<int>(currentPosition) };
        double readPositionDelta { currentPosition - readPosition };

        // linear interpolation
        int previousPosition { (sample + readPosition) % delayBufferSize };
        int nextPosition { (previousPosition + 1) % delayBufferSize };
        auto currentData = (delayBufferData[nextPosition] - delayBufferData[previousPosition]) * readPositionDelta + delayBufferData[previousPosition];

        while (currentAngle[static_cast<long unsigned int> (channel)] > 2.0 * juce::MathConstants<double>::pi)
            currentAngle[static_cast<long unsigned int> (channel)] -= 2.0 * juce::MathConstants<double>::pi;

        updateAngleDelta();

        bufferData[sample] += currentData * feedback;
    }
}

void Flanger::updateAngleDelta()
{
    auto frequency = parameters->getRawParameterValue(FLANGER_LFO_FREQ_ID)->load();
    auto cyclesPerSample = frequency / sampleRate;
    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
}

float Flanger::getSineWaveData (int channel)
{
    auto lfoOffset = parameters->getRawParameterValue (FLANGER_LFO_OFFSET_ID)->load() * juce::MathConstants<double>::pi;
    float currentSample;

    if (channel % 2 == 1)
        currentSample = static_cast<float> (std::sin(currentAngle[static_cast<long unsigned int>(channel)] + (lfoOffset)));
    else
        currentSample = static_cast<float> (std::sin(currentAngle[static_cast<long unsigned int>(channel)]));
    
    currentAngle[static_cast<long unsigned int>(channel)] += angleDelta;

    return currentSample;
}