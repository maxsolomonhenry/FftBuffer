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
    juce::Label vanityLabel;
    
    juce::TextButton freezeButton;
    juce::TextButton refreshButton;
    juce::TextButton tempoSyncButton;
    
    juce::Slider rateSlider;
    juce::Label rateLabel;
    
    juce::Slider dryWetSlider;
    juce::Label dryWetLabel;
    
    juce::Slider envelopeDepthSlider;
    juce::Label envelopeDepthLabel;
    
    juce::ComboBox bufferSizeMenu;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> freezeButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tempoSyncButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envelopeDepthSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> bufferSizeMenuAttachment;

    FftBufferAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FftBufferAudioProcessorEditor)
};
