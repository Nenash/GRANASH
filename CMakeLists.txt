cmake_minimum_required(VERSION 3.15)

project(GranularSynth VERSION 0.1.0)

# Set path to JUCE
set(JUCE_DIR "/users/artemnenashev/JUCE")

add_subdirectory(${JUCE_DIR} JUCE)

juce_add_plugin(GranularSynth
    COMPANY_NAME "YourName"
    PLUGIN_NAME "GranularSynth"
    BUNDLE_ID com.yourname.granularsynth
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Demo
    PLUGIN_CODE Gsyn
    FORMATS VST3
    PRODUCT_NAME "GranularSynth"
)

target_sources(GranularSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/GranularEngine.cpp
    Source/GranularVoice.cpp
    Source/Grain.cpp
    Source/EffectsChain.cpp
    Source/WaveformDisplay.cpp
    Source/GrainCloudVisualizer.cpp
    Source/ParameterControls.cpp
)

target_compile_features(GranularSynth PRIVATE cxx_std_17)

target_link_libraries(GranularSynth PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_dsp
)