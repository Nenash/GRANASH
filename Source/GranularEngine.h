#pragma once
#include <JuceHeader.h>

class GranularEngine
{
public:
    GranularEngine() {}

    void prepare (double newSampleRate, int /*samplesPerBlock*/)
    {
        sampleRate = newSampleRate;
    }

    // Load a file into internal buffer
    bool loadFile (const juce::File& file)
    {
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor(file));

        if (reader.get() == nullptr)
            return false;

        sampleBuffer.setSize ((int) reader->numChannels, (int) reader->lengthInSamples);
        reader->read (&sampleBuffer, 0, (int) reader->lengthInSamples, 0, true, true);

        return true;
    }

    juce::AudioBuffer<float>* getSampleBuffer() { return &sampleBuffer; }
    double getSampleRate() const { return sampleRate; }

private:
    double sampleRate { 44100.0 };
    juce::AudioBuffer<float> sampleBuffer;
};
