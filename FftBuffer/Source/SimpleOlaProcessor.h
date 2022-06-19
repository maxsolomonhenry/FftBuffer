//
//  SimpleOlaProcessor.h
//  FftBuffer - VST3
//
//  Created by Max Henry on 2022-06-11.
//

#pragma once
#include "OlaBuffer.h"
#include <JuceHeader.h>
#include <math.h>

class SimpleOlaProcessor : public OlaBuffer
{
public:
    SimpleOlaProcessor();
    SimpleOlaProcessor(int frameSize, int numFrames);
    void processFrameBuffers() override;
    void setIsEffectRequested(bool input);
    
private:
    void init(int frameSize);
    void initWindow(int frameSize);
    void initFftBuffer(int frameSize);
    void convertToMagnitudeAndPhase(std::vector<float> &X);
    
    int nonnegativeModulus(int i, int n);
    
    juce::dsp::FFT fft;
    std::vector<std::vector<float>> fftBuffer;
    std::vector<float> window;
    
    bool isEffectRequested;
};
