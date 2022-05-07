//
//  LockFreeQueue.cpp
//  FftBuffer
//
//  Created by Max Henry on 2022-05-07.
//

#include "LockFreeQueue.h"

LockFreeQueue::LockFreeQueue()
{
    setSize(1024);
}

LockFreeQueue::LockFreeQueue(int bufferSize)
{
    setSize(bufferSize);
}

LockFreeQueue::~LockFreeQueue()
{
}

void LockFreeQueue::setSize(int bufferSize)
{
    
    // Clear fifo pointer stuffs.
    fifo.setTotalSize(bufferSize);
    fifo.reset();
    
    // Clear, reset data array.
    data.ensureStorageAllocated(bufferSize);
    juce::FloatVectorOperations::clear(data.getRawDataPointer(), bufferSize);
    
    while(data.size() < bufferSize)
    {
        data.add(0.0f);
    }
}

int LockFreeQueue::getSize()
{
    return fifo.getTotalSize();
}

void LockFreeQueue::writeTo(const float* writeData, int numToWrite)
{
   
    int start1, blockSize1, start2, blockSize2;
    
    fifo.prepareToWrite(numToWrite, start1, blockSize1, start2, blockSize2);
    
    if (blockSize1 > 0)
    {
        juce::FloatVectorOperations::copy(
            data.getRawDataPointer() + start1, writeData, blockSize1
        );
    }
    
    if (blockSize2 > 0)
    {
        juce::FloatVectorOperations::copy(
            data.getRawDataPointer() + start2, writeData + blockSize1, blockSize2
        );
    }
    
}

void LockFreeQueue::incrementWritePointer(int numWritten)
{
    fifo.finishedWrite(numWritten);
}

void LockFreeQueue::readFrom(float* readData, int numToRead)
{
    int start1, blockSize1, start2, blockSize2;
    
    fifo.prepareToRead(numToRead, start1, blockSize1, start2, blockSize2);
    
    if (blockSize1 > 0)
    {
        juce::FloatVectorOperations::copy(
            readData, data.getRawDataPointer() + start1, blockSize1
        );
    }
    
    if (blockSize2 > 0)
    {
        juce::FloatVectorOperations::copy(
            readData + blockSize1, data.getRawDataPointer() + start2, blockSize2
        );
    }
    
}

void LockFreeQueue::incrementReadPointer(int numRead)
{
    fifo.finishedRead(numRead);
}
