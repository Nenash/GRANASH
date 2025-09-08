#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class GranularSynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    GranularSynthAudioProcessorEditor(GranularSynthAudioProcessor&);
    ~GranularSynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    GranularSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularSynthAudioProcessorEditor)
};