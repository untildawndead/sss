#include "PluginEditor.h"

AbyssalReverbAudioProcessorEditor::AbyssalReverbAudioProcessorEditor(AbyssalReverbAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&lairLookAndFeel);
    setSize(660, 420);

    title.setText("ABYSSAL // LAIR", juce::dontSendNotification);
    title.setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
    title.setColour(juce::Label::textColourId, juce::Colour(0xffece4d0));
    title.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(title);

    subtitle.setText("DARK CATACOMB REVERBERATOR", juce::dontSendNotification);
    subtitle.setFont(juce::Font(juce::FontOptions(10.5f, juce::Font::bold)));
    subtitle.setColour(juce::Label::textColourId, juce::Colour(0xffc8a464));
    subtitle.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(subtitle);

    const auto goldAccent  = juce::Colour(0xffd4af6a); // Янтарно-золотой (как в секции пространства Lair)
    const auto eerieAccent = juce::Colour(0xff7ec4a8); // Призрачно-бирюзовый (для модуляции/окраса)
    const auto bloodAccent = juce::Colour(0xffcf6a62); // Тускло-багровый (для сатурации и микса)

    // Инициализация каждой крутилки вместе с видимой текстовой подписью
    setupSlider(size,    sizeLabel,    "SIZE",    goldAccent);
    setupSlider(decay,   decayLabel,   "DECAY",   goldAccent);
    setupSlider(damping, dampingLabel, "DAMPING", goldAccent);
    setupSlider(width,   widthLabel,   "WIDTH",   eerieAccent);
    setupSlider(mix,     mixLabel,     "MIX",     bloodAccent);
    setupSlider(drive,   driveLabel,   "DRIVE",   bloodAccent);
    setupSlider(tone,    toneLabel,    "TONE",    eerieAccent);

    mode.addItem("SPACE", 1);
    mode.addItem("ARTIFACT", 2);
    mode.addItem("MIRROR", 3);
    mode.addItem("RIFT", 4);
    addAndMakeVisible(mode);

    modeLabel.setText("REALM / ARTIFACT", juce::dontSendNotification);
    modeLabel.setFont(juce::Font(juce::FontOptions(10.5f, juce::Font::bold)));
    modeLabel.setColour(juce::Label::textColourId, juce::Colour(0xffc8a464));
    modeLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(modeLabel);

    auto& state = processor.parameters;
    sizeAttachment    = std::make_unique<SliderAttachment>(state, "size", size);
    decayAttachment   = std::make_unique<SliderAttachment>(state, "decay", decay);
    dampingAttachment = std::make_unique<SliderAttachment>(state, "damping", damping);
    widthAttachment   = std::make_unique<SliderAttachment>(state, "width", width);
    mixAttachment     = std::make_unique<SliderAttachment>(state, "mix", mix);
    driveAttachment   = std::make_unique<SliderAttachment>(state, "drive", drive);
    toneAttachment    = std::make_unique<SliderAttachment>(state, "tone", tone);
    modeAttachment    = std::make_unique<ComboAttachment>(state, "mode", mode);
}

AbyssalReverbAudioProcessorEditor::~AbyssalReverbAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void AbyssalReverbAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label,
                                                    const juce::String& labelText, juce::Colour accentColour)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 18);
    slider.setName(labelText);

    slider.setColour(juce::Slider::rotarySliderFillColourId, accentColour);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2c2930));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffece4d0));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffece4d0));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff111014));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff38333d));
    addAndMakeVisible(slider);

    // Настройка видимого названия крутилки
    label.setText(labelText, juce::dontSendNotification);
    label.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
    label.setColour(juce::Label::textColourId, juce::Colour(0xffece4d0));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void AbyssalReverbAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Глубокий угольный фон в стиле Aberrant DSP
    g.fillAll(juce::Colour(0xff0e0d10));

    auto outer = getLocalBounds().toFloat().reduced(10.0f);
    g.setColour(juce::Colour(0xff161419));
    g.fillRect(outer);

    // Двойная контрастная рамка (кремовая + золотая)
    g.setColour(juce::Colour(0xffece4d0).withAlpha(0.75f));
    g.drawRect(outer, 1.5f);
    g.setColour(juce::Colour(0xffc8a464).withAlpha(0.35f));
    g.drawRect(outer.reduced(4.0f), 1.0f);

    // Разделитель шапки
    g.setColour(juce::Colour(0xffece4d0).withAlpha(0.45f));
    g.drawLine(22.0f, 76.0f, static_cast<float>(getWidth() - 22), 76.0f, 1.2f);

    // Секционные рамки (верхний блок CHAMBERS и нижний блок ALCHEMY & OUTPUT)
    auto topBox = juce::Rectangle<float>(24.0f, 90.0f, static_cast<float>(getWidth() - 48), 152.0f);
    auto botBox = juce::Rectangle<float>(24.0f, 254.0f, static_cast<float>(getWidth() - 48), 144.0f);

    g.setColour(juce::Colour(0xff121015));
    g.fillRect(topBox);
    g.fillRect(botBox);

    g.setColour(juce::Colour(0xffc8a464).withAlpha(0.4f));
    g.drawRect(topBox, 1.0f);
    g.drawRect(botBox, 1.0f);

    // Декоративный оккультный символ в шапке (в духе графики Lair)
    const float cx = 345.0f, cy = 43.0f;
    g.setColour(juce::Colour(0xffc8a464).withAlpha(0.5f));
    g.drawEllipse(cx - 18.0f, cy - 18.0f, 36.0f, 36.0f, 1.2f);
    g.drawEllipse(cx - 8.0f, cy - 8.0f, 16.0f, 16.0f, 1.2f);
    g.drawLine(cx - 26.0f, cy, cx + 26.0f, cy, 1.0f);
    g.drawLine(cx, cy - 24.0f, cx, cy + 24.0f, 1.0f);
}

void AbyssalReverbAudioProcessorEditor::resized()
{
    title.setBounds(26, 18, 280, 30);
    subtitle.setBounds(28, 46, 280, 18);

    modeLabel.setBounds(440, 20, 190, 16);
    mode.setBounds(450, 40, 180, 26);

    // Вспомогательная лямбда: размещает название ровно над крутилкой
    auto placeKnob = [](juce::Slider& slider, juce::Label& label, int x, int y, int w, int h)
    {
        label.setBounds(x, y, w, 20);
        slider.setBounds(x, y + 20, w, h - 20);
    };

    // Верхний ряд: 4 крутилки (SIZE, DECAY, DAMPING, WIDTH)
    const int topY = 98;
    const int knobW = 140;
    const int knobH = 136;

    placeKnob(size,    sizeLabel,    34,  topY, knobW, knobH);
    placeKnob(decay,   decayLabel,   184, topY, knobW, knobH);
    placeKnob(damping, dampingLabel, 334, topY, knobW, knobH);
    placeKnob(width,   widthLabel,   484, topY, knobW, knobH);

    // Нижний ряд: 3 крутилки (MIX, DRIVE, TONE)
    const int botY = 260;
    placeKnob(mix,   mixLabel,   95,  botY, knobW, knobH);
    placeKnob(drive, driveLabel, 260, botY, knobW, knobH);
    placeKnob(tone,  toneLabel,  425, botY, knobW, knobH);
}
