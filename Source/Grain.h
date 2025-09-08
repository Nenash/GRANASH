#pragma once
#include <JuceHeader.h>

enum class WindowType
{
    Hanning,
    Gaussian,
    Triangular,
    Blackman
};

class GrainWindow
{
public:
    static float getWindowValue(WindowType type, float position)
    {
        // position should be 0.0 to 1.0
        position = juce::jlimit(0.0f, 1.0f, position);
        
        switch (type)
        {
            case WindowType::Hanning:
                return 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * position));
                
            case WindowType::Gaussian:
            {
                float sigma = 0.4f;
                float x = (position - 0.5f) / sigma;
                return std::exp(-0.5f * x * x);
            }
            
            case WindowType::Triangular:
                return 1.0f - std::abs(2.0f * position - 1.0f);
                
            case WindowType::Blackman:
            {
                float a0 = 0.42f;
                float a1 = 0.5f;
                float a2 = 0.08f;
                float twoPiPos = 2.0f * juce::MathConstants<float>::pi * position;
                return a0 - a1 * std::cos(twoPiPos) + a2 * std::cos(2.0f * twoPiPos);
            }
        }
        return 1.0f;
    }
};

struct Grain
{
    // Playback state
    bool isActive = false;
    int currentPosition = 0;
    int lengthSamples = 0;
    
    // Sample source
    int startSample = 0;
    float pitch = 1.0f;
    float gain = 1.0f;
    
    // Windowing
    WindowType windowType = WindowType::Hanning;
    
    // Envelope (for additional shaping beyond windowing)
    juce::ADSR envelope;
    juce::ADSR::Parameters envelopeParams;
    
    void reset()
    {
        isActive = false;
        currentPosition = 0;
        envelope.reset();
    }
    
    void trigger(int startPos, int length, float pitchRatio, float grainGain, WindowType window)
    {
        startSample = startPos;
        lengthSamples = length;
        pitch = pitchRatio;
        gain = grainGain;
        windowType = window;
        currentPosition = 0;
        isActive = true;
        
        // Set up envelope parameters for grain (quick attack/release)
        envelopeParams.attack = 0.01f;   // 10ms attack
        envelopeParams.decay = 0.0f;
        envelopeParams.sustain = 1.0f;
        envelopeParams.release = 0.01f;  // 10ms release
        
        envelope.setParameters(envelopeParams);
        envelope.noteOn();
    }
    
    float getNextSample(const juce::AudioBuffer<float>& sampleBuffer, double sampleRate)
    {
        if (!isActive || currentPosition >= lengthSamples)
        {
            if (isActive)
            {
                envelope.noteOff();
                isActive = false;
            }
            return 0.0f;
        }
        
        // Calculate window position (0.0 to 1.0)
        float windowPos = static_cast<float>(currentPosition) / static_cast<float>(lengthSamples - 1);
        
        // Get window amplitude
        float windowAmp = GrainWindow::getWindowValue(windowType, windowPos);
        
        // Get envelope amplitude
        float envelopeAmp = envelope.getNextSample();
        
        // Calculate source position with pitch shifting
        float sourcePos = startSample + (currentPosition * pitch);
        
        // Interpolate sample from buffer
        float sample = 0.0f;
        if (sourcePos >= 0 && sourcePos < sampleBuffer.getNumSamples() - 1)
        {
            // Linear interpolation
            int pos = static_cast<int>(sourcePos);
            float frac = sourcePos - pos;
            
            for (int ch = 0; ch < sampleBuffer.getNumChannels(); ++ch)
            {
                float s1 = sampleBuffer.getSample(ch, pos);
                float s2 = sampleBuffer.getSample(ch, pos + 1);
                sample += s1 + frac * (s2 - s1);
            }
            
            // Average channels if multi-channel
            if (sampleBuffer.getNumChannels() > 1)
                sample /= sampleBuffer.getNumChannels();
        }
        
        currentPosition++;
        
        // Apply windowing, envelope, and gain
        return sample * windowAmp * envelopeAmp * gain;
    }
    
    bool isFinished() const
    {
        return !isActive;
    }
};