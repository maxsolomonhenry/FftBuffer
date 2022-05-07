//
//  LockFreeQueue.hpp
//  FftBuffer
//
//  Created by Max Henry on 2022-05-07.
//
#pragma once

#include <JuceHeader.h>


class LockFreeQueue
{
public:
    LockFreeQueue();
    LockFreeQueue(int bufferSize);
    ~LockFreeQueue();
    
    void setSize(int bufferSize);
    int getSize();

    void writeTo(const float* writeDate, int numToWrite);
    void readFrom(float* readData, int numToRead);
    
    void incrementWritePointer(int numWritten);
    void incrementReadPointer(int numRead);
    
private:
    juce::AbstractFifo fifo{1024};
    juce::Array<float> data;
};
