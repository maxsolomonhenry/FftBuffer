/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SimpleOlaProcessor.h"

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
    void setStutterRateHz(float input);
    juce::AudioProcessorValueTreeState params;
    
    std::vector<SimpleOlaProcessor> olaProcessor;
    
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    juce::LinearSmoothedValue<float> dryWetSmoothedValue { 1.0 };
    
    int ctrStutter;
    int samplesPerStutterPeriod;
    
    float stutterRateHz;
    const float eps{1e-4};
    juce::AudioBuffer<float> dryDelayBuffer;
    juce::dsp::DelayLine<float> dryDelayLine;
    
    juce::dsp::DelayLine<float> envelopeDelayLine;
    
    juce::AudioBuffer<float> envelopeBuffer;
    juce::dsp::IIR::Filter<float> envelopeFollower;
    
    const float kEnvelopeTrim = 0.9;
    const float kEnvelopeGainLinear = 5.623413251903491;
    const int kEnvelopeDelaySamples = 1100;
    const float kCrossFadeIncrement = 0.0009765625;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FftBufferAudioProcessor)
};
