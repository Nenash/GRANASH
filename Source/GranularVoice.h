#pragma once
#include <JuceHeader.h>

class GranularVoice : public juce::SynthesiserVoice
{
public:
    GranularVoice() {}

    // === Required by SynthesiserVoice ===
    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<juce::SamplerSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* sound,
                    int currentPitchWheelPosition) override
    {
        // Reset grain state when a note is triggered
        currentSamplePos = 0;
        active = true;
        noteFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        gain = velocity;
    }

    void stopNote (float velocity, bool allowTailOff) override
    {
        active = false;
        clearCurrentNote();
    }

    void pitchWheelMoved (int newPitchWheelValue) override {}
    void controllerMoved (int controllerNumber, int newControllerValue) override {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                          int startSample,
                          int numSamples) override
    {
        if (! active || sampleBuffer == nullptr)
            return;

        for (int i = 0; i < numSamples; ++i)
        {
            // Get current grain sample
            float sample = getNextGrainSample();

            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                outputBuffer.addSample (channel, startSample + i, sample * gain);
        }
    }

    // === Grain Engine Basics ===
    void setSampleBuffer (juce::AudioBuffer<float>* buffer) { sampleBuffer = buffer; }

    void setGrainSize (int sizeInSamples) { grainSize = sizeInSamples; }
    void setGrainDensity (float grainsPerSecond) { density = grainsPerSecond; }
    void setPitchRatio (float ratio) { pitchRatio = ratio; }

private:
    float getNextGrainSample()
    {
        if (sampleBuffer == nullptr) return 0.0f;

        // Wrap grain playback
        if (grainSamplePos >= grainSize)
        {
            // Start a new grain at random position (for now)
            grainStart = juce::Random::getSystemRandom().nextInt (sampleBuffer->getNumSamples() - grainSize);
            grainSamplePos = 0;
        }

        // Read from sample buffer
        float sample = 0.0f;
        for (int ch = 0; ch < sampleBuffer->getNumChannels(); ++ch)
            sample += sampleBuffer->getSample(ch, grainStart + grainSamplePos);

        sample *= 0.5f; // normalize
        grainSamplePos++;
        return sample;
    }

    // === State ===
    bool active { false };
    float gain { 0.0f };
    float noteFrequency { 0.0f };

    juce::AudioBuffer<float>* sampleBuffer { nullptr };

    int grainSize { 2048 };      // in samples
    int grainStart { 0 };
    int grainSamplePos { 0 };
    float density { 10.0f };     // grains/sec
    float pitchRatio { 1.0f };   // pitch shifting
    int currentSamplePos { 0 };
};
