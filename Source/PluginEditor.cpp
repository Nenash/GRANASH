#include "PluginProcessor.h"
#include "PluginEditor.h"

GranularSynthAudioProcessorEditor::GranularSynthAudioProcessorEditor(GranularSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(600, 400);
}

GranularSynthAudioProcessorEditor::~GranularSynthAudioProcessorEditor()
{
}

void GranularSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Granular Synth", getLocalBounds(), juce::Justification::centred, 1);
}

void GranularSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}