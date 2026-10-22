#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <cmath>

class CrimsonLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(6.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto centre = bounds.getCentre();
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto arcRadius = radius - 4.0f;

        const int numTicks = 13;
        for (int i = 0; i < numTicks; ++i)
        {
            const float prop = static_cast<float>(i) / static_cast<float>(numTicks - 1);
            const float angle = rotaryStartAngle + prop * (rotaryEndAngle - rotaryStartAngle);
            const bool active = (prop <= sliderPos + 0.001f);

            g.setColour(active ? juce::Colour(0xffdc1e28) : juce::Colour(0xff4a1218));
            juce::Path tick;
            tick.startNewSubPath(0.0f, -arcRadius - 1.5f);
            tick.lineTo(0.0f, -arcRadius - 5.0f);
            g.strokePath(tick, juce::PathStrokeType(active ? 1.5f : 1.0f),
                         juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
        }

        juce::Path bgArc;
        bgArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour(0xff26090d));
        g.strokePath(bgArc, juce::PathStrokeType(2.5f));

        juce::Path valArc;
        valArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle, toAngle, true);
        g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        g.strokePath(valArc, juce::PathStrokeType(3.0f));

        auto knobRadius = arcRadius - 7.0f;
        if (knobRadius > 4.0f)
        {
            g.setColour(juce::Colour(0xff030203));
            g.fillEllipse(centre.x - knobRadius + 1.5f, centre.y - knobRadius + 2.5f, knobRadius * 2.0f, knobRadius * 2.0f);

            juce::ColourGradient grad(juce::Colour(0xff1a1214), centre.x, centre.y - knobRadius,
                                      juce::Colour(0xff090607), centre.x, centre.y + knobRadius, false);
            g.setGradientFill(grad);
            g.fillEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

            g.setColour(juce::Colour(0xff8c1620));
            g.drawEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.4f);

            g.setColour(juce::Colour(0xff3d0c11));
            g.drawEllipse(centre.x - knobRadius + 3.5f, centre.y - knobRadius + 3.5f,
                          (knobRadius - 3.5f) * 2.0f, (knobRadius - 3.5f) * 2.0f, 1.0f);

            juce::Path pointer;
            pointer.startNewSubPath(0.0f, -knobRadius * 0.18f);
            pointer.lineTo(0.0f, -knobRadius + 2.0f);
            g.setColour(slider.findColour(juce::Slider::thumbColourId));
            g.strokePath(pointer, juce::PathStrokeType(2.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded),
                         juce::AffineTransform::rotation(toAngle).translated(centre.x, centre.y));

            g.setColour(juce::Colour(0xffdc1e28));
            g.fillEllipse(centre.x - 2.0f, centre.y - 2.0f, 4.0f, 4.0f);
        }
    }
};

class CharacterWheel final : public juce::Component
{
public:
    explicit CharacterWheel(juce::RangedAudioParameter& param)
        : attachment(param, [this](float v) { updateIndex(static_cast<int>(std::round(v))); })
    {
        attachment.sendInitialUpdate();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        const auto centre = bounds.getCentre();
        const float ringRadius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.28f;

        g.setColour(juce::Colour(0xff160709));
        g.fillEllipse(centre.x - ringRadius - 10.0f, centre.y - ringRadius - 10.0f,
                      (ringRadius + 10.0f) * 2.0f, (ringRadius + 10.0f) * 2.0f);

        g.setColour(juce::Colour(0xff4a1017));
        g.drawEllipse(centre.x - ringRadius - 10.0f, centre.y - ringRadius - 10.0f,
                      (ringRadius + 10.0f) * 2.0f, (ringRadius + 10.0f) * 2.0f, 1.0f);

        g.setColour(juce::Colour(0xffb01c26));
        g.drawEllipse(centre.x - ringRadius, centre.y - ringRadius, ringRadius * 2.0f, ringRadius * 2.0f, 2.0f);

        const float innerR = ringRadius * 0.45f;
        g.setColour(juce::Colour(0xff090405));
        g.fillEllipse(centre.x - innerR, centre.y - innerR, innerR * 2.0f, innerR * 2.0f);
        g.setColour(juce::Colour(0xff6b1118));
        g.drawEllipse(centre.x - innerR, centre.y - innerR, innerR * 2.0f, innerR * 2.0f, 1.2f);

        for (int i = 0; i < 4; ++i)
        {
            const float diagAngle = juce::MathConstants<float>::pi * 0.25f + static_cast<float>(i) * juce::MathConstants<float>::halfPi;
            const float x1 = centre.x + std::sin(diagAngle) * innerR;
            const float y1 = centre.y - std::cos(diagAngle) * innerR;
            const float x2 = centre.x + std::sin(diagAngle) * (ringRadius + 10.0f);
            const float y2 = centre.y - std::cos(diagAngle) * (ringRadius + 10.0f);
            g.setColour(juce::Colour(0xff380b10));
            g.drawLine(x1, y1, x2, y2, 1.0f);
        }

        const char* names[4] = { "SPACE", "ARTIFACT", "MIRROR", "RIFT" };
        const float angles[4] = {
            0.0f,
            juce::MathConstants<float>::halfPi,
            juce::MathConstants<float>::pi,
            juce::MathConstants<float>::pi * 1.5f
        };

        const float activeAngle = angles[currentIndex];
        juce::Path needle;
        needle.startNewSubPath(0.0f, 8.0f);
        needle.lineTo(0.0f, -ringRadius + 4.0f);
        g.setColour(juce::Colour(0xffff2432));
        g.strokePath(needle, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded),
                     juce::AffineTransform::rotation(activeAngle).translated(centre.x, centre.y));

        g.setColour(juce::Colour(0xffff2432));
        g.fillEllipse(centre.x - 4.5f, centre.y - 4.5f, 9.0f, 9.0f);

        g.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
        for (int i = 0; i < 4; ++i)
        {
            const bool isSelected = (i == currentIndex);
            const float nx = centre.x + std::sin(angles[i]) * ringRadius;
            const float ny = centre.y - std::cos(angles[i]) * ringRadius;

            if (isSelected)
            {
                g.setColour(juce::Colour(0x55ff1e2b));
                g.fillEllipse(nx - 9.0f, ny - 9.0f, 18.0f, 18.0f);
                g.setColour(juce::Colour(0xffff2a36));
                g.fillEllipse(nx - 5.0f, ny - 5.0f, 10.0f, 10.0f);
            }
            else
            {
                g.setColour(juce::Colour(0xff140709));
                g.fillEllipse(nx - 4.5f, ny - 4.5f, 9.0f, 9.0f);
                g.setColour(juce::Colour(0xff801820));
                g.drawEllipse(nx - 4.5f, ny - 4.5f, 9.0f, 9.0f, 1.3f);
            }

            juce::Rectangle<float> textRect;
            if (i == 0)      textRect = { centre.x - 45.0f, ny - 30.0f, 90.0f, 18.0f };
            else if (i == 1) textRect = { nx + 10.0f,       ny - 9.0f,  66.0f, 18.0f };
            else if (i == 2) textRect = { centre.x - 45.0f, ny + 12.0f, 90.0f, 18.0f };
            else             textRect = { nx - 76.0f,       ny - 9.0f,  66.0f, 18.0f };

            g.setColour(isSelected ? juce::Colour(0xffff3844) : juce::Colour(0xffb58d91));
            const auto just = (i == 1) ? juce::Justification::centredLeft
                            : (i == 3) ? juce::Justification::centredRight
                                       : juce::Justification::centred;
            g.drawText(names[i], textRect, just, false);
