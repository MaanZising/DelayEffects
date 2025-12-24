#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return "Granular Synthesizer";
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    int delayBufferSize { static_cast<int>(sampleRate * 2.0) };
    delayBuffer.setSize (getTotalNumInputChannels(), delayBufferSize);
    for (auto channel = 0; channel < getTotalNumOutputChannels(); ++channel)
        delayBuffer.clear (channel, 0, delayBuffer.getNumSamples());

    currentSampleRate = sampleRate;
    updateAngleDelta();

    for (auto channel = 0; channel < getTotalNumOutputChannels(); ++channel)
        currentAngle.push_back (0.);

    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const auto* inputData = buffer.getReadPointer (channel);
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            channelData[sample] = inputData[sample];

        #if 0
        // copy input signal to delay buffer
        fillDelayBuffer (channel, buffer);
        // read from the past in the delay buffer, then add back to main buffer
        readFromDelayBufferByBlock (channel, buffer);
        // copy signal from main buffer to delay buffer, creating feedback loop
        fillDelayBuffer(channel, buffer);
        #endif

        fillDelayBuffer (channel, buffer);
        readFromDelayBufferByFrame (channel, buffer);

        //for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        //    channelData[sample] = getSineWaveData (channel);
        
        while (currentAngle[static_cast<long unsigned int> (channel)] > 2.0 * juce::MathConstants<double>::pi)
            currentAngle[static_cast<long unsigned int> (channel)] -= 2.0 * juce::MathConstants<double>::pi;
    }

    // determine where to write the next block data to the delay buffer
    updateWritePosition (buffer);
}

void AudioPluginAudioProcessor::updateWritePosition (juce::AudioBuffer<float>& buffer)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    writePosition += bufferSize;
    writePosition %= delayBufferSize;
}

void AudioPluginAudioProcessor::fillDelayBuffer(int channel, juce::AudioBuffer<float>& buffer)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();

    // check if main buffer copies to delay buffer without needing to wrap
    if (delayBufferSize >= bufferSize + writePosition)
    {
        delayBuffer.copyFrom (channel, writePosition, buffer.getWritePointer (channel), bufferSize);
    }
    else
    {
        // determine how much space is left at the end of the delay buffer
        auto numSamplesToEnd = delayBufferSize - writePosition;
        delayBuffer.copyFrom (channel, writePosition, buffer.getWritePointer (channel), numSamplesToEnd);

        // calculate the amount of remaining samples to copy
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        delayBuffer.copyFrom (channel, 0, buffer.getWritePointer (channel, numSamplesToEnd), numSamplesAtStart);
    }
}

void AudioPluginAudioProcessor::readFromDelayBufferByBlock (int channel, juce::AudioBuffer<float>& buffer)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();

    // length of audio from in the past
    //auto readPosition = writePosition - (getSampleRate() * delayTime);
    auto readPosition = writePosition - getSampleRate();
    if (readPosition < 0)
        readPosition += delayBufferSize;

    float g = 0.7f;
    if (readPosition + bufferSize < delayBufferSize)
    {
        buffer.addFromWithRamp (channel, 0, delayBuffer.getReadPointer (channel, readPosition), bufferSize, g, g);
    }
    else
    {           
        auto numSamplesToEnd = delayBufferSize - readPosition;
        buffer.addFromWithRamp (channel, 0, delayBuffer.getReadPointer (channel, readPosition), numSamplesToEnd, g, g);
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        buffer.addFromWithRamp (channel, numSamplesToEnd, delayBuffer.getReadPointer (channel, 0), numSamplesAtStart, g, g);
    }
}

void AudioPluginAudioProcessor::readFromDelayBufferByFrame (int channel, juce::AudioBuffer<float>& buffer)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    auto* bufferData = buffer.getWritePointer (channel);
    auto* delayBufferData = delayBuffer.getReadPointer (channel);
    float g { 0.7f };
    
    // length of audio from in the past
    double delayPosition { writePosition - (getSampleRate() * delayTime) };

    for (int sample = 0; sample < bufferSize; ++sample)
    {
        auto currentPosition = delayPosition + (getSineWaveData (channel) * getSampleRate() * 0.015);
        if (currentPosition < 0)
            currentPosition += static_cast<double>(delayBufferSize);
        int readPosition { static_cast<int>(currentPosition) };
        double readPositionDelta { currentPosition - readPosition };

        // linear interpolation
        int previousPosition { (sample + readPosition) % delayBufferSize };
        int nextPosition { (previousPosition + 1) % delayBufferSize };
        // auto currentData = (delayBufferData[nextPosition] * readPositionDelta) + (delayBufferData[previousPosition] * (1 - readPositionDelta));
        auto currentData = (delayBufferData[nextPosition] - delayBufferData[previousPosition]) * readPositionDelta + delayBufferData[previousPosition];

        bufferData[sample] += currentData * g;
    }
}

void AudioPluginAudioProcessor::updateAngleDelta()
{
    auto cyclesPerSample = frequency / currentSampleRate; // [2]
    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
}

float AudioPluginAudioProcessor::getSineWaveData (int channel)
{
    auto currentSample = static_cast<float> (std::sin(currentAngle[static_cast<long unsigned int>(channel)]));
    currentAngle[static_cast<long unsigned int>(channel)] += angleDelta;
    return currentSample;
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
