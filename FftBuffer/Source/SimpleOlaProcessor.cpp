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
    initWindow(frameSize);
    initFftBuffer(frameSize);
}

void SimpleOlaProcessor::initFftBuffer(int frameSize)
{
    fftBuffer.resize(2, std::vector<float>(frameSize * 2, 0.0f));
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

    // Forward FFT on current and previous frame.
    for (int n = 0; n < 2; ++n)
    {
        int whichFrame = nonnegativeModulus(pNewestFrame - n, numOverlap);
        
        std::vector<float>& frameOfInterest = frameBuffers[whichFrame];
        
        if (isEffectRequested)
        {
            std::copy(frameOfInterest.begin(), frameOfInterest.end(), fftBuffer[n].begin());
            
            // Current does nothing -- fft, convert rect -> polar -> rect, ifft.
            fft.performRealOnlyForwardTransform(fftBuffer[n].data());
            convertToMagnitudeAndPhase(fftBuffer[n]);
            convertToPolar(fftBuffer[n]);
            fft.performRealOnlyInverseTransform(fftBuffer[n].data());
            
            std::copy(fftBuffer[n].begin(), fftBuffer[n].begin() + frameOfInterest.size(), frameOfInterest.begin());
        }
            
        // Window before OLA.
        for (int i = 0; i < frameOfInterest.size(); ++i)
            frameOfInterest[i] *= window[i];

    }
}

void SimpleOlaProcessor::setIsEffectRequested(bool input)
{
    bool isSame = (input == isEffectRequested);
    
    if (!isSame)
        isEffectRequested = input;
        
}

int SimpleOlaProcessor::nonnegativeModulus(int i, int n)
{
    // Positive-only modulus:
    //
    // https://stackoverflow.com/questions/14997165/fastest-way-to-get-a-positive-modulo-in-c-c
    
    return (i % n + n) % n;
}

void SimpleOlaProcessor::convertToMagnitudeAndPhase(std::vector<float> &X)
{
    for (int n = 0; n < X.size(); n += 2)
    {
        float real = X[n];
        float imaginary = X[n + 1];
        
        float magnitude = sqrt( pow(real, 2) + pow(imaginary, 2) );
        float phase = atan2(imaginary, real);
        
        X[n] = magnitude;
        X[n + 1] = phase;
    }
}

void SimpleOlaProcessor::convertToPolar(std::vector<float> &X)
{
    for (int n = 0; n < X.size(); n += 2)
    {
        float magnitude = X[n];
        float phase = X[n + 1];
        
        float real = magnitude * cos(phase);
        float imaginary = magnitude * sin(phase);
        
        X[n] = real;
        X[n + 1] = imaginary;
    }
}
