/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FftBufferAudioProcessorEditor::FftBufferAudioProcessorEditor (FftBufferAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    freezeButton.setButtonText("Freeze");
    addAndMakeVisible(freezeButton);
    
    freezeButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.params, "FREEZE", freezeButton);
    
    refreshButton.setButtonText("Refresh");
    addAndMakeVisible(refreshButton);
    
    refreshButton.onClick = [this]
    {
        for (int i = 0; i < audioProcessor.olaProcessor.size(); ++i )
            audioProcessor.olaProcessor[i].setIsRefreshRequested(true);
        
    };
    
    rateSlider.setTitle("Stutter Rate");
    rateSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(rateSlider);
    rateSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "STUTTERRATE", rateSlider);
    
    dryWetSlider.setTitle("Dry/Wet");
    dryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(dryWetSlider);
    dryWetSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DRYWET", dryWetSlider);

    setSize (400, 800);
}

FftBufferAudioProcessorEditor::~FftBufferAudioProcessorEditor()
{
}

//==============================================================================
void FftBufferAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
}

void FftBufferAudioProcessorEditor::resized()
{
    int itemWidth = 100;

    int freezeHeight = 50;
    int refreshHeight = 50;
    
    int knobWidth = 200;
    int knobHeight = 200;
    
    int border = 30;

    freezeButton.setBounds((getWidth() - itemWidth) / 2, border, itemWidth, freezeHeight);
    
    refreshButton.setBounds((getWidth() - itemWidth) / 2, border + freezeHeight + border, itemWidth, refreshHeight);
    rateSlider.setBounds((getWidth() - knobWidth) / 2, border + freezeHeight + border + refreshHeight + border, knobWidth, knobHeight);
    
    dryWetSlider.setBounds((getWidth() - knobWidth) / 2, border + freezeHeight + border + refreshHeight + border + knobHeight + border, knobWidth, knobHeight);
}
