# Granular Synthesizer VST3 Plugin

## Project Overview
A **granular synthesizer VST3 plugin** built with the **JUCE framework (v7+)**, designed for music producers and sound designers.  
It processes single audio samples (WAV/AIFF) with a focus on flexibility, real-time MIDI control, built-in effects, and a modern, intuitive UI.  
The plugin prioritizes **usability, performance, and creative potential** for VST3-compatible DAWs (Ableton Live, Logic, Cubase, Reaper, etc.).

- **Target Platforms**: VST3 for Windows and macOS (expandable to AU/AAX via JUCE).
- **Dependencies**: `juce_audio_basics`, `juce_audio_devices`, `juce_audio_formats`, `juce_audio_processors`,  
  `juce_audio_utils`, `juce_gui_basics`, `juce_gui_extra`.

---

## Core Features

### 1. Sample Handling
- Load single WAV/AIFF files via file browser or drag-and-drop.
- Display sample metadata (length, sample rate).
- Waveform display with navigation, zoom, and scrubbing (`juce::AudioThumbnail`).
- Basic management: clear/reload sample.

### 2. Granular Engine
- Grain playback (10–500 ms) from loaded sample.
- Adjustable parameters:
  - **Grain size** (10–500 ms)
  - **Density** (1–100 grains/sec)
  - **Position** (0–100% of sample) with randomization/jitter
  - **Envelope shape**: Hanning or Gaussian (to prevent clicks)
- Modes: synchronous (grid-based) or asynchronous (random onset).
- Pitch shifting: ±24 semitones per grain (`juce::LagrangeInterpolator`).
- Time stretching: independent speed control via grain overlap/spacing.
- Grain cloud visualization: real-time 2D scatter plot (x: position, y: pitch/density).

### 3. Effects Processing
- **Reverb**: `juce::Reverb` with room size, damping, wet/dry.
- **Delay**: `juce::DelayLine` with feedback + tempo sync.
- **Filter**: `juce::StateVariableTPTFilter` (LP/HP/BP).
- Effects chain post-granular, with bypass toggles.

### 4. Controls & Modulation
- Rotary knobs for synthesis/effect parameters.
- Sliders and XY-pads for expressive modulation.
- MIDI input:
  - Notes trigger grains (e.g., C3 = original pitch).
  - Velocity maps to grain volume.
  - MIDI CC (e.g., mod wheel = jitter).
- Polyphony: 8–16 voices (balanced CPU use).

---

## User Interface
- **Layout**: Clean, modern design (`juce::TabbedComponent` for Sample, Granular, Effects, MIDI).
- **Waveform Display**: central, zoom/scrub (`juce::AudioThumbnailComponent`).
- **Grain Cloud Overlay**: custom component showing active grains (updated via timer).
- **Parameter Section**: knobs, sliders, XY-pads.
- **Effects Section**: dedicated controls + on/off toggles.
- **Real-time Visual Feedback**: waveform + grain activity.

---

## Technical Architecture

- **PluginProcessor** (`juce::AudioProcessor`)  
  Handles audio rendering, MIDI, and granular engine. Uses `juce::Synthesiser` for polyphony.

- **GranularVoice** (`juce::SynthesiserVoice`)  
  Renders individual grains (extraction, pitch, envelope).

- **GranularEngine**  
  Manages sample buffer, grain windowing, randomization, effects routing.

- **PluginEditor** (`juce::AudioProcessorEditor`)  
  GUI components: waveform, cloud, knobs, effects panels.

- **Parameter Management**:  
  `juce::AudioProcessorValueTreeState` (APVTS) for thread-safe parameters, DAW automation, presets.

**Data Flow:**  
Load sample → store in `juce::AudioBuffer` → MIDI note → allocate voice → extract grain → pitch/time/effects → mix output.

---

## Development Roadmap

1. **Setup (1–2 days)**  
   Projucer project, CMake, JUCE modules.

2. **Sample Handling (2–3 days)**  
   File chooser, WAV/AIFF loading, waveform display.

3. **Granular Engine (4–6 days)**  
   Grain extraction, windowing, density/jitter, sync/async modes.

4. **Pitch & Time (2–3 days)**  
   Lagrange interpolation for pitch, overlap control for stretch.

5. **Effects Integration (2 days)**  
   Add reverb, delay, filter chain with bypass.

6. **UI & Visualization (3–4 days)**  
   Tabbed interface, knobs/sliders, XY pads, grain cloud overlay.

7. **MIDI & Polyphony (2 days)**  
   `juce::Synthesiser`, note handling, CC modulation.

8. **Testing & Optimization (3–5 days)**  
   Stability in DAWs, CPU optimization (grain caps, lock-free queues, `LinearSmoothedValue`), preset system.

**Estimated Time**: ~2–4 weeks for a functional prototype.

---

## Challenges & Solutions
- **Performance (high grain counts)** → cap grains, optimize interpolation.  
- **Clicks/artifacts** → use envelopes, parameter smoothing.  
- **Polyphony overload** → note stealing, voice limits.  
- **Sample rate mismatches** → resize buffers in `prepareToPlay()`.  
- **Cross-platform quirks** → JUCE abstractions, test on Win/macOS.

---

## Future Expansion
- Multi-sample layering.
- Advanced modulation (LFOs, envelopes).
- Randomization/generative controls.
- Preset browser with factory + user banks.

---

## Resources
- **JUCE Docs** & tutorials.  
- Open-source: *GRNLR* plugin (reference).  
- JUCE Forum (granular threads).  
- Theory: *Microsound* by Curtis Roads.

---
## Target Outcome
A **versatile, user-friendly granular synthesizer** that empowers producers and sound designers with innovative textures, intuitive workflow, and seamless DAW integration.
