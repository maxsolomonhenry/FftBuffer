/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FftBufferAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    FftBufferAudioProcessorEditor (FftBufferAudioProcessor&);
    ~FftBufferAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::ToggleButton freezeButton;
    juce::TextButton refreshButton;
    juce::Slider rateSlider;
    juce::Slider dryWetSlider;
    juce::Slider envelopeDepthSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> freezeButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envelopeDepthSliderAttachment;

    FftBufferAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FftBufferAudioProcessorEditor)
};
