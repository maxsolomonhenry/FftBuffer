//
//  SimpleOlaProcessor.h
//  FftBuffer - VST3
//
//  Created by Max Henry on 2022-06-11.
//

#pragma once
#include "OlaBuffer.h"

class SimpleOlaProcesser : public OlaBuffer
{
public:
    SimpleOlaProcesser();
    SimpleOlaProcesser(int frameSize, int numFrames);
    void processFrameBuffers() override;
};
