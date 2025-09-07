#pragma once
#include <JuceHeader.h>

struct GrainParameters
{
    float grainSizeMs {100.0f};
    float density {10.0f};
    float position {0.5f};
    float pitch {0.0f};
};

class GranularEngine
{
public:
    void prepare(double sampleRate, int samplesPerBlock);
    void loadSample(const juce::File& file);
    juce::AudioBuffer<float>* getSampleBuffer() { return &sampleBuffer; }
    GrainParameters params;

private:
    juce::AudioBuffer<float> sampleBuffer;
};