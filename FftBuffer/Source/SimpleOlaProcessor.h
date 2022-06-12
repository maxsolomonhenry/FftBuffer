//
//  SimpleOlaProcessor.h
//  FftBuffer - VST3
//
//  Created by Max Henry on 2022-06-11.
//

#pragma once
#include "OlaBuffer.h"

class SimpleOlaProcessor : public OlaBuffer
{
public:
    SimpleOlaProcessor();
    SimpleOlaProcessor(int frameSize, int numFrames);
    void processFrameBuffers() override;
};
