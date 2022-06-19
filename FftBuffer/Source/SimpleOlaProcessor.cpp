//
//  SimpleOlaProcessor.cpp
//  FftBuffer - VST3
//
//  Created by Max Henry on 2022-06-11.
//

#include "SimpleOlaProcessor.h"

SimpleOlaProcessor::SimpleOlaProcessor()
: OlaBuffer(1024, 4), fft (10)
{
    init(1024);
}

SimpleOlaProcessor::SimpleOlaProcessor(int frameSize, int numFrames)
: OlaBuffer(frameSize, numFrames), fft(log2(frameSize))
{
    init(frameSize);
}

void SimpleOlaProcessor::init(int frameSize)
{
    isEffectRequested = false;
    fftBuffer.resize(frameSize * 2, 0.0f);
    initWindow(frameSize);
}

void SimpleOlaProcessor::initWindow(int frameSize)
{
    window.resize(frameSize);
    
    float N = static_cast<float>(frameSize);
    
    // Hamming window.
    for (int n = 0; n < frameSize; ++n)
        window[n] = 0.54 - 0.46 * cos(2.0 * M_PI * static_cast<float>(n) / N);
}

void SimpleOlaProcessor::processFrameBuffers()
{
    // Alias for clarity.
    std::vector<float>& newestFrame = frameBuffers[pNewestFrame];
    
    std::copy(newestFrame.begin(), newestFrame.end(), fftBuffer.begin());
        
    fft.performRealOnlyForwardTransform(fftBuffer.data());
    
    float numOverlapAsFloat = static_cast<float>(numOverlap);
    
    // Very simple "spectral" processing.
    const int kCutoffBin = floor(fftBuffer.size() / 32);
    
    if (isEffectRequested)
    {
        for (int n = kCutoffBin; n < fftBuffer.size(); ++n)
            fftBuffer[n] = 0.0f;
    }
    
    for (int n = 0; n < fftBuffer.size(); ++n)
        fftBuffer[n] /= numOverlapAsFloat;
    
    
    
    fft.performRealOnlyInverseTransform(fftBuffer.data());
    std::copy(fftBuffer.begin(), fftBuffer.begin() + newestFrame.size(), newestFrame.begin());
    
    // Window before OLA.
    for (int n = 0; n < newestFrame.size(); ++n)
        newestFrame[n] *= window[n];

}

void SimpleOlaProcessor::setIsEffectRequested(bool input)
{
    bool isSame = (input == isEffectRequested);
    
    if (!isSame)
        isEffectRequested = input;
        
}
