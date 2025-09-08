// Source/PluginEditor.cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"

GranularSynthAudioProcessorEditor::GranularSynthAudioProcessorEditor(GranularSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(600, 400);
}

void GranularSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void GranularSynthAudioProcessorEditor::resized()
{
}