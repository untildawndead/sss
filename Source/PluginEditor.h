#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class LairLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    LairLookAndFeel()
    {
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff141316));
        setColour(juce::ComboBox::textColourId, juce::Colour(0xffece4d0));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xffc8a464));
        setColour(juce::ComboBox::arrowColourId, juce::Colour(0xffc8a464));
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff141316));
        setColour(juce::PopupMenu::textColourId, juce::Colour(0xffece4d0));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xffc8a464));
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colour(0xff100f12));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(6.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto centre = bounds.getCentre();
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto arcRadius = radius - 4.0f;

        // Внешние насечки шкалы в стиле Lair
        g.setColour(juce::Colour(0xffece4d0).withAlpha(0.28f));
        const int numTicks = 11;
        for (int i = 0; i < numTicks; ++i)
        {
            const float prop = static_cast<float>(i) / static_cast<float>(numTicks - 1);
            const float angle = rotaryStartAngle + prop * (rotaryEndAngle - rotaryStartAngle);
            juce::Path tick;
            tick.startNewSubPath(0.0f, -arcRadius - 2.0f);
            tick.lineTo(0.0f, -arcRadius - 5.5f);
            g.strokePath(tick, juce::PathStrokeType(1.2f),
                         juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
        }

        // Фоновая дуга
        juce::Path bgArc;
        bgArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
        g.strokePath(bgArc, juce::PathStrokeType(2.2f, juce::PathStrokeType::curved, juce::PathStrokeType::butt));

        // Активная дуга значения
        juce::Path valArc;
        valArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle, toAngle, true);
        g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        g.strokePath(valArc, juce::PathStrokeType(2.8f, juce::PathStrokeType::curved, juce::PathStrokeType::butt));

        // Внутренний плоский винтажный диск крутилки
        auto knobRadius = arcRadius - 7.0f;
        if (knobRadius > 4.0f)
        {
            g.setColour(juce::Colour(0xff0b0a0d));
            g.fillEllipse(centre.x - knobRadius + 1.5f, centre.y - knobRadius + 2.0f, knobRadius * 2.0f, knobRadius * 2.0f);

            g.setColour(juce::Colour(0xff19171c));
            g.fillEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

            // Двойное кольцо в эстетике старых приборов Aberrant DSP
            g.setColour(juce::Colour(0xffece4d0).withAlpha(0.75f));
            g.drawEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.4f);

            g.setColour(juce::Colour(0xffc8a464).withAlpha(0.3f));
            g.drawEllipse(centre.x - knobRadius + 3.0f, centre.y - knobRadius + 3.0f, (knobRadius - 3.0f) * 2.0f, (knobRadius - 3.0f) * 2.0f, 0.8f);

            // Указатель крутилки
            juce::Path pointer;
            pointer.startNewSubPath(0.0f, -knobRadius * 0.2f);
            pointer.lineTo(0.0f, -knobRadius + 2.5f);
            g.setColour(slider.findColour(juce::Slider::thumbColourId));
            g.strokePath(pointer, juce::PathStrokeType(2.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded),
                         juce::AffineTransform::rotation(toAngle).translated(centre.x, centre.y));

            // Центральная точка
            g.fillEllipse(centre.x - 2.0f, centre.y - 2.0f, 4.0f, 4.0f);
        }
    }
};

class AbyssalReverbAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AbyssalReverbAudioProcessorEditor(AbyssalReverbAudioProcessor&);
    ~AbyssalReverbAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    AbyssalReverbAudioProcessor& processor;
    LairLookAndFeel lairLookAndFeel;

    juce::Label title, subtitle, modeLabel;
    juce::Slider size, decay, damping, width, mix, drive, tone;
    juce::Label sizeLabel, decayLabel, dampingLabel, widthLabel, mixLabel, driveLabel, toneLabel;
    juce::ComboBox mode;

    std::unique_ptr<SliderAttachment> sizeAttachment, decayAttachment, dampingAttachment,
        widthAttachment, mixAttachment, driveAttachment, toneAttachment;
    std::unique_ptr<ComboAttachment> modeAttachment;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText, juce::Colour accentColour);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbyssalReverbAudioProcessorEditor)
};
