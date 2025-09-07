#pragma once
#include <JuceHeader.h>
#include "GranularEngine.h"

class GranularVoice : public juce::SynthesiserVoice
{
public:
    GranularVoice(GranularEngine* enginePtr) : engine(enginePtr) {}

    bool canPlaySound (juce::SynthesiserSound* sound) override { return true; }
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void renderNextBlock (juce::AudioBuffer<float>&, int startSample, int numSamples) override;
    void pitchWheelMoved (int newPitchWheelValue) override {}
    void controllerMoved (int controllerNumber, int newControllerValue) override {}

private:
    GranularEngine* engine;
};