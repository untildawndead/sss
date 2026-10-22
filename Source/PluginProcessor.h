#pragma once
#include <JuceHeader.h>

class AbyssalReverbAudioProcessor final : public juce::AudioProcessor
{
public:
    AbyssalReverbAudioProcessor();
    ~AbyssalReverbAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 20.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;
    juce::AudioBuffer<float> dryBuffer;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> wetSmoothed;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> driveSmoothed;
    juce::dsp::StateVariableTPTFilter<float> toneFilterL, toneFilterR;
    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbyssalReverbAudioProcessor)
};
