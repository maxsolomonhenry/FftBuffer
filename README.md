# FFT Buffer

Experiments in JUCE to build an overlap add framework capable of doing spectral processing. 

## Basic use

At the moment you're going to want to inherit from the `OlaBuffer` object, and implement a `processFrameBuffers()` method. For an example of this, see `SimpleOlaProcessor` which makes a very silly lowpass filter in the spectral domain.

## Implementation details

The OLA buffer object generates and conitinually updates a two-dimensional vector of overlapping frames. Every time a frame is filled, the object calls the `processFrameBuffers` method. Every hop-size worth of samples, the overlapping frames are summed up and output.

The easiest way to use this object in your code is probably to use the `processBlock` convenience method, which will input a block to the necessary buffers, due the OLA calculations, and output a block of (slightly delayed) processed, overlapped-added samples.
