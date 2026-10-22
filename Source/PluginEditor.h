#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class AbyssalReverbAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AbyssalReverbAudioProcessorEditor(AbyssalReverbAudioProcessor&);
    ~AbyssalReverbAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    AbyssalReverbAudioProcessor& processor;
    juce::Label title, subtitle, modeLabel;
    juce::Slider size, decay, damping, width, mix, drive, tone;
    juce::ComboBox mode;
    std::unique_ptr<SliderAttachment> sizeAttachment, decayAttachment, dampingAttachment,
        widthAttachment, mixAttachment, driveAttachment, toneAttachment;
    std::unique_ptr<ComboAttachment> modeAttachment;

    void setupSlider(juce::Slider&, const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbyssalReverbAudioProcessorEditor)
};
