#pragma once
#include <JuceHeader.h>

class GrainCloudVisualizer : public juce::Component
{
public:
    void updateGrains (const std::vector<juce::Point<float>>& grains) { activeGrains = grains; }
    void paint (juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::aqua);
        for (auto& grain : activeGrains)
            g.fillEllipse(grain.x, grain.y, 4.0f, 4.0f);
    }

private:
    std::vector<juce::Point<float>> activeGrains;
};