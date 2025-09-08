#include "GranularVoice.h"
#include "GranularEngine.h"

GranularVoice::GranularVoice(GranularEngine* eng)
    : engine(eng)
{
    // Initialize smoothed values
    pitchSmoothed.reset(44100.0, 0.05); // 50ms smoothing
    gainSmoothed.reset(44100.0, 0.05);
}

bool GranularVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<GranularSound*>(sound) != nullptr;
}

void GranularVoice::startNote(int midiNoteNumber, float velocity,
                              juce::SynthesiserSound* /*sound*/,
                              int /*currentPitchWheelPosition*/)
{
    isNoteActive = true;
    noteVelocity = velocity;
    midiNote = midiNoteNumber;
    noteFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
    // Calculate pitch ratio based on MIDI note (C4 = 60 = original pitch)
    float semitoneOffset = midiNoteNumber - 60;
    pitchRatio = std::pow(2.0f, semitoneOffset / 12.0f);
    
    // Set smoothed values
    pitchSmoothed.setTargetValue(pitchRatio);
    gainSmoothed.setTargetValue(velocity);
    
    // Reset grain scheduling
    samplesUntilNextGrain = 0.0f;
    nextGrainIndex = 0;
    
    // Reset all grains
    for (auto& grain : grains)
        grain.reset();
    
    // Get sample rate from engine
    if (engine)
        sampleRate = engine->getSampleRate();
}

void GranularVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        // Let grains finish naturally
        isNoteActive = false;
        gainSmoothed.setTargetValue(0.0f);
    }
    else
    {
        // Stop immediately
        isNoteActive = false;
        for (auto& grain : grains)
            grain.reset();
        clearCurrentNote();
    }
}

void GranularVoice::pitchWheelMoved(int newPitchWheelValue)
{
    // Convert pitch wheel to ratio (-2 to +2 semitones)
    float pitchBend = (newPitchWheelValue - 8192) / 8192.0f * 2.0f;
    float bendRatio = std::pow(2.0f, pitchBend / 12.0f);
    
    // Calculate base pitch ratio from MIDI note
    float semitoneOffset = midiNote - 60;
    float basePitchRatio = std::pow(2.0f, semitoneOffset / 12.0f);
    
    pitchSmoothed.setTargetValue(basePitchRatio * bendRatio);
}

void GranularVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    float ccValue = newControllerValue / 127.0f;
    
    switch (controllerNumber)
    {
        case 1: // Mod wheel - position jitter
            setPositionJitter(ccValue);
            break;
        case 74: // Filter cutoff - grain size
            setGrainSize(20.0f + ccValue * 480.0f); // 20ms to 500ms
            break;
        case 71: // Resonance - density
            setGrainDensity(1.0f + ccValue * 99.0f); // 1 to 100 grains/sec
            break;
    }
}

void GranularVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                    int startSample,
                                    int numSamples)
{
    if (!engine || !engine->getSampleBuffer())
        return;
    
    auto* sampleBuffer = engine->getSampleBuffer();
    if (sampleBuffer->getNumSamples() == 0)
        return;
    
    // Update grain scheduling
    updateGrainScheduling(numSamples);
    
    // Render each sample
    for (int i = 0; i < numSamples; ++i)
    {
        float outputSample = 0.0f;
        
        // Sum all active grains
        for (auto& grain : grains)
        {
            if (grain.isActive)
            {
                outputSample += grain.getNextSample(*sampleBuffer, sampleRate);
            }
        }
        
        // Apply smoothed gain
        outputSample *= gainSmoothed.getNextValue();
        
        // Write to output buffer
        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
        {
            outputBuffer.addSample(ch, startSample + i, outputSample);
        }
    }
    
    // Check if voice should be cleared
    if (!isNoteActive)
    {
        bool anyGrainsActive = false;
        for (const auto& grain : grains)
        {
            if (grain.isActive)
            {
                anyGrainsActive = true;
                break;
            }
        }
        
        if (!anyGrainsActive && gainSmoothed.getCurrentValue() < 0.001f)
        {
            clearCurrentNote();
        }
    }
}

void GranularVoice::updateGrainScheduling(int numSamples)
{
    if (!isNoteActive || density <= 0.0f)
        return;
    
    // Calculate samples between grains
    float samplesPerGrain = sampleRate / density;
    
    for (int i = 0; i < numSamples; ++i)
    {
        samplesUntilNextGrain -= 1.0f;
        
        if (samplesUntilNextGrain <= 0.0f)
        {
            triggerNewGrain();
            samplesUntilNextGrain += samplesPerGrain;
        }
    }
}

void GranularVoice::triggerNewGrain()
{
    auto* sampleBuffer = engine->getSampleBuffer();
    if (!sampleBuffer || sampleBuffer->getNumSamples() == 0)
        return;
    
    // Find next available grain slot
    Grain* availableGrain = nullptr;
    for (int i = 0; i < maxGrains; ++i)
    {
        int index = (nextGrainIndex + i) % maxGrains;
        if (!grains[index].isActive)
        {
            availableGrain = &grains[index];
            nextGrainIndex = (index + 1) % maxGrains;
            break;
        }
    }
    
    if (!availableGrain)
    {
        // Force reuse oldest grain if all slots are full
        availableGrain = &grains[nextGrainIndex];
        nextGrainIndex = (nextGrainIndex + 1) % maxGrains;
    }
    
    // Calculate grain parameters
    int grainSizeSamples = static_cast<int>(grainSizeMs * 0.001f * sampleRate);
    grainSizeSamples = juce::jlimit(64, sampleBuffer->getNumSamples(), grainSizeSamples);
    
    int startPos = static_cast<int>(getRandomizedPosition() * (sampleBuffer->getNumSamples() - grainSizeSamples));
    startPos = juce::jlimit(0, sampleBuffer->getNumSamples() - grainSizeSamples, startPos);
    
    float currentPitch = pitchSmoothed.getCurrentValue();
    float grainGain = noteVelocity * 0.3f; // Scale down to prevent clipping
    
    // Trigger the grain
    availableGrain->trigger(startPos, grainSizeSamples, currentPitch, grainGain, windowType);
}

float GranularVoice::getRandomizedPosition()
{
    if (positionJitter <= 0.0f)
        return samplePosition;
    
    float jitterAmount = (random.nextFloat() - 0.5f) * 2.0f * positionJitter;
    float randomizedPos = samplePosition + jitterAmount;
    
    return juce::jlimit(0.0f, 1.0f, randomizedPos);
}