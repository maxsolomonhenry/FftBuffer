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
    init(1024, 4);
}

SimpleOlaProcessor::SimpleOlaProcessor(int frameSize, int numFrames)
: OlaBuffer(frameSize, numFrames), fft(log2(frameSize))
{
    init(frameSize, numFrames);
}

void SimpleOlaProcessor::init(int frameSize, int numFrames)
{
    isEffectRequested = false;
    initWindow(frameSize, numFrames);
    initFftBuffer(frameSize);
    initPhaseAdvanceAndPhaseDelta(frameSize, numFrames);

    ctrRefresh = kNumRefreshFrames;
    
    isVoiced = false;
    isCurrentFrameVoiced = false;
    isPreviousFrameVoiced = false;
}

void SimpleOlaProcessor::initPhaseAdvanceAndPhaseDelta(int frameSize, int numFrames)
{
    // Half frame size.
    int hN = frameSize / 2;
    
    phaseAdvance.resize(hN);
    phaseDelta.resize(hN);
    
    float hopFraction = (1.0 / static_cast<float>(numFrames));
    
    for (int n = 0; n < hN; ++n)
    {
        phaseAdvance[n] = 2 * M_PI * hopFraction * n;
    }
}

void SimpleOlaProcessor::initFftBuffer(int frameSize)
{
    fftBuffer.resize(2, std::vector<float>(frameSize * 2, 0.0f));
}


void SimpleOlaProcessor::initWindow(int frameSize, int numFrames)
{
    window.resize(frameSize);
    
    float N = static_cast<float>(frameSize);
    
    // Hamming window.
    for (int n = 0; n < frameSize; ++n)
        window[n] = 0.54 - 0.46 * cos(2.0 * M_PI * static_cast<float>(n) / (N - 1.0));
    
    // Calculate overlap gain.
    int hopSize = frameSize / numFrames;
    float overlapGainLinear = 0;
    
    for (int i = 0; i < numFrames; ++i)
    {
        int idx = i * hopSize;
        
        overlapGainLinear += window[idx];
    }
    
    // Normalize window to overlap add to 1.
    for (int n = 0; n < frameSize; ++n)
        window[n] /= overlapGainLinear;
}

void SimpleOlaProcessor::processFrameBuffers()
{

    // Get spectra of newest and second-newest frame.
    for (int n = 0; n < 2; ++n)
    {
        int frameIdx = nonnegativeModulus(pNewestFrame - n, numOverlap);
        
        if (n == 0)
        {
            isCurrentFrameVoiced = determineIfVoiced(frameBuffers[frameIdx]);
            
            isVoiced = (isCurrentFrameVoiced && isPreviousFrameVoiced);
            isPreviousFrameVoiced = isCurrentFrameVoiced;
        }
        
        if (n == 1)
        {
            // Remove window of old frame before spectral analysis.
            for (int i = 0; i < frameSize; ++i)
                frameBuffers[frameIdx][i] /= window[i];
        }
            
        std::copy(frameBuffers[frameIdx].begin(), frameBuffers[frameIdx].end(), fftBuffer[n].begin());
        
        fft.performRealOnlyForwardTransform(fftBuffer[n].data());
        convertToMagnitudeAndPhase(fftBuffer[n]);
    }
    
    // Timing logic to refresh frozen sound.
    if (isRefreshRequested)
    {
        isRefreshRequested = false;
        ctrRefresh = 0;
    }
    bool isGrabbingRefreshFrames = (ctrRefresh++ < kNumRefreshFrames);
    
    std::vector<float>& newSpectrum = fftBuffer[0];
    std::vector<float>& lastSpectrum = fftBuffer[1];
    
    if (isEffectRequested && !isGrabbingRefreshFrames)
    {
        // Replace new magnitude with last magnitude.
        for (int n = 0; n < phaseDelta.size(); ++n)
        {
            int magIdx = 2 * n;
            
            newSpectrum[magIdx] = lastSpectrum[magIdx];
        }
        
        // Replace new phase with last + delta.
        for (int n = 0; n < phaseDelta.size(); ++n)
        {
            int phaseIdx = 2 * n + 1;
            
            newSpectrum[phaseIdx] = lastSpectrum[phaseIdx] + phaseDelta[n];
        }
    }
    else
    {
        // Update "freeze" phase advance.
        for (int n = 0; n < phaseDelta.size(); ++n)
        {
            int phaseIdx = 2 * n + 1;
            phaseDelta[n] = newSpectrum[phaseIdx] - lastSpectrum[phaseIdx];
        }
    }
    
    // Convert back to polar, then back to time.
    for (int n = 0; n < 2; ++n)
    {
        int frameIdx = nonnegativeModulus(pNewestFrame - n, numOverlap);
 
        convertToPolar(fftBuffer[n]);
        fft.performRealOnlyInverseTransform(fftBuffer[n].data());
                
        std::copy(fftBuffer[n].begin(), fftBuffer[n].begin() + frameBuffers[frameIdx].size(), frameBuffers[frameIdx].begin());
        
        // Window both frames.
        for (int i = 0; i < frameSize; ++i)
            frameBuffers[frameIdx][i] *= window[i];
    }
}

void SimpleOlaProcessor::setIsEffectRequested(bool input)
{
    bool isSame = (input == isEffectRequested);
    
    if (!isSame)
        isEffectRequested = input;
}

void SimpleOlaProcessor::setIsRefreshRequested(bool input)
{
    bool isSame = (input == isRefreshRequested);
    
    if (!isSame)
        isRefreshRequested = input;
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

bool SimpleOlaProcessor::determineIfVoiced(const std::vector<float> &frame)
{
    float numZeroCrossings = countZeroCrossings(frame);
    float energy = getNormalizedEnergy(frame);
    
    bool isItSilence = (energy < 20.0);
    bool isItVoiced = isItSilence ? true : (energy / numZeroCrossings > kVoicedThreshold);
    
    return isItVoiced;
}

float SimpleOlaProcessor::countZeroCrossings(const std::vector<float> &frame)
{
    float count = 0.0;
    
    bool isPreviousPositive = true;
    for (int i = 0; i < frame.size(); ++i)
    {
        // Technically this is "non-negative," but who's counting...
        bool isCurrentPositive = (frame[i] >= 0);
        
        if (isCurrentPositive != isPreviousPositive)
            count++;
        
        isPreviousPositive = isCurrentPositive;
    }
    
    return count;
}

float SimpleOlaProcessor::getNormalizedEnergy(const std::vector<float> &frame)
{
    float energy = 0.0;
    
    float maxValue = *std::max_element(frame.begin(), frame.end());
    
    for (int i = 0; i < frame.size(); ++i)
    {
        energy += frame[i] * frame[i];
    }
    
    // Only normalize non-silent(ish) frames.
    if (maxValue > kNoiseFloor)
        energy /= maxValue;
    
    return energy;
}

bool SimpleOlaProcessor::getIsVoiced()
{
    return isVoiced;
}
