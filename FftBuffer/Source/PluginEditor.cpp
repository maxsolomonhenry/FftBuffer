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
    vanityLabel.setText("Stutter+Hold", juce::dontSendNotification);
    vanityLabel.setJustificationType(juce::Justification::centred);
    vanityLabel.setFont(juce::Font(16.0, juce::Font::bold));
    vanityLabel.setColour(juce::Label::textColourId, juce::Colours::skyblue);
    addAndMakeVisible(vanityLabel);
    
    freezeButton.setButtonText("Freeze");
    freezeButton.setClickingTogglesState(true);
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
    rateSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(rateSlider);
    rateSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "STUTTERRATE", rateSlider);
    
    rateLabel.setText("Stutter Rate", juce::dontSendNotification);
    rateLabel.attachToComponent(&rateSlider, false);
    rateLabel.setJustificationType(juce::Justification::centred);
    
    dryWetSlider.setTitle("Dry/Wet");
    dryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(dryWetSlider);
    dryWetSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DRYWET", dryWetSlider);
    
    dryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
    dryWetLabel.attachToComponent(&dryWetSlider, false);
    dryWetLabel.setJustificationType(juce::Justification::centred);
    
    envelopeDepthSlider.setTitle("Envelope Depth");
    envelopeDepthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    envelopeDepthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(envelopeDepthSlider);
    envelopeDepthSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "ENVDEPTH", envelopeDepthSlider);
    
    envelopeDepthLabel.setText("Envelope Depth", juce::dontSendNotification);
    envelopeDepthLabel.attachToComponent(&envelopeDepthSlider, false);
    envelopeDepthLabel.setJustificationType(juce::Justification::centred);
    
    setSize (450, 460);
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

    int borderSize = 25;
    int bufferSize = 20;
    
    auto totalRegion = getLocalBounds();
    totalRegion.reduce(borderSize, borderSize);
    
    int componentHeight = totalRegion.getHeight() / 3;
    int componentWidth = totalRegion.getWidth() / 2;
    
    auto right = totalRegion.removeFromRight(componentWidth);
    auto left = totalRegion.removeFromLeft(componentWidth);
    
    vanityLabel.setBounds(left.removeFromTop(componentHeight));
    freezeButton.setBounds(left.removeFromTop(componentHeight).reduced(35));
    refreshButton.setBounds(left.removeFromTop(componentHeight).reduced(35));
    rateSlider.setBounds(right.removeFromTop(componentHeight).reduced(bufferSize));
    dryWetSlider.setBounds(right.removeFromTop(componentHeight).reduced(bufferSize));
    envelopeDepthSlider.setBounds(right.removeFromTop(componentHeight).reduced(bufferSize));
}
