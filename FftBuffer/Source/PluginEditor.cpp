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

    int borderSize = 50;
    int bufferSize = 20;
    
    auto totalRegion = getLocalBounds();
    auto leftMargin = totalRegion.removeFromLeft(borderSize);
    auto rightMargin = totalRegion.removeFromRight(borderSize);
    auto topMargin = totalRegion.removeFromTop(borderSize);
    auto bottomMargine = totalRegion.removeFromBottom(borderSize);
    
    int rightComponentHeight = 120;
    int leftComponentHeight = totalRegion.getHeight() / 4;
    
    int columnWidth = totalRegion.getWidth() / 2;
    
    auto right = totalRegion.removeFromRight(columnWidth);
    auto left = totalRegion.removeFromLeft(columnWidth);
    
    vanityLabel.setBounds(left.removeFromTop(rightComponentHeight));
    freezeButton.setBounds(left.removeFromTop(rightComponentHeight).reduced(25));
    refreshButton.setBounds(left.removeFromTop(rightComponentHeight).reduced(25));
    rateSlider.setBounds(right.removeFromTop(rightComponentHeight).reduced(bufferSize));
    dryWetSlider.setBounds(right.removeFromTop(rightComponentHeight).reduced(bufferSize));
    envelopeDepthSlider.setBounds(right.removeFromTop(rightComponentHeight).reduced(bufferSize));
}
