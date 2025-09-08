#pragma once
#include <JuceHeader.h>

class GranularSound : public juce::SynthesiserSound
{
public:
    GranularSound() {}

    bool appliesToNote (int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel (int /*midiChannel*/) override { return true; }
};
