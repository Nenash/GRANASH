#pragma once
#include <JuceHeader.h>

class EffectsChain
{
public:
    void prepare(double sampleRate, int samplesPerBlock)
    {
        reverb.reset();
        // TODO: prepare filter/delay
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        // TODO: apply filter, delay, reverb
    }

private:
    juce::Reverb reverb;
    // juce::DelayLine<float> delay; // requires custom template setup
    juce::dsp::StateVariableTPTFilter<float> filter;
};