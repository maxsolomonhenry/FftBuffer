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
    
    setSize (400, 300);
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
    int freezeWidth = 100;
    int freezeHeight = 100;
    
    freezeButton.setBounds((getWidth() - freezeWidth) / 2, (getHeight() - freezeHeight) / 2, freezeWidth, freezeHeight);
}
