# FFT Buffer

Experiments in JUCE to build an overlap add framework capable of doing spectral processing. 

## Basic use

Here's how it's working for now: inherit from the `OlaBuffer` object, which takes care of all the overlap adding. You'll have to implement a `processFrameBuffers` method, which is called every `hopSize` samples (i.e., whenever an overlapping frame is filled up). For an example of this, see the `SimpleOlaProcessor` class, which makes a very silly lowpass filter in the spectral domain. (Silly in that it implements an ideal lowpass filter, which leads to much time aliasing -- this clickiness is mitigated by re-windowing the ifft'd frame with a synthesis window, which is very convenient to implement in this setup!).

## Implementation details

The OLA buffer object generates and continually updates a two-dimensional vector of `numOverlap` overlapping frames. Every time a frame is filled, the object calls the `processFrameBuffers` method. Every `hopSize` samples, the overlapping frames are summed up and stored in an internal synthesis buffer.

The easiest way to use this object in your code is probably to use the `processBlock` convenience method, which will input a block to the necessary buffers, due the OLA calculations, and output a block of (slightly delayed) processed, overlapped-added samples.

## Much ado.

The plugin is still mono, needs a GUI, and many such things. But the logic is pretty sound. Hurray.
