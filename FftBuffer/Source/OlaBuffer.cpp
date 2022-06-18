//
//  OlaBuffer.cpp
//  Created by Max Henry on 2022-05-29.
//

#include "OlaBuffer.h"

OlaBuffer::OlaBuffer(int frameSize, int numOverlap)
    : frameSize(frameSize)
    , numOverlap(numOverlap)
{
    initOlaBuffer();
}

OlaBuffer::OlaBuffer()
{
    frameSize = 1024;
    numOverlap = 4;
    
    initOlaBuffer();
}

OlaBuffer::~OlaBuffer() {}

void OlaBuffer::initOlaBuffer()
{
    hopSize = frameSize / numOverlap;
    
    delayBuffer.resize(frameSize, 0.0);
    overlapAddBuffer.resize(hopSize, 0.0);
    frameBuffers.resize(numOverlap, std::vector<float>(frameSize, 0.0));
    
    pDelayBuffer = 0;
    pOverlapAddBuffer = 0;
    pNewestFrame = 0;
}

void OlaBuffer::process(float& x)
{
    delayBuffer[pDelayBuffer] = x;
    
    bool isThisHopComplete = (pDelayBuffer % hopSize == 0);
    
    if (isThisHopComplete)
    {
        std::vector<float>& newestFrame = frameBuffers[pNewestFrame];
        
        fillFrameFromDelayBuffer(newestFrame);
        
        // Virtual function to be defined by inheriting class.
        processFrameBuffers();

        // OLA calculations.
        fillOverlapAddBuffer();

        pNewestFrame = (pNewestFrame + 1) % numOverlap;
    }
    
    x = overlapAddBuffer[pOverlapAddBuffer];
    
    pDelayBuffer = (pDelayBuffer + 1) % frameSize;
    pOverlapAddBuffer = (pOverlapAddBuffer + 1) % hopSize;
}

void OlaBuffer::processFrameBuffers() {}

void OlaBuffer::fillOverlapAddBuffer()
{
    // Overlap, add and store the `hopSize`-length part where all frames overlap.
    std::fill(overlapAddBuffer.begin(), overlapAddBuffer.end(), 0.0);
    
    int pIn = 0;
    
    for (int n = 0; n < numOverlap; ++n)
    {
        // Positive-only modulus:
        //
        // https://stackoverflow.com/questions/14997165/fastest-way-to-get-a-positive-modulo-in-c-c
        int frameNo = ((pNewestFrame - n) % numOverlap + numOverlap) % numOverlap;
        
        for (int i = 0; i < hopSize; ++i)
            overlapAddBuffer[i] += frameBuffers[frameNo][pIn + i];
        
        pIn += hopSize;
    }
}

void OlaBuffer::fillFrameFromDelayBuffer(std::vector<float> &frame)
{
    int pRead = pDelayBuffer;
    
    for (int n = 0; n < frameSize; n++)
    {
        frame[n] = delayBuffer[pRead];
        pRead = (pRead + 1) % frameSize;
    }
}
