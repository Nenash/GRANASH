#pragma once
#include <JuceHeader.h>

class GranularSynthAudioProcessor;

class PluginEditor : public juce::AudioProcessorEditor
{
public:
    PluginEditor (GranularSynthAudioProcessor&);
    ~PluginEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    GranularSynthAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};