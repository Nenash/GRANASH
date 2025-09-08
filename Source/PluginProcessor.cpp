#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GranularVoice.h"
#include "GranularSound.h"
#include "GranularEngine.h"
#include "EffectsChain.h"

class GranularSynthAudioProcessor : public juce::AudioProcessor
{
public:
    GranularSynthAudioProcessor();
    ~GranularSynthAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "GranularSynth"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override {}
    void setStateInformation (const void* data, int sizeInBytes) override {}

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::Synthesiser synth;
    GranularEngine engine;     // manages sample buffer & grain logic
    EffectsChain effects;      // wrapper for reverb/delay/filter

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GranularSynthAudioProcessor)
};

// =======================================================

GranularSynthAudioProcessor::GranularSynthAudioProcessor()
    : apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    // Add voices (polyphony)
    for (int i = 0; i < 8; ++i)
        synth.addVoice (new GranularVoice(&engine));

    // Add a dummy sound (always returns true)
    synth.addSound (new GranularSound());
}

GranularSynthAudioProcessor::~GranularSynthAudioProcessor() {}

void GranularSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    engine.prepare (sampleRate, samplesPerBlock);
    effects.prepare (sampleRate, samplesPerBlock);
}

void GranularSynthAudioProcessor::releaseResources() {}

void GranularSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                juce::MidiBuffer& midi)
{
    buffer.clear();
    synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());
    effects.process (buffer);
}

juce::AudioProcessorEditor* GranularSynthAudioProcessor::createEditor()
{
    return new PluginEditor(*this);
}

juce::AudioProcessorValueTreeState::ParameterLayout
GranularSynthAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Example params
    params.push_back (std::make_unique<juce::AudioParameterFloat>("grainSize", "Grain Size",
                                                                  1.0f, 500.0f, 100.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("density", "Density",
                                                                  1.0f, 100.0f, 20.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("pitch", "Pitch",
                                                                  -24.0f, 24.0f, 0.0f));

    return { params.begin(), params.end() };
}
