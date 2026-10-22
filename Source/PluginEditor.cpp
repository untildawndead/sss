#include "PluginEditor.h"

AbyssalReverbAudioProcessorEditor::AbyssalReverbAudioProcessorEditor(AbyssalReverbAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&crimsonLookAndFeel);
    setSize(740, 410);

    // Лаконичный заголовок без слов Lair и Dark catacomb reverberator
    title.setText("ABYSSAL", juce::dontSendNotification);
    title.setFont(juce::Font(juce::FontOptions(28.0f, juce::Font::bold)));
    title.setColour(juce::Label::textColourId, juce::Colour(0xffdc1e28));
    title.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(title);

    const auto crimsonBright = juce::Colour(0xffe01e2b);
    const auto crimsonDeep   = juce::Colour(0xffb51621);

    // Настройка всех 7 крутилок с видимыми названиями
    setupSlider(size,    sizeLabel,    "SIZE",    crimsonBright);
    setupSlider(decay,   decayLabel,   "DECAY",   crimsonBright);
    setupSlider(damping, dampingLabel, "DAMPING", crimsonDeep);
    setupSlider(width,   widthLabel,   "WIDTH",   crimsonDeep);
    setupSlider(mix,     mixLabel,     "MIX",     crimsonBright);
    setupSlider(drive,   driveLabel,   "DRIVE",   crimsonBright);
    setupSlider(tone,    toneLabel,    "TONE",    crimsonDeep);

    // Заголовок над окружностью выбора характера справа
    modeLabel.setText("CHARACTER", juce::dontSendNotification);
    modeLabel.setFont(juce::Font(juce::FontOptions(13.0f, juce::Font::bold)));
    modeLabel.setColour(juce::Label::textColourId, juce::Colour(0xffe01e2b));
    modeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(modeLabel);

    auto& state = processor.parameters;
    if (auto* modeParam = state.getParameter("mode"))
    {
        characterWheel = std::make_unique<CharacterWheel>(*modeParam);
        addAndMakeVisible(*characterWheel);
    }

    sizeAttachment    = std::make_unique<SliderAttachment>(state, "size", size);
    decayAttachment   = std::make_unique<SliderAttachment>(state, "decay", decay);
    dampingAttachment = std::make_unique<SliderAttachment>(state, "damping", damping);
    widthAttachment   = std::make_unique<SliderAttachment>(state, "width", width);
    mixAttachment     = std::make_unique<SliderAttachment>(state, "mix", mix);
    driveAttachment   = std::make_unique<SliderAttachment>(state, "drive", drive);
    toneAttachment    = std::make_unique<SliderAttachment>(state, "tone", tone);
}

AbyssalReverbAudioProcessorEditor::~AbyssalReverbAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void AbyssalReverbAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label,
                                                    const juce::String& labelText, juce::Colour accentColour)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 68, 18);
    slider.setName(labelText);

    slider.setColour(juce::Slider::rotarySliderFillColourId, accentColour);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff26090d));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffff2b39));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffd8b8bc));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff0b0607));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff420f16));
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setFont(juce::Font(juce::FontOptions(11.5f, juce::Font::bold)));
    label.setColour(juce::Label::textColourId, juce::Colour(0xffe0c6c8));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void AbyssalReverbAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Обсидианово-чёрный фон
    g.fillAll(juce::Colour(0xff060405));

    auto outer = getLocalBounds().toFloat().reduced(10.0f);
    g.setColour(juce::Colour(0xff0c0709));
    g.fillRect(outer);

    // Двойная кроваво-красная рамка корпуса
    g.setColour(juce::Colour(0xff9e1620));
    g.drawRect(outer, 1.5f);
    g.setColour(juce::Colour(0xff450b10));
    g.drawRect(outer.reduced(4.0f), 1.0f);

    // Горизонтальная линия под заголовком
    g.setColour(juce::Colour(0xff7a121b));
    g.drawLine(24.0f, 62.0f, static_cast<float>(getWidth() - 24), 62.0f, 1.2f);

    // Левые отсеки для крутилок
    auto topBox = juce::Rectangle<float>(24.0f, 76.0f, 476.0f, 150.0f);
    auto botBox = juce::Rectangle<float>(24.0f, 238.0f, 476.0f, 150.0f);
    // Правый отсек для кругового селектора CHARACTER
    auto rightBox = juce::Rectangle<float>(512.0f, 76.0f, 204.0f, 312.0f);

    g.setColour(juce::Colour(0xff090506));
    g.fillRect(topBox);
    g.fillRect(botBox);
    g.fillRect(rightBox);

    g.setColour(juce::Colour(0xff5e1017));
    g.drawRect(topBox, 1.0f);
    g.drawRect(botBox, 1.0f);
    g.drawRect(rightBox, 1.0f);
}

void AbyssalReverbAudioProcessorEditor::resized()
{
    title.setBounds(24, 18, 300, 36);

    auto placeKnob = [](juce::Slider& slider, juce::Label& label, int x, int y, int w, int h)
    {
        label.setBounds(x, y, w, 18);
        slider.setBounds(x, y + 18, w, h - 18);
    };

    const int knob
