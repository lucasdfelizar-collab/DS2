#pragma once

#include "PluginProcessor.h"

class TwelveGaugeAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit TwelveGaugeAudioProcessorEditor(TwelveGaugeAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    TwelveGaugeAudioProcessor& processor;
    juce::Slider trigger, tone, decay, mix;
    juce::Label triggerLabel, toneLabel, decayLabel, mixLabel;
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> triggerAttachment, toneAttachment, decayAttachment, mixAttachment;
    void styleKnob(juce::Slider&, juce::Label&, const juce::String&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TwelveGaugeAudioProcessorEditor)
};
