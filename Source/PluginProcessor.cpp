#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GranularVoice.h"
#include "GranularSound.h"

GranularSynthAudioProcessor::GranularSynthAudioProcessor()
    : apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    // Add voices (polyphony)
    for (int i = 0; i < 8; ++i)
        synth.addVoice(new GranularVoice(&engine));

    // Add a dummy sound (always returns true)
    synth.addSound(new GranularSound());
}

GranularSynthAudioProcessor::~GranularSynthAudioProcessor() {}

void GranularSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    engine.prepare(sampleRate, samplesPerBlock);
    effects.prepare(sampleRate, samplesPerBlock);

    // TEST: load a file at startup (e.g., from desktop)
    auto testFile = juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
                        .getChildFile("test.wav");

    if (engine.loadFile(testFile))
    {
        // Pass buffer into all voices
        for (int i = 0; i < synth.getNumVoices(); ++i)
            if (auto* v = dynamic_cast<GranularVoice*>(synth.getVoice(i)))
                v->setSampleBuffer(engine.getSampleBuffer());
    }
}

void GranularSynthAudioProcessor::releaseResources() {}

void GranularSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                juce::MidiBuffer& midi)
{
    buffer.clear();
    synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());
    effects.process(buffer);
}

juce::AudioProcessorEditor* GranularSynthAudioProcessor::createEditor()
{
    return new GranularSynthAudioProcessorEditor(*this);
}

juce::AudioProcessorValueTreeState::ParameterLayout
GranularSynthAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Granular synthesis parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "grainSize", "Grain Size", 
        juce::NormalisableRange<float>(10.0f, 500.0f, 1.0f), 100.0f,
        "ms"));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "density", "Density",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f), 10.0f,
        "grains/sec"));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "position", "Position",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f,
        "%"));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "jitter", "Position Jitter",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.1f,
        "%"));
    
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "windowType", "Window Type",
        juce::StringArray{"Hanning", "Gaussian", "Triangular", "Blackman"}, 0));

    return { params.begin(), params.end() };
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GranularSynthAudioProcessor();
}