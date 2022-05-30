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

void OlaBuffer::initOlaBuffer()
{
    hopSize = frameSize / numOverlap;
    
    delayBuffer.resize(frameSize, 0.0);
    addBuffer.resize(hopSize, 0.0);
    frameBuffers.resize(numOverlap, std::vector<float>(frameSize, 0.0));
    newestFrame.resize(frameSize, 0.0);
    
    pDelayBuffer = 0;
    pAddBuffer = 0;
    pNewestFrame = 0;
}

void OlaBuffer::processBlock(juce::AudioBuffer<float> &block)
{
    // Convenience wrapper for block-based processing.
    
    // Plugin is mono.
    const int kChannelNo = 0;
    
    auto* channelData = block.getWritePointer(kChannelNo);
    auto numSamples = block.getNumSamples();
    
    for (int n = 0; n < numSamples; ++n)
    {
        process(channelData[n]);
    }
}

void OlaBuffer::process(float& x)
{
    delayBuffer[pDelayBuffer] = x;
    
    bool isThisHopComplete = (pDelayBuffer % hopSize == 0);
    
    if (isThisHopComplete)
    {
        fillNewestFrameFromDelayBuffer();
        
        // Any processing to newest frame would be here.
        float numOverlapAsFloat = static_cast<float>(numOverlap);
        
        for (int n = 0; n < frameSize; ++n)
        {
            newestFrame[n] /= numOverlapAsFloat;
        }
        
        frameBuffers[pNewestFrame].assign(newestFrame.begin(), newestFrame.end());
        fillAddBuffer();

        pNewestFrame = (pNewestFrame + 1) % numOverlap;
    }
    
    x = addBuffer[pAddBuffer];
    
    pDelayBuffer = (pDelayBuffer + 1) % frameSize;
    pAddBuffer = (pAddBuffer + 1) % hopSize;
}

void OlaBuffer::fillAddBuffer()
{
    std::fill(addBuffer.begin(), addBuffer.end(), 0.0);
    
    int pIn = 0;
    
    for (int n = 0; n < numOverlap; ++n)
    {
        // Positive-only modulus:
        //
        // https://stackoverflow.com/questions/14997165/fastest-way-to-get-a-positive-modulo-in-c-c
        int frameNo = ((pNewestFrame - n) % numOverlap + numOverlap) % numOverlap;
        
        for (int i = 0; i < hopSize; ++i)
            addBuffer[i] += frameBuffers[frameNo][pIn + i];
        
        pIn += hopSize;
    }
}

void OlaBuffer::fillNewestFrameFromDelayBuffer()
{
    int pRead = pDelayBuffer;
    
    for (int n = 0; n < frameSize; n++)
    {
        newestFrame[n] = delayBuffer[pRead];
        pRead = (pRead + 1) % frameSize;
    }
}
