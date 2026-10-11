#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

//==============================================================================
// Colour palette — "Analog Lab Dark"
//==============================================================================
namespace AnalogColour
{
    inline const juce::Colour BG{ 0xFF0E1116 };
    inline const juce::Colour Panel{ 0xFF1A1E26 };
    inline const juce::Colour PanelBdr{ 0xFF2A3040 };
    inline const juce::Colour Header{ 0xFF080A0E };

    // Section accents
    inline const juce::Colour Teal{ 0xFF00D4C8 };     // Floor / HF shelf
    inline const juce::Colour TealDim{ 0xFF005A55 };
    inline const juce::Colour Amber{ 0xFFFFB300 };    // Mains / Hum
    inline const juce::Colour AmberDim{ 0xFF6B4C00 };
    inline const juce::Colour Magenta{ 0xFFFF0066 };  // Quantize / Bitcrush
    inline const juce::Colour Phosphor{ 0xFF39FF14 }; // Saturation
    inline const juce::Colour Violet{ 0xFFBB66FF };   // Thermal drift
    inline const juce::Colour IvoryText{ 0xFFD8DDE8 };
    inline const juce::Colour DimText{ 0xFF6A7080 };
    inline const juce::Colour White{ 0xFFFFFFFF };
}

//==============================================================================
// LookAndFeel
//==============================================================================
class AnalogLAF : public juce::LookAndFeel_V4
{
public:
    AnalogLAF();

    void drawRotarySlider(juce::Graphics&, int x, int y, int w, int h,
        float sliderPos, float startAng, float endAng,
        juce::Slider&) override;

    void drawToggleButton(juce::Graphics&, juce::ToggleButton&,
        bool highlight, bool down) override;

    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonW, int buttonH,
        juce::ComboBox&) override;

    static juce::Font monoFont(float size);
    static juce::Font displayFont(float size);
};

//==============================================================================
// Labelled knob
//==============================================================================
class AnalogKnob : public juce::Component
{
public:
    AnalogKnob(const juce::String& label, const juce::String& paramID,
        juce::AudioProcessorValueTreeState& apvts,
        juce::Colour accent = AnalogColour::Teal);

    void resized() override;
    void paint(juce::Graphics&) override;

    juce::Slider slider;

private:
    juce::Label nameLabel, valueLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    juce::String paramID;
    juce::AudioProcessorValueTreeState& apvtsRef;
    juce::Colour accent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogKnob)
};

//==============================================================================
// Section panel — draws a titled container with an accent border
//==============================================================================
class AnalogSection : public juce::Component
{
public:
    AnalogSection(const juce::String& titleText, juce::Colour accentCol);

    void paint(juce::Graphics&) override;

    juce::String title;
    juce::Colour accent;
};

//==============================================================================
// Level meter (in / out, 30 Hz)
//==============================================================================
class AnalogMeter : public juce::Component,
    private juce::Timer
{
public:
    AnalogMeter(AnalogImperfectionProcessor& p);
    ~AnalogMeter() override;
    void paint(juce::Graphics&) override;
    void timerCallback() override;

private:
    AnalogImperfectionProcessor& proc;
    float dispInL = 0.f, dispInR = 0.f;
    float dispOutL = 0.f, dispOutR = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogMeter)
};

//==============================================================================
// Main editor
//==============================================================================
class AnalogImperfectionEditor : public juce::AudioProcessorEditor,
    private juce::Timer
{
public:
    AnalogImperfectionEditor(AnalogImperfectionProcessor&);
    ~AnalogImperfectionEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    AnalogImperfectionProcessor& audioProcessor;
    AnalogLAF laf;

    //==========================================================================
    // Sections
    //==========================================================================
    AnalogSection secFloor{ "ANALOG FLOOR", AnalogColour::Teal };
    AnalogSection secMains{ "MAINS RIPPLE", AnalogColour::Amber };
    AnalogSection secQuant{ "QUANTIZE", AnalogColour::Magenta };
    AnalogSection secSat{ "SATURATION", AnalogColour::Phosphor };
    AnalogSection secDrift{ "THERMAL DRIFT", AnalogColour::Violet };
    AnalogSection secOutput{ "OUTPUT", AnalogColour::Teal };

    //==========================================================================
    // Controls — Floor
    //==========================================================================
    std::unique_ptr<AnalogKnob> floorAmountKnob;
    std::unique_ptr<AnalogKnob> floorShelfKnob;
    std::unique_ptr<AnalogKnob> floorLpKnob;
    juce::ToggleButton floorPinkBtn{ "PINK" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> floorPinkAttach;

    //==========================================================================
    // Controls — Mains
    //==========================================================================
    std::unique_ptr<AnalogKnob> mainsAmountKnob;
    std::unique_ptr<AnalogKnob> mainsH2Knob;
    std::unique_ptr<AnalogKnob> mainsH3Knob;
    juce::ComboBox mainsFreqCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> mainsFreqAttach;

    //==========================================================================
    // Controls — Quantize
    //==========================================================================
    std::unique_ptr<AnalogKnob> quantPitchKnob;
    std::unique_ptr<AnalogKnob> quantPitchStepsKnob;
    std::unique_ptr<AnalogKnob> quantWaveKnob;
    std::unique_ptr<AnalogKnob> quantWaveStepsKnob;
    std::unique_ptr<AnalogKnob> quantMixKnob;

    //==========================================================================
    // Controls — Saturation
    //==========================================================================
    std::unique_ptr<AnalogKnob> satDriveKnob;
    std::unique_ptr<AnalogKnob> satCmosKnob;
    std::unique_ptr<AnalogKnob> satMixKnob;

    //==========================================================================
    // Controls — Drift
    //==========================================================================
    std::unique_ptr<AnalogKnob> driftAmountKnob;
    std::unique_ptr<AnalogKnob> driftRateKnob;

    //==========================================================================
    // Controls — Output
    //==========================================================================
    std::unique_ptr<AnalogKnob> outputKnob;
    juce::ToggleButton bypassBtn{ "BYPASS" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;

    //==========================================================================
    // A/B
    //==========================================================================
    juce::TextButton abButton{ "A/B SWAP" };
    juce::TextButton captureBtn{ "CAPTURE A" };

    //==========================================================================
    // Preset bar
    //==========================================================================
    juce::TextButton    prevPresetButton{ "<" };
    juce::TextButton    nextPresetButton{ ">" };
    juce::ComboBox      presetSelector;
    juce::TextButton    savePresetButton{ "SAVE" };
    juce::TextButton    deletePresetButton{ "DEL" };
    juce::TextButton    exportPresetButton{ "EXP" };
    juce::TextButton    openFolderButton{ "DIR" };

    bool updatingPresetSelector = false;
    int  lastKnownPresetIndex = -1;

    //==========================================================================
    // Meter
    //==========================================================================
    std::unique_ptr<AnalogMeter> meter;

    //==========================================================================
    // Helpers
    //==========================================================================
    void drawHeader(juce::Graphics&, juce::Rectangle<int>);
    void setupCombo(juce::ComboBox&, const juce::StringArray&);

    // Preset helpers
    void refreshPresetList();
    void onPresetSelected(int comboItemId);
    void onProcessorPresetChanged(int newIndex);
    void onSavePresetClicked();
    void onDeletePresetClicked();
    void onExportPresetClicked();
    void onOpenFolderClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogImperfectionEditor)
};

// Backward-compatible alias so old main.cpp / CMake references still compile
using ChainChorusAudioProcessorEditor = AnalogImperfectionEditor;