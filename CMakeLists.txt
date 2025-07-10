cmake_minimum_required(VERSION 3.15)

project(StutterAndHold VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

# Find JUCE
find_package(PkgConfig REQUIRED)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Add JUCE as a subdirectory if it exists locally, otherwise use find_package
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
    add_subdirectory(../JUCE JUCE)
else()
    find_package(JUCE CONFIG REQUIRED)
endif()

# Plugin sources
set(PLUGIN_SOURCES
    FftBuffer/Source/OlaBuffer.cpp
    FftBuffer/Source/PluginEditor.cpp
    FftBuffer/Source/PluginProcessor.cpp
    FftBuffer/Source/SimpleOlaProcessor.cpp
)

# Create plugin target
juce_add_plugin(StutterAndHold
    COMPANY_NAME "Pass By Reference"
    COMPANY_EMAIL "max.henry@mail.mcgill.ca"
    PLUGIN_MANUFACTURER_CODE PbRf
    PLUGIN_CODE StHd
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "StutterAndHold"
    MICROPHONE_PERMISSION_ENABLED TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_DESCRIPTION "Audio stutter and hold effect"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    PLUGIN_AAX_IDENTIFIER com.passbyreference.stutterandhold
    PLUGIN_AU_MAIN_TYPE kAudioUnitType_Effect
)

# Add sources to target
target_sources(StutterAndHold PRIVATE ${PLUGIN_SOURCES})

# Link JUCE modules
target_link_libraries(StutterAndHold
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compile definitions
target_compile_definitions(StutterAndHold
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_STRICT_REFCOUNTEDPOINTER=1
)

# Include directories
target_include_directories(StutterAndHold
    PRIVATE
        FftBuffer/Source
        FftBuffer/JuceLibraryCode
)