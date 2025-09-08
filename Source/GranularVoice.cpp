#include "GranularVoice.h"

GranularVoice::GranularVoice(GranularEngine* eng)
    : engine(eng)
{
}

bool GranularVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<GranularSound*> (sound) != nullptr;
}

void GranularVoice::startNote(int midiNoteNumber, float velocity,
                              juce::SynthesiserSound* /*sound*/,
                              int /*currentPitchWheelPosition*/)
{
    active = true;
    gain = velocity;
    noteFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

    // reset grain positions
    grainSamplePos = 0;
    grainStart = 0;
}

void GranularVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    active = false;
    clearCurrentNote();
}

void GranularVoice::pitchWheelMoved(int /*newPitchWheelValue*/) {}
void GranularVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/) {}

void GranularVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                    int startSample,
                                    int numSamples)
{
    if (!active || engine == nullptr)
        return;

    auto* sampleBuffer = engine->getSampleBuffer();
    if (sampleBuffer == nullptr || sampleBuffer->getNumSamples() == 0)
        return;

    for (int i = 0; i < numSamples; ++i)
    {
        float sample = getNextGrainSample();

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample(ch, startSample + i, sample * gain);
    }
}

float GranularVoice::getNextGrainSample()
{
    auto* sampleBuffer = engine->getSampleBuffer();
    if (!sampleBuffer) return 0.0f;

    if (grainSamplePos >= grainSize)
    {
        // Start a new grain at random position
        int totalSamples = sampleBuffer->getNumSamples();
        if (totalSamples > grainSize)
            grainStart = juce::Random::getSystemRandom().nextInt(totalSamples - grainSize);
        else
            grainStart = 0;

        grainSamplePos = 0;
    }

    float sample = 0.0f;
    for (int ch = 0; ch < sampleBuffer->getNumChannels(); ++ch)
        sample += sampleBuffer->getSample(ch, grainStart + grainSamplePos);

    sample *= 0.5f; // normalize
    ++grainSamplePos;

    return sample;
}