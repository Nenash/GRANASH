#pragma once
#include <JuceHeader.h>
#include "GranularSound.h"

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

    // === Grain Engine Basics ===
    void setSampleBuffer(juce::AudioBuffer<float>* buffer) { sampleBuffer = buffer; }

    void setGrainSize(int sizeInSamples) { grainSize = sizeInSamples; }
    void setGrainDensity(float grainsPerSecond) { density = grainsPerSecond; }
    void setPitchRatio(float ratio) { pitchRatio = ratio; }

private:
    float getNextGrainSample();

    // === State ===
    bool active { false };
    float gain { 0.0f };
    float noteFrequency { 0.0f };

    GranularEngine* engine { nullptr };
    juce::AudioBuffer<float>* sampleBuffer { nullptr };

    int grainSize { 2048 };      // in samples
    int grainStart { 0 };
    int grainSamplePos { 0 };
    float density { 10.0f };     // grains/sec
    float pitchRatio { 1.0f };   // pitch shifting
    int currentSamplePos { 0 };
};