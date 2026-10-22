# Build fix: JUCE DSP Reverb API

The previous processor called `processStereo()` and `processMono()` on `juce::dsp::Reverb`. Those methods belong to the lower-level `juce::Reverb` class, not the DSP wrapper used by this project.

This revision prepares the DSP reverb with the current `ProcessSpec`, creates an `AudioBlock` over the active mono/stereo channels, and processes it with `juce::dsp::ProcessContextReplacing<float>`.

The local environment used to prepare this archive could not reach GitHub to download JUCE, so a full compile was not verified here. The next GitHub Actions run will perform the actual Windows build.
