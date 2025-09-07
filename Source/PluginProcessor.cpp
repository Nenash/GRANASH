#include "PluginProcessor.h"
#include "PluginEditor.h"

GranularSynthAudioProcessor::GranularSynthAudioProcessor()
    : apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    synth.clearVoices();
    for (int i = 0; i < 8; ++i)
        synth.addVoice(new GranularVoice(&engine));

    synth.clearSounds();
    synth.addSound(new juce::SamplerSound("GranularSound", *engine.getSampleBuffer(), 
                                          juce::BigInteger().setRange(0,128,true), 60, 0.0, 0.0, 10.0));
}

GranularSynthAudioProcessor::~GranularSynthAudioProcessor() {}

void GranularSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    engine.prepare(sampleRate, samplesPerBlock);
    effects.prepare(sampleRate, samplesPerBlock);
}

void GranularSynthAudioProcessor::releaseResources() {}

void GranularSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    buffer.clear();
    synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());
    effects.process(buffer);
}

juce::AudioProcessorEditor* GranularSynthAudioProcessor::createEditor()
{
    return new PluginEditor(*this);
}

juce::AudioProcessorValueTreeState::ParameterLayout GranularSynthAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    // TODO: Add knobs/sliders (grain size, density, pitch, etc.)
    return { params.begin(), params.end() };
}
