#include "PluginEditor.h"

namespace
{
const juce::Colour orange(0xffe75c20);
const juce::Colour dark(0xff242321);
}

TwelveGaugeAudioProcessorEditor::TwelveGaugeAudioProcessorEditor(TwelveGaugeAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(560, 390);
    styleKnob(trigger, triggerLabel, "TRIGGER");
    styleKnob(tone, toneLabel, "TONE");
    styleKnob(decay, decayLabel, "DECAY");
    styleKnob(mix, mixLabel, "MIX");

    triggerAttachment = std::make_unique<Attachment>(processor.parameters, "trigger", trigger);
    toneAttachment = std::make_unique<Attachment>(processor.parameters, "tone", tone);
    decayAttachment = std::make_unique<Attachment>(processor.parameters, "decay", decay);
    mixAttachment = std::make_unique<Attachment>(processor.parameters, "mix", mix);
}

void TwelveGaugeAudioProcessorEditor::styleKnob(juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffffd6a5));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff6e2b17));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff8eee0));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, dark);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setFont(juce::Font(juce::FontOptions(13.0f, juce::Font::bold)));
    addAndMakeVisible(label);
}

void TwelveGaugeAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.fillAll(dark);
    auto pedal = bounds.reduced(16.0f);
    g.setColour(orange);
    g.fillRoundedRectangle(pedal, 22.0f);
    g.setColour(juce::Colour(0xffb93c18));
    g.drawRoundedRectangle(pedal, 22.0f, 3.0f);

    g.setColour(juce::Colour(0x33ffffff));
    for (int x = 35; x < getWidth() - 20; x += 16)
        g.drawVerticalLine(x, 24.0f, 72.0f);

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(juce::FontOptions(25.0f, juce::Font::bold)));
    g.drawFittedText("12 GAUGE", 30, 25, getWidth() - 60, 34, juce::Justification::centred, 1);
    g.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
    g.drawFittedText("GUITAR-TRIGGERED IMPACT  •  SYNTHESIZED AUDIO", 26, 60, getWidth() - 52, 18, juce::Justification::centred, 1);

    g.setColour(juce::Colour(0xfff5c5a4));
    g.drawRoundedRectangle(30.0f, 87.0f, (float)getWidth() - 60.0f, 220.0f, 12.0f, 1.5f);
    g.setColour(juce::Colour(0xff762b1c));
    g.fillEllipse((float)getWidth() * 0.5f - 9.0f, 315.0f, 18.0f, 18.0f);
    g.setColour(juce::Colour(0xffd5d0c8));
    g.fillRoundedRectangle((float)getWidth() * 0.5f - 30.0f, 341.0f, 60.0f, 27.0f, 5.0f);
    g.setColour(juce::Colour(0xff5d5a56));
    g.drawRoundedRectangle((float)getWidth() * 0.5f - 30.0f, 341.0f, 60.0f, 27.0f, 5.0f, 1.2f);
}

void TwelveGaugeAudioProcessorEditor::resized()
{
    const int knobWidth = 94;
    const int knobHeight = 128;
    const int gap = (getWidth() - 4 * knobWidth) / 5;
    juce::Slider* sliders[] = { &trigger, &tone, &decay, &mix };
    juce::Label* labels[] = { &triggerLabel, &toneLabel, &decayLabel, &mixLabel };
    for (int i = 0; i < 4; ++i)
    {
        const int x = gap + i * (knobWidth + gap);
        sliders[i]->setBounds(x, 125, knobWidth, knobHeight);
        labels[i]->setBounds(x, 102, knobWidth, 20);
    }
}
