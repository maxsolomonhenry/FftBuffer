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
    void setIsRefreshRequested(bool input);
    void setStutterRateHz(float input);
    
    bool getIsVoiced();
    
private:
    void init(int frameSize, int numFrames);
    void initWindow(int frameSize, int numFrames);
    void initFftBuffer(int frameSize);
    void initPhaseAdvanceAndPhaseDelta(int frameSize, int numFrames);
    void convertToMagnitudeAndPhase(std::vector<float> &X);
    void convertToPolar(std::vector<float> &X);
    
    int nonnegativeModulus(int i, int n);
    int ctrRefresh;
    const int kNumRefreshFrames{2};
    
    juce::dsp::FFT fft;
    std::vector<std::vector<float>> fftBuffer;
    std::vector<float> phaseAdvance;
    std::vector<float> phaseDelta;
    std::vector<float> window;
    
    bool isEffectRequested;
    bool isRefreshRequested;
    
    bool determineIfVoiced(const std::vector<float> &frame);
    bool isCurrentFrameVoiced;
    bool isPreviousFrameVoiced;
    bool isVoiced;
    
    float countZeroCrossings(const std::vector<float> &frame);
    float getNormalizedEnergy(const std::vector<float> &frame);
    
    const float kVoicedThreshold = 0.5;
    const float kNoiseFloor = 1e-4;
};
