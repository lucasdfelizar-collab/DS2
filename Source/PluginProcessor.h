#pragma once

#include <JuceHeader.h>

class TwelveGaugeAudioProcessor final : public juce::AudioProcessor
{
public:
    TwelveGaugeAudioProcessor();
    ~TwelveGaugeAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    float sampleRateHz = 44100.0f;
    float inputEnvelope = 0.0f;
    float previousEnvelope = 0.0f;
    int cooldownSamples = 0;
    float effectTime = 0.0f;
    float effectLength = 0.5f;
    float noiseState = 0.371f;
    float toneState = 0.0f;
    float lowPhase = 0.0f;
    float previousTrigger = 0.0f;

    float nextNoise();
    void triggerEffect(float decay);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TwelveGaugeAudioProcessor)
};
