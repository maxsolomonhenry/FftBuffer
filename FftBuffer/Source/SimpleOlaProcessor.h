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
    void init(int frameSize, int numFrames);
    void initWindow(int frameSize);
    void initFftBuffer(int frameSize);
    void initPhaseAdvanceAndPhaseDelta(int frameSize, int numFrames);
    void convertToMagnitudeAndPhase(std::vector<float> &X);
    void convertToPolar(std::vector<float> &X);
    
    int nonnegativeModulus(int i, int n);
    
    juce::dsp::FFT fft;
    std::vector<std::vector<float>> fftBuffer;
    std::vector<float> phaseAdvance;
    std::vector<float> phaseDelta;
    std::vector<float> window;
    
    bool isEffectRequested;
};
