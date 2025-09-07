#pragma once
#include <JuceHeader.h>

class ParameterControls : public juce::Component
{
public:
    ParameterControls() {}
    void paint (juce::Graphics& g) override { g.fillAll(juce::Colours::darkblue); }
};