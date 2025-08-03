# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**StutterAndHold** is a JUCE-based audio plugin that implements a "timbral tremolo" effect - essentially combining a freeze pedal and tremolo using phase vocoder technology. The plugin captures and stutters audio frames while providing amplitude envelope following.

## Build System

This project uses **CMake** as the primary build system:

### Build Commands
```bash
# Configure build (from project root)
cmake -B build

# Build the plugin
cmake --build build

# Build specific target
cmake --build build --target StutterAndHold
```

### Build Outputs
- Plugin formats: AU, VST3, Standalone
- Target name: `StutterAndHold`
- Company: "Pass By Reference"

## Core Architecture

### Key Classes and Inheritance

1. **OlaBuffer** (`FftBuffer/Source/OlaBuffer.h/cpp`)
   - Base class for overlap-add processing
   - Manages frame buffers, delay buffers, and overlap-add synthesis
   - Virtual `processFrameBuffers()` method for subclass implementation
   - Core method: `process(float& x)` - processes samples one by one

2. **SimpleOlaProcessor** (`FftBuffer/Source/SimpleOlaProcessor.h/cpp`)
   - Inherits from `OlaBuffer`
   - Implements phase vocoder functionality using JUCE FFT
   - Handles polar/rectangular coordinate conversion
   - Manages stuttering logic and frame refresh

3. **FftBufferAudioProcessor** (`FftBuffer/Source/PluginProcessor.h/cpp`)
   - Main JUCE plugin processor
   - Contains `std::vector<SimpleOlaProcessor> olaProcessor` for multi-channel processing
   - Manages plugin parameters via `juce::AudioProcessorValueTreeState`
   - Handles tempo sync and stutter rate calculations

4. **Transport** (`FftBuffer/Source/Transport.h`)
   - Utility struct for DAW tempo sync
   - Tracks PPQ positions for beat-synchronized stuttering

### Parameter System
The plugin uses JUCE's `AudioProcessorValueTreeState` for parameter management:
- Freeze on/off
- Tempo sync
- Manual refresh trigger
- Stutter rate (Hz or beats)
- Envelope depth
- Dry/wet mix

### Audio Processing Flow
1. Audio input → `FftBufferAudioProcessor::processBlock()`
2. Per-channel processing via `SimpleOlaProcessor` instances
3. Sample-by-sample processing through `OlaBuffer::process()`
4. Frame-based spectral processing in `SimpleOlaProcessor::processFrameBuffers()`
5. Overlap-add synthesis back to time domain

## Project Structure

```
FftBuffer/
├── Source/                    # Main source code
│   ├── PluginProcessor.*     # Main JUCE processor
│   ├── PluginEditor.*        # GUI implementation
│   ├── OlaBuffer.*           # Base overlap-add class
│   ├── SimpleOlaProcessor.*  # Phase vocoder implementation
│   └── Transport.h           # Tempo sync utilities
├── Builds/MacOSX/            # Xcode project files
├── JuceLibraryCode/          # Generated JUCE wrapper code
└── FftBuffer.jucer           # JUCE project file
```

## Development Notes

### JUCE Integration
- Uses JUCE 7+ with modern CMake integration
- Requires JUCE modules: audio_basics, audio_processors, dsp, gui_basics, etc.
- Generated code in `JuceLibraryCode/` should not be manually edited

### Audio Processing Constants
Key constants defined in `PluginProcessor.h`:
- `kMaxStutterRateHz = 12.0`
- `kEnvelopeDelaySamples = 1100`
- `kEqualPowerCoefficient = 0.70710678118`

### MATLAB Prototyping
The `prototyping/` directory contains MATLAB scripts used for algorithm development:
- `OlaBuffer.m` - Original overlap-add implementation
- `EnvelopeFollower.m` - Envelope detection prototyping

## Testing and Development

The project includes a standalone application for testing. Build outputs are automatically copied to system plugin directories when `COPY_PLUGIN_AFTER_BUILD` is enabled.

Use the GUI parameters or host automation to test different stutter rates, envelope depths, and tempo sync behaviors.