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
                       ),
        parameters (*this, &undoManager, juce::Identifier ("DEP"), 
        {
            //////////////////////////////////////////////// current effect
            std::make_unique<juce::AudioParameterInt>
            (
                "tab_index",
                "Tab Index",
                0,
                1,
                0
            ),
            //////////////////////////////////////////////// delay
            std::make_unique<juce::AudioParameterFloat>
            (
                DELAY_DELAY_TIME_ID,
                DELAY_DELAY_TIME_NAME,
                juce::NormalisableRange<float> (0.001f, 2.000f, 0.0001f, 0.4f),
                1.f
            ),
            std::make_unique<juce::AudioParameterFloat>
            (
                DELAY_FEEDBACK_ID,
                DELAY_FEEDBACK_NAME,
                juce::NormalisableRange<float> (0.000f, 1.000f, 0.0001f),
                0.5f
            ),
            //////////////////////////////////////////////// flanger
            std::make_unique<juce::AudioParameterFloat>
            (
                FLANGER_LFO_FREQ_ID,
                FLANGER_LFO_FREQ_NAME,
                juce::NormalisableRange<float> (0.01f, 20.00f, 0.001f, 0.4f),
                0.6f
            ),
            std::make_unique<juce::AudioParameterFloat>
            (
                FLANGER_LFO_OFFSET_ID,
                FLANGER_LFO_OFFSET_NAME,
                juce::NormalisableRange<float> (0.f, 1.f, 0.0001f),
                0.5f
            ),
            std::make_unique<juce::AudioParameterFloat>
            (
                FLANGER_DELAY_TIME_ID,
                FLANGER_DELAY_TIME_NAME,
                juce::NormalisableRange<float> (0.000f, 1.000f, 0.00001f, 0.3f),
                0.005f
            ),
            std::make_unique<juce::AudioParameterFloat>
            (
                FLANGER_LFO_DEPTH_ID,
                FLANGER_LFO_DEPTH_NAME,
                juce::NormalisableRange<float> (0.f, 1.f, 0.0001f),
                0.5f
            ),
            std::make_unique<juce::AudioParameterFloat>
            (
                FLANGER_FEEDBACK_ID,
                FLANGER_FEEDBACK_NAME,
                juce::NormalisableRange<float> (0.000f, 1.000f, 0.0001f),
                0.7f
            ),
            std::make_unique<juce::AudioParameterBool>
            (
                FLANGER_FEEDBACK_TOGGLE_ID,
                FLANGER_FEEDBACK_TOGGLE_NAME,
                false
            ),
            //////////////////////////////////////////////// master
            std::make_unique<juce::AudioParameterFloat>
            (
                GAIN_ID,
                GAIN_NAME,
                juce::NormalisableRange<float> (0.f, 2.f, 0.0001f, 0.4f),
                0.7f
            ),
            std::make_unique<juce::AudioParameterFloat>
            (
                MIX_ID,
                MIX_NAME,
                juce::NormalisableRange<float> (0.f, 1.f, 0.0001f),
                1.f
            )
        })
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return "Delay Effects";
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
    // initialize delay buffer
    int delayBufferSize { static_cast<int>(sampleRate * 2.01) };
    delayBuffer.setSize (getTotalNumInputChannels(), delayBufferSize);
    for (auto channel = 0; channel < getTotalNumOutputChannels(); ++channel)
        delayBuffer.clear (channel, 0, delayBuffer.getNumSamples());
    
    // initialize effects
    delay.init (sampleRate, writePosition, parameters, delayBuffer);
    flanger.init (sampleRate, writePosition, parameters, delayBuffer);
    
    // initialize flanger
    flanger.updateAngleDelta();
    for (auto channel = 0; channel < getTotalNumOutputChannels(); ++channel)
    {
        flanger.currentAngle.push_back (0.);
        //flanger.smoothedDelayTime[channel].reset (sampleRate, samplesPerBlock*2/sampleRate);
    }
    flanger.smoothedDelayTime.reset (sampleRate, samplesPerBlock*2/sampleRate);
    flanger.smoothedDelayTimeBuffer.setSize (1, samplesPerBlock);

    juce::ignoreUnused (samplesPerBlock);
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
        std::vector<float> input;

        const auto* inputData = buffer.getReadPointer (channel);
        auto* channelData = buffer.getWritePointer (channel);
        int tabIndex = static_cast<int> (parameters.getRawParameterValue("tab_index")->load());

        // get input signal
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = inputData[sample];
            input.push_back(inputData[sample]);
        }

        // set current effect
        switch (tabIndex)
        {
            case 0:
                fillDelayBuffer (channel, buffer);
                delay.readFromDelayBuffer (channel, buffer);
                fillDelayBuffer (channel, buffer);
                break;
            case 1:
                fillDelayBuffer (channel, buffer);
                flanger.readFromDelayBuffer (channel, buffer);
                if (parameters.getRawParameterValue(FLANGER_FEEDBACK_TOGGLE_ID)->load())
                    fillDelayBuffer (channel, buffer);
                break;
        }
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // equal power crossfade
            auto mix = parameters.getRawParameterValue(MIX_ID)->load();
            channelData[sample] =
            (channelData[sample] * std::sin(mix * juce::MathConstants<double>::pi / 2.0)) +
            (input[sample] * std::cos(mix * juce::MathConstants<double>::pi / 2.0));

            // gain
            channelData[sample] = channelData[sample] * parameters.getRawParameterValue(GAIN_ID)->load();
        }
    }

    // determine where to write the next block of data to the delay buffer
    updateWritePosition (buffer);

    if (clearBuffer)
    {
        for (auto channel = 0; channel < totalNumOutputChannels; ++channel)
            buffer.clear (channel, 0, buffer.getNumSamples());

        clearBuffer = false;
    }
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
    //juce::ignoreUnused (destData);

    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    //juce::ignoreUnused (data, sizeInBytes);

    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
