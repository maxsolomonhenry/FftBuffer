//
//  SimpleOlaProcessor.cpp
//  FftBuffer - VST3
//
//  Created by Max Henry on 2022-06-11.
//

#include "SimpleOlaProcessor.h"

SimpleOlaProcessor::SimpleOlaProcessor() : OlaBuffer(1024, 4) {}

SimpleOlaProcessor::SimpleOlaProcessor(int frameSize, int numFrames)
: OlaBuffer(frameSize, numFrames) {}

void SimpleOlaProcessor::processFrameBuffers()
{
    // Any processing to newest frame would be here.
    //
    // For now, simple gain adjust to compensate for OLA.
    
    float numOverlapAsFloat = static_cast<float>(numOverlap);
    
    for (int n = 0; n < frameSize; ++n)
    {
        frameBuffers[pNewestFrame][n] /= numOverlapAsFloat;
    }
}
