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
    virtual ~OlaBuffer();

    void process(float& x);
    void initOlaBuffer();
    
protected:
    int frameSize;
    int numOverlap;
    int hopSize;
    
    std::vector<float> delayBuffer;
    std::vector<float> overlapAddBuffer;
    std::vector<std::vector<float>> frameBuffers;
    
    int pDelayBuffer;
    int pOverlapAddBuffer;
    int pNewestFrame;
    
    void fillFrameFromDelayBuffer(std::vector<float> &frame);
    void fillOverlapAddBuffer();
    virtual void processFrameBuffers();
};
