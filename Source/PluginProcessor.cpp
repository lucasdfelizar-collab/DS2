#include "PluginProcessor.h"
#include "PluginEditor.h"

TwelveGaugeAudioProcessor::TwelveGaugeAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout TwelveGaugeAudioProcessor::createParameterLayout()
{
    using Float = juce::AudioParameterFloat;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<Float>("trigger", "Trigger", juce::NormalisableRange<float>(0.01f, 0.5f, 0.001f, 0.5f), 0.09f));
    layout.add(std::make_unique<Float>("tone", "Tone", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.62f));
    layout.add(std::make_unique<Float>("decay", "Decay", juce::NormalisableRange<float>(0.12f, 1.5f, 0.001f), 0.65f));
    layout.add(std::make_unique<Float>("mix", "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.75f));
    return layout;
}

bool TwelveGaugeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto output = layouts.getMainOutputChannelSet();
    return output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo();
}

void TwelveGaugeAudioProcessor::prepareToPlay(double sampleRate, int)
{
    sampleRateHz = static_cast<float>(sampleRate);
    inputEnvelope = previousEnvelope = 0.0f;
    cooldownSamples = 0;
    effectTime = effectLength;
    toneState = 0.0f;
    lowPhase = 0.0f;
}

float TwelveGaugeAudioProcessor::nextNoise()
{
    // Small deterministic xorshift PRNG; avoids allocation and external samples in the audio thread.
    noiseState = std::fmod(noiseState * 16807.0f, 1.0f);
    noiseState = noiseState * 3.999f + 0.000123f;
    if (noiseState >= 1.0f)
        noiseState -= 1.0f;
    return noiseState * 2.0f - 1.0f;
}

void TwelveGaugeAudioProcessor::triggerEffect(float decay)
{
    effectTime = 0.0f;
    effectLength = decay;
    lowPhase = 0.0f;
    toneState = 0.0f;
    // Slightly vary the noise stream on each hit for a less repetitive result.
    noiseState = std::fmod(noiseState + 0.173f, 1.0f);
}

void TwelveGaugeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = buffer.getNumChannels();
    const int samples = buffer.getNumSamples();
    const float trigger = parameters.getRawParameterValue("trigger")->load();
    const float tone = parameters.getRawParameterValue("tone")->load();
    const float decay = parameters.getRawParameterValue("decay")->load();
    const float mix = parameters.getRawParameterValue("mix")->load();
    const float attackCoeff = std::exp(-1.0f / (0.003f * sampleRateHz));
    const float releaseCoeff = std::exp(-1.0f / (0.075f * sampleRateHz));
    const float noiseCoeff = 0.015f + tone * 0.42f;
    const int cooldownDuration = static_cast<int>(0.22f * sampleRateHz);
    const float dryGain = 1.0f - mix;

    for (int i = 0; i < samples; ++i)
    {
        float inputPeak = 0.0f;
        for (int ch = 0; ch < channels; ++ch)
            inputPeak = juce::jmax(inputPeak, std::abs(buffer.getReadPointer(ch)[i]));

        const float coeff = inputPeak > inputEnvelope ? attackCoeff : releaseCoeff;
        inputEnvelope = coeff * inputEnvelope + (1.0f - coeff) * inputPeak;
        if (cooldownSamples > 0)
            --cooldownSamples;
        if (inputEnvelope >= trigger && previousEnvelope < trigger && cooldownSamples == 0)
        {
            triggerEffect(decay);
            cooldownSamples = cooldownDuration;
        }
        previousEnvelope = inputEnvelope;

        const float elapsed = effectTime;
        float generated = 0.0f;
        if (elapsed < effectLength)
        {
            const float normalized = elapsed / juce::jmax(effectLength, 0.01f);
            const float crackEnvelope = std::exp(-elapsed * (18.0f + (1.0f - tone) * 28.0f));
            const float tailEnvelope = std::pow(1.0f - normalized, 2.2f);
            const float noise = nextNoise();
            toneState += noiseCoeff * (noise - toneState);
            const float crack = (noise * (0.75f + tone * 0.25f) + toneState * 0.35f) * crackEnvelope;

            const float boomFreq = 48.0f + 34.0f * tone;
            lowPhase += juce::MathConstants<float>::twoPi * boomFreq / sampleRateHz;
            if (lowPhase > juce::MathConstants<float>::twoPi)
                lowPhase -= juce::MathConstants<float>::twoPi;
            const float boom = std::sin(lowPhase) * std::exp(-elapsed * 7.0f);
            generated = (crack * 0.82f + boom * 0.55f) * tailEnvelope * 0.9f;
            effectTime += 1.0f / sampleRateHz;
        }

        for (int ch = 0; ch < channels; ++ch)
        {
            const float dry = buffer.getReadPointer(ch)[i];
            const float stereoSpread = ch == 0 ? 1.0f : 0.94f;
            buffer.getWritePointer(ch)[i] = dry * dryGain + generated * stereoSpread * mix;
        }
    }
}

juce::AudioProcessorEditor* TwelveGaugeAudioProcessor::createEditor()
{
    return new TwelveGaugeAudioProcessorEditor(*this);
}

void TwelveGaugeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TwelveGaugeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TwelveGaugeAudioProcessor();
}
