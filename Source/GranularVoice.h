#pragma once
#include <JuceHeader.h>
#include "GranularSound.h"
#include "Grain.h"

class GranularEngine;

class GranularVoice : public juce::SynthesiserVoice
{
public:
    GranularVoice(GranularEngine* engine);

    // === Required by SynthesiserVoice ===
    bool canPlaySound(juce::SynthesiserSound* sound) override;

    void startNote(int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* sound,
                    int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                          int startSample,
                          int numSamples) override;

    // === Grain Engine Parameters ===
    void setGrainSize(float sizeMs) { grainSizeMs = sizeMs; }
    void setGrainDensity(float grainsPerSecond) { density = grainsPerSecond; }
    void setPitchRatio(float ratio) { pitchRatio = ratio; }
    void setPosition(float pos) { samplePosition = juce::jlimit(0.0f, 1.0f, pos); }
    void setPositionJitter(float jitter) { positionJitter = juce::jlimit(0.0f, 1.0f, jitter); }
    void setWindowType(WindowType type) { windowType = type; }

private:
    void updateGrainScheduling(int numSamples);
    void triggerNewGrain();
    float getRandomizedPosition();
    
    // === State ===
    bool isNoteActive { false };
    float noteVelocity { 0.0f };
    float noteFrequency { 440.0f };
    int midiNote { 60 };

    GranularEngine* engine { nullptr };

    // === Grain Parameters ===
    float grainSizeMs { 100.0f };        // Grain size in milliseconds
    float density { 10.0f };             // Grains per second
    float pitchRatio { 1.0f };           // Pitch shifting ratio
    float samplePosition { 0.0f };       // Position in sample (0.0 to 1.0)
    float positionJitter { 0.1f };       // Random position variation
    WindowType windowType { WindowType::Hanning };
    
    // === Grain Management ===
    static constexpr int maxGrains = 32;
    std::array<Grain, maxGrains> grains;
    int nextGrainIndex { 0 };
    
    // === Scheduling ===
    double sampleRate { 44100.0 };
    float samplesUntilNextGrain { 0.0f };
    juce::Random random;
    
    // === Smoothing ===
    juce::LinearSmoothedValue<float> pitchSmoothed;
    juce::LinearSmoothedValue<float> gainSmoothed;
};