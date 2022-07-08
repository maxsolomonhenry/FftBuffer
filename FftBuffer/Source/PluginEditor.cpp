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

    setSize (400, 400);
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
    int spacing = 30;
    int itemWidth = 100;
    int itemHeight = 100;
    
    freezeButton.setBounds((getWidth() - itemWidth) / 2, getHeight() / 2 + itemHeight / 2, itemWidth, itemHeight);
    refreshButton.setBounds((getWidth() - itemWidth) / 2, getHeight() / 2 - itemHeight / 2, itemWidth, itemHeight);
}
