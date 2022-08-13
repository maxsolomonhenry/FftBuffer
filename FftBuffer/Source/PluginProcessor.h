/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SimpleOlaProcessor.h"
#include "Transport.h"

//==============================================================================
/**
*/
class FftBufferAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    FftBufferAudioProcessor();
    ~FftBufferAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void extracted(juce::AudioBuffer<float> &buffer, float isFreezeOn, float isTempoSyncOn);
    
    void extracted(juce::AudioBuffer<float> &buffer, float envelopeDepth);
    
void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setStutterRate(const float &input, const bool &isTempoSyncOn);
    juce::AudioProcessorValueTreeState params;
    
    std::vector<SimpleOlaProcessor> olaProcessor;
    
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    juce::LinearSmoothedValue<float> dryWetSmoothedValue { 1.0 };
    
    int ctrStutter;
    int samplesPerStutterPeriod;
    
    float stutterRate;
    float stutterBeatSubdivision;
    
    juce::AudioBuffer<float> dryDelayBuffer;
    juce::dsp::DelayLine<float> dryDelayLine;
    
    juce::dsp::DelayLine<float> envelopeDelayLine;
    
    juce::AudioBuffer<float> envelopeBuffer;
    juce::dsp::IIR::Filter<float> envelopeLpFilter;
    
    const int kNumSpectralBufferSamples = 4096;
    const int kNumOverlap = 4;
    
    const float kEps = 1e-4;
    const float kEnvelopeTrim = 0.9;
    const float kEnvelopeGainLinear = 5.623413251903491;
    const int kEnvelopeDelaySamples = 1100;
    const float kMaxStutterRateHz = 12.0;
    const float kNumSyncConditions = 10.0;
    const float kEqualPowerCoefficient = 0.70710678118;
    
    juce::dsp::ProcessSpec getProcessSpec(double sampleRate, int samplesPerBlock);
    
    void initOlaProcessor();
    void initDrySignalChain(const juce::dsp::ProcessSpec &spec);
    void initEnvelopeFollower(const juce::dsp::ProcessSpec &spec);
    void initStutterCounting();
    
    void delayTheDryAudio(juce::dsp::AudioBlock<float> &block, juce::dsp::AudioBlock<float> &dryDelayBlock);
    void calculateTheAmplitudeEnvelope(juce::dsp::AudioBlock<float> &block, juce::dsp::AudioBlock<float> &envelopeBlock);
    void applyFreezeEffect(juce::AudioBuffer<float> &buffer, bool isFreezeOn, bool isTempoSyncOn);
    void applyEnvelope(juce::AudioBuffer<float> &buffer, float &envelopeDepth);
    void mixWetAndDry(juce::AudioBuffer<float> &buffer, float &dryWetGuiValue);
    
    Transport transport;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FftBufferAudioProcessor)
};
