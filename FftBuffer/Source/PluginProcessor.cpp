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
    , params(*this, nullptr, "PARAMETERS", createParameters() )
#endif
{
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
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    const int kNumSpectralBufferSamples = 4096;
    
    // Necessary in the case that `prepareToPlay` is called more than once.
    olaProcessor.clear();
    
    // Build one processor per channel.
    for (int i = 0; i < getTotalNumInputChannels(); ++i)
    {
        olaProcessor.push_back(SimpleOlaProcessor(kNumSpectralBufferSamples, 4));
    }
        
    dryDelayLine = juce::dsp::DelayLine<float>(kNumSpectralBufferSamples);
    dryDelayLine.prepare(spec);
    dryDelayLine.setDelay(kNumSpectralBufferSamples);
    
    envelopeDelayLine = juce::dsp::DelayLine<float>(kNumSpectralBufferSamples - kEnvelopeDelaySamples);
    envelopeDelayLine.prepare(spec);
    envelopeDelayLine.setDelay(kNumSpectralBufferSamples - kEnvelopeDelaySamples);
    
    envelopeFollower = juce::dsp::IIR::Filter<float>(juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 100.0, 1.0));
    envelopeFollower.prepare(spec);
    
    dryDelayBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);
    envelopeBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);
    
    stutterRate = 0.0;
    samplesPerStutterPeriod = std::numeric_limits<int>::max();
    ctrStutter = 0;
    
    setLatencySamples(kNumSpectralBufferSamples);
    
    dryWetSmoothedValue.reset(sampleRate, 0.0001);
    
    transport.prepare(sampleRate, samplesPerBlock);
}

void FftBufferAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FftBufferAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

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
    transport.process(getPlayHead(), buffer.getNumSamples());
    
    // Get parameter valeus from GUI.
    auto isFreezeOn = params.getRawParameterValue("FREEZE")->load();
    auto isTempoSyncOn = params.getRawParameterValue("TEMPOSYNC")->load();
    auto stutterRate = params.getRawParameterValue("STUTTERRATE")->load();
    auto dryWetGuiValue = params.getRawParameterValue("DRYWET")->load();
    auto envelopeDepth = params.getRawParameterValue("ENVDEPTH")->load();
    
    dryWetSmoothedValue.setTargetValue(dryWetGuiValue);
    
    // Checks if new value, reassigns as necessary.
    setStutterRate(stutterRate);
    
    // Blocks.
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::AudioBlock<float> dryDelayBlock(dryDelayBuffer);
    juce::dsp::AudioBlock<float> envelopeBlock(envelopeBuffer);
    
    // Delay dry audio.
    juce::dsp::ProcessContextNonReplacing<float> dryDelayContext(block, dryDelayBlock);
    dryDelayLine.process(dryDelayContext);
    
    // Calculate envelope.
    envelopeBlock.replaceWithAbsoluteValueOf(block);
    juce::dsp::ProcessContextReplacing<float> envelopeContext(envelopeBlock);
    envelopeFollower.process(envelopeContext);
    envelopeBlock.multiplyBy(kEnvelopeGainLinear);
    
    
    // Logic for "stutter" timing (i.e., the freeze on/off).
    for (int c = 0; c < buffer.getNumChannels(); ++c)
    {
        olaProcessor[c].setIsEffectRequested(isFreezeOn);
        
        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            if (isTempoSyncOn)
            {
                if (fmod(transport.getBeatAtSample(n), 0.5) < 1e-4)
                    olaProcessor[c].setIsRefreshRequested(true);
            }
            else
            {
                if (c == 0)
                    ctrStutter++;

                if (ctrStutter >= samplesPerStutterPeriod)
                {
                    olaProcessor[c].setIsRefreshRequested(true);
                    
                    if (c == (olaProcessor.size() - 1))
                        ctrStutter = 0;
                }
            }
            
            olaProcessor[c].process(buffer.getWritePointer(c)[n]);
        }
    }
    
    // Calculate dry/wet coefficients (TODO: linear for now).
    auto wetVal = dryWetSmoothedValue.getNextValue();
    auto dryVal = 1.0 - wetVal;
    
    // Apply amplitude envelope, and mix dry/wet.
    for (int c = 0; c < buffer.getNumChannels(); ++c)
    {
        auto* outPointer = buffer.getWritePointer(c);
        auto* dryPointer = dryDelayBuffer.getReadPointer(c);
        auto* envelopePointer = envelopeBuffer.getWritePointer(c);
        
        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            // Clip envelope to a maximum of 1.0.
            envelopePointer[n] = envelopePointer[n] > 1.0 ? 1.0 : envelopePointer[n];
            
            outPointer[n] *= envelopeDepth * (envelopePointer[n] - kEnvelopeTrim) + kEnvelopeTrim;
            outPointer[n] = outPointer[n] * wetVal + dryPointer[n] * dryVal;
        }
    }
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
    auto state = params.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FftBufferAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (params.state.getType()))
            params.replaceState(juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FftBufferAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout FftBufferAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ "FREEZE", 1 }, "Freeze", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"TEMPOSYNC", 2}, "Tempo Sync", false));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "STUTTERRATE" , 3}, "Stutter Rate", juce::NormalisableRange<float>(0.0, 1.0) ,0.0));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "DRYWET", 4 }, "Dry/Wet Mix", juce::NormalisableRange<float>(0.0, 1.0), 1.0));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "ENVDEPTH", 5 }, "Envelope Depth", juce::NormalisableRange<float>(0.0, 1.0), 0.0));
    
    return { params.begin(), params.end() };
}

void FftBufferAudioProcessor::setStutterRate(float input)
{
    bool hasValueChanged = (abs(input - stutterRate) > eps);
    
    if (hasValueChanged)
    {
        stutterRate = input;
        
        if (stutterRate > 0.0)
        {
            float stutterRateHz = static_cast<float>(stutterRate) * kMaxStutterRateHz;
            
            float periodSecs = 1.0 / stutterRateHz;
            
            float answerAsFloat = periodSecs * static_cast<float>(getSampleRate());
            samplesPerStutterPeriod = static_cast<int>(answerAsFloat);
        }
        else
        {
            samplesPerStutterPeriod = std::numeric_limits<int>::max();
        }
        
        DBG(samplesPerStutterPeriod);
    }
}
