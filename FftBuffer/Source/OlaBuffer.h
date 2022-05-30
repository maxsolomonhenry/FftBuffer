//
//  OlaBuffer.h
//  Created by Max Henry on 2022-05-29.
//

#pragma once
#include <JuceHeader.h>
#include <vector>

class OlaBuffer
{
public:
    OlaBuffer();
    OlaBuffer(int frameSize, int numOverlap);
    void processBlock(juce::AudioBuffer<float> &block);
    void process(float& x);
    void initOlaBuffer();
    
private:
    int frameSize;
    int numOverlap;
    int hopSize;
    
    std::vector<float> delayBuffer;
    std::vector<float> addBuffer;
    std::vector<std::vector<float>> frameBuffers;
    std::vector<float> newestFrame;
    
    int pDelayBuffer;
    int pAddBuffer;
    int pNewestFrame;
    
    void fillNewestFrameFromDelayBuffer();
    void fillAddBuffer();
};
