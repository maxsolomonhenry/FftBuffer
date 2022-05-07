/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FftBufferAudioProcessor::FftBufferAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                     #endif
                       )
#endif
{
    const int kFrameSize = 2048;
    const int kNumOverlap = 4;
    
    int hopSize = kFrameSize / kNumOverlap;
    int bufferSize = (2 * kNumOverlap - 1) * hopSize;
    
    asyncBuffer.setSize(bufferSize);
}

FftBufferAudioProcessor::~FftBufferAudioProcessor()
{
}

//==============================================================================
const juce::String FftBufferAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FftBufferAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FftBufferAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FftBufferAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FftBufferAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FftBufferAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FftBufferAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FftBufferAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FftBufferAudioProcessor::getProgramName (int index)
{
    return {};
}

void FftBufferAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FftBufferAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void FftBufferAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FftBufferAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
#endif

void FftBufferAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Plugin is mono.
    const int kChannelNo = 0;
    auto* channelData = buffer.getWritePointer(kChannelNo);
    
    auto numSamples = buffer.getNumSamples();
    
    asyncBuffer.incrementReadPointer(numSamples);

    asyncBuffer.writeTo(channelData, numSamples);
    asyncBuffer.readFrom(channelData, numSamples);

    asyncBuffer.incrementWritePointer(numSamples);
}

//==============================================================================
bool FftBufferAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FftBufferAudioProcessor::createEditor()
{
    return new FftBufferAudioProcessorEditor (*this);
}

//==============================================================================
void FftBufferAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FftBufferAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FftBufferAudioProcessor();
}
