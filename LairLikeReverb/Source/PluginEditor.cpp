#include "PluginEditor.h"

AbyssalReverbAudioProcessorEditor::AbyssalReverbAudioProcessorEditor(AbyssalReverbAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(620, 390);
    title.setText("ABYSSAL", juce::dontSendNotification);
    title.setFont(juce::Font(juce::FontOptions(29.0f, juce::Font::bold)));
    title.setColour(juce::Label::textColourId, juce::Colour(0xfff0e9e4));
    title.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(title);

    subtitle.setText("EXPERIMENTAL SPACE / REVERB LAB", juce::dontSendNotification);
    subtitle.setFont(juce::Font(juce::FontOptions(10.5f, juce::Font::plain)));
    subtitle.setColour(juce::Label::textColourId, juce::Colour(0xffa69ba8));
    subtitle.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(subtitle);

    setupSlider(size, "SIZE"); setupSlider(decay, "DECAY"); setupSlider(damping, "DAMPING");
    setupSlider(width, "WIDTH"); setupSlider(mix, "MIX"); setupSlider(drive, "DRIVE"); setupSlider(tone, "TONE");
    mode.addItem("SPACE", 1); mode.addItem("ARTIFACT", 2); mode.addItem("MIRROR", 3); mode.addItem("RIFT", 4);
    mode.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff24202b));
    mode.setColour(juce::ComboBox::textColourId, juce::Colour(0xfff0e9e4));
    mode.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff6e596f));
    addAndMakeVisible(mode);
    modeLabel.setText("CHARACTER", juce::dontSendNotification);
    modeLabel.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
    modeLabel.setColour(juce::Label::textColourId, juce::Colour(0xffb6a8b8));
    addAndMakeVisible(modeLabel);

    auto& state = processor.parameters;
    sizeAttachment = std::make_unique<SliderAttachment>(state, "size", size);
    decayAttachment = std::make_unique<SliderAttachment>(state, "decay", decay);
    dampingAttachment = std::make_unique<SliderAttachment>(state, "damping", damping);
    widthAttachment = std::make_unique<SliderAttachment>(state, "width", width);
    mixAttachment = std::make_unique<SliderAttachment>(state, "mix", mix);
    driveAttachment = std::make_unique<SliderAttachment>(state, "drive", drive);
    toneAttachment = std::make_unique<SliderAttachment>(state, "tone", tone);
    modeAttachment = std::make_unique<ComboAttachment>(state, "mode", mode);
}

void AbyssalReverbAudioProcessorEditor::setupSlider(juce::Slider& slider, const juce::String& labelText)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 76, 20);
    slider.setName(labelText);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffd28cba));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff403444));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffffd7a1));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff0e9e4));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff201c26));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff403444));
    addAndMakeVisible(slider);
}

void AbyssalReverbAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff121018));
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff302033), 0.0f, 0.0f,
                                           juce::Colour(0xff121018), 620.0f, 390.0f, false));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(8.0f), 12.0f);
    g.setColour(juce::Colour(0xff5a405d));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(8.0f), 12.0f, 1.0f);
    g.setColour(juce::Colour(0xff5a405d).withAlpha(0.55f));
    g.drawLine(26.0f, 82.0f, 594.0f, 82.0f, 1.0f);
    g.setColour(juce::Colour(0xffd28cba).withAlpha(0.13f));
    g.fillEllipse(490.0f, 12.0f, 105.0f, 65.0f);
}

void AbyssalReverbAudioProcessorEditor::resized()
{
    title.setBounds(28, 19, 220, 37);
    subtitle.setBounds(30, 54, 300, 18);
    modeLabel.setBounds(398, 27, 100, 18);
    mode.setBounds(398, 46, 180, 27);

    const int top = 105;
    const int w = 140;
    const int h = 125;
    size.setBounds(28, top, w, h);
    decay.setBounds(178, top, w, h);
    damping.setBounds(328, top, w, h);
    width.setBounds(478, top, 112, h);
    mix.setBounds(78, 245, w, h);
    drive.setBounds(238, 245, w, h);
    tone.setBounds(398, 245, w, h);
}
