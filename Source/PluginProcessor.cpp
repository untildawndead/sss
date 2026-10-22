#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace
{
    constexpr auto sizeID = "size";
    constexpr auto decayID = "decay";
    constexpr auto dampingID = "damping";
    constexpr auto widthID = "width";
    constexpr auto mixID = "mix";
    constexpr auto driveID = "drive";
    constexpr auto modeID = "mode";
    constexpr auto toneID = "tone";

    float getParam(juce::AudioProcessorValueTreeState& state, const char* id)
    {
        return state.getRawParameterValue(id)->load();
    }
}

AbyssalReverbAudioProcessor::AbyssalReverbAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout AbyssalReverbAudioProcessor::createParameterLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;

    auto percentAttrs = AudioParameterFloatAttributes()
        .withStringFromValueFunction([](float v, int) { return String(static_cast<int>(std::round(v * 100.0f))) + " %"; })
        .withValueFromStringFunction([](const String& t) { return t.retainCharacters("0123456789.,-").getFloatValue() * 0.01f; });

    auto secondsAttrs = AudioParameterFloatAttributes()
        .withStringFromValueFunction([](float v, int) { return String(v, 2) + " s"; })
        .withValueFromStringFunction([](const String& t) { return t.retainCharacters("0123456789.,-").getFloatValue(); });

    auto hzAttrs = AudioParameterFloatAttributes()
        .withStringFromValueFunction([](float v, int) { return String(static_cast<int>(std::round(v))) + " Hz"; })
        .withValueFromStringFunction([](const String& t) { return t.retainCharacters("0123456789.,-").getFloatValue(); });

    p.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID{sizeID, 1}, "Size",
        NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.68f, percentAttrs));

    p.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID{decayID, 1}, "Decay",
        NormalisableRange<float>(0.1f, 18.0f, 0.01f, 0.45f), 7.0f, secondsAttrs));

    p.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID{dampingID, 1}, "Damping",
        NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.42f, percentAttrs));

    p.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID{widthID, 1}, "Width",
        NormalisableRange<float>(0.0f, 2.0f, 0.001f), 1.25f, percentAttrs));

    p.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID{mixID, 1}, "Mix",
        NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.35f, percentAttrs));

    p.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID{driveID, 1}, "Drive",
        NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.12f, percentAttrs));

    p.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID{toneID, 1}, "Tone",
        NormalisableRange<float>(500.0f, 18000.0f, 1.0f, 0.35f), 9500.0f, hzAttrs));

    p.push_back(std::make_unique<AudioParameterChoice>(
        ParameterID{modeID, 1}, "Character",
        StringArray{"Space", "Artifact", "Mirror", "Rift"}, 0));

    return { p.begin(), p.end() };
}

void AbyssalReverbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 };
    reverb.prepare(spec);
    reverb.reset();
    reverb.setParameters(reverbParams);

    toneFilterL.prepare(spec);
    toneFilterR.prepare(spec);
    toneFilterL.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    toneFilterR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    toneFilterL.reset();
    toneFilterR.reset();

    wetSmoothed.reset(sampleRate, 0.025);
    driveSmoothed.reset(sampleRate, 0.025);
    wetSmoothed.setCurrentAndTargetValue(getParam(parameters, mixID));
    driveSmoothed.setCurrentAndTargetValue(getParam(parameters, driveID));

    dryBuffer.setSize(2, juce::jmax(1, samplesPerBlock), false, false, false);
}

void AbyssalReverbAudioProcessor::releaseResources()
{
    reverb.reset();
    toneFilterL.reset();
    toneFilterR.reset();
}

bool AbyssalReverbAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainIn = layouts.getMainInputChannelSet();
    const auto& mainOut = layouts.getMainOutputChannelSet();
    return mainIn == mainOut && (mainOut == juce::AudioChannelSet::mono() || mainOut == juce::AudioChannelSet::stereo());
}

void AbyssalReverbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = juce::jmin(2, buffer.getNumChannels());
    const int numSamples = buffer.getNumSamples();
    if (channels == 0 || numSamples == 0) return;

    const float size = getParam(parameters, sizeID);
    const float decay = getParam(parameters, decayID);
    const float damping = getParam(parameters, dampingID);
    const float width = getParam(parameters, widthID);
    const float mix = getParam(parameters, mixID);
    const float drive = getParam(parameters, driveID);
    const float tone = getParam(parameters, toneID);
    const int mode = static_cast<int>(getParam(parameters, modeID));

    float decayScale = 1.0f;
    float damp = damping;
    float stereoWidth = width;
    switch (mode)
    {
        case 1: decayScale = 1.28f; damp *= 0.72f; break;
        case 2: decayScale = 0.82f; damp *= 0.42f; stereoWidth = juce::jmin(2.0f, width * 1.45f); break;
        case 3: decayScale = 1.55f; damp = juce::jmax(0.05f, damping * 0.62f); stereoWidth = juce::jmin(2.0f, width * 1.2f); break;
        default: break;
    }

    const float room = juce::jlimit(0.05f, 1.0f, 0.10f + size * 0.25f + (decay / 18.0f) * 0.70f * decayScale);

    reverbParams.roomSize = room;
    reverbParams.damping = juce::jlimit(0.0f, 1.0f, damp);
    reverbParams.wetLevel = 1.0f;
    reverbParams.dryLevel = 0.0f;
    reverbParams.width = juce::jlimit(0.0f, 1.0f, stereoWidth * 0.5f);
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters(reverbParams);

    wetSmoothed.setTargetValue(mix);
    driveSmoothed.setTargetValue(drive);

    if (dryBuffer.getNumSamples() < numSamples)
        dryBuffer.setSize(2, numSamples, false, false, true);

    for (int ch = 0; ch < channels; ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    for (int i = 0; i < numSamples; ++i)
    {
        const float d = driveSmoothed.getNextValue();
        const float gain = 1.0f + d * 5.0f;
        const float norm = (d > 0.001f) ? (1.0f / std::tanh(gain)) : 1.0f;
        for (int ch = 0; ch < channels; ++ch)
        {
            const float x = buffer.getSample(ch, i) * gain;
            buffer.setSample(ch, i, (d > 0.001f) ? std::tanh(x) * norm : x);
        }
    }

    auto reverbBlock = juce::dsp::AudioBlock<float>(buffer)
                           .getSubsetChannelBlock(0, static_cast<size_t>(channels));
    juce::dsp::ProcessContextReplacing<float> reverbContext(reverbBlock);
    reverb.process(reverbContext);

    const float modeToneScale = (mode == 2) ? 1.0f : ((mode == 3) ? 0.72f : ((mode == 1) ? 0.86f : 0.92f));
    const float cutoff = juce::jlimit(300.0f, 20000.0f, tone * modeToneScale);
    toneFilterL.setCutoffFrequency(cutoff);
    toneFilterR.setCutoffFrequency(cutoff);

    for (int i = 0; i < numSamples; ++i)
    {
        const float wet = wetSmoothed.getNextValue();
        const float dry = 1.0f - wet;
        const float wetL = toneFilterL.processSample(0, buffer.getSample(0, i));
        const float dryL = dryBuffer.getSample(0, i);
        buffer.setSample(0, i, dryL * dry + wetL * wet);
        if (channels > 1)
        {
            const float wetR = toneFilterR.processSample(0, buffer.getSample(1, i));
            const float dryR = dryBuffer.getSample(1, i);
            buffer.setSample(1, i, dryR * dry + wetR * wet);
        }
    }

    const float makeupGain = juce::jlimit(0.5f, 1.0f, 1.0f - drive * 0.12f);
    for (int ch = 0; ch < channels; ++ch)
        buffer.applyGain(ch, 0, numSamples, makeupGain);
}

juce::AudioProcessorEditor* AbyssalReverbAudioProcessor::createEditor()
{
    return new AbyssalReverbAudioProcessorEditor(*this);
}

void AbyssalReverbAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AbyssalReverbAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AbyssalReverbAudioProcessor();
}
