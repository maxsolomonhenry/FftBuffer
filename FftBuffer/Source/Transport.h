/*
  ==============================================================================

    Transport.h
    Created: 12 Aug 2022 3:11:38pm
    Author:  Max Henry

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct Transport
{
public:
    void prepare(double sampleRate, int maxBlockSize)
    {
        samplesPerMinute = sampleRate * 60.0;
        ppqPositions.resize(maxBlockSize);
    }
    
    void process(juce::AudioPlayHead* playHead, int numSamples)
    {
        if (playHead != nullptr)
        {
            auto info = playHead->getPosition();
            
            double beatsPerSample = *info->getBpm() / samplesPerMinute;
            
            for (int n = 0; n < numSamples; ++n)
            {
                ppqPositions[n] = *info->getPpqPosition() + n * beatsPerSample;
            }
        }
    }
    
    std::vector<double> ppqPositions;
    
private:
    double samplesPerMinute;
};
