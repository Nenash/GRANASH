#pragma once
#include <JuceHeader.h>
#include "GranularEngine.h"
#include "GranularVoice.h"
#include "EffectsChain.h"

class GranularSynthAudioProcessor : public juce::AudioProcessor
{
public:
    GranularSynthAudioProcessor();
    ~GranularSynthAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // Basic plugin info
    const juce::String getName() const override { return JucePlugin_Name; }
    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override;

    // Parameters
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    juce::Synthesiser synth;
    GranularEngine engine;
    EffectsChain effects;
    juce::AudioProcessorValueTreeState apvts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GranularSynthAudioProcessor)
};