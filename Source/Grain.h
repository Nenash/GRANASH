#pragma once
#include <JuceHeader.h>

struct Grain
{
    int startSample = 0;
    int lengthSamples = 0;
    float pitch = 1.0f;
    juce::ADSR envelope;
};