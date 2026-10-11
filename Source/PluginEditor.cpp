#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "PresetManager.h"

//==============================================================================
// AnalogLAF
//==============================================================================
AnalogLAF::AnalogLAF()
{
    setColour(juce::Slider::thumbColourId, AnalogColour::Teal);
    setColour(juce::Slider::rotarySliderFillColourId, AnalogColour::Teal);
    setColour(juce::Slider::trackColourId, AnalogColour::TealDim);
    setColour(juce::Label::textColourId, AnalogColour::IvoryText);
    setColour(juce::ComboBox::backgroundColourId, AnalogColour::Panel);
    setColour(juce::ComboBox::textColourId, AnalogColour::IvoryText);
    setColour(juce::ComboBox::outlineColourId, AnalogColour::PanelBdr);
    setColour(juce::ComboBox::arrowColourId, AnalogColour::Teal);
    setColour(juce::PopupMenu::backgroundColourId, AnalogColour::Panel);
    setColour(juce::PopupMenu::textColourId, AnalogColour::IvoryText);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, AnalogColour::TealDim);
    setColour(juce::TextButton::buttonColourId, AnalogColour::Panel);
    setColour(juce::TextButton::textColourOffId, AnalogColour::Teal);
    setColour(juce::TextButton::textColourOnId, AnalogColour::BG);
    setColour(juce::ToggleButton::textColourId, AnalogColour::IvoryText);
    setColour(juce::ToggleButton::tickDisabledColourId, AnalogColour::DimText);
    setColour(juce::ToggleButton::tickColourId, AnalogColour::Teal);
}

juce::Font AnalogLAF::monoFont(float size)
{
    return juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(),
        size, juce::Font::plain));
}

juce::Font AnalogLAF::displayFont(float size)
{
    return juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(),
        size, juce::Font::bold));
}

void AnalogLAF::drawRotarySlider(juce::Graphics& g,
    int x, int y, int w, int h,
    float sliderPos, float startAng, float endAng,
    juce::Slider& slider)
{
    const float radius = (float)juce::jmin(w, h) * 0.5f - 4.0f;
    const float centreX = (float)x + (float)w * 0.5f;
    const float centreY = (float)y + (float)h * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = startAng + sliderPos * (endAng - startAng);

    g.setColour(AnalogColour::BG);
    g.fillEllipse(rx, ry, rw, rw);
    g.setColour(AnalogColour::PanelBdr);
    g.drawEllipse(rx, ry, rw, rw, 2.0f);

    const juce::Colour accentCol = slider.findColour(juce::Slider::rotarySliderFillColourId);
    {
        juce::Path trackArc;
        trackArc.addArc(rx + 3.0f, ry + 3.0f, rw - 6.0f, rw - 6.0f,
            startAng, endAng, true);
        g.setColour(accentCol.darker(1.4f));
        g.strokePath(trackArc, juce::PathStrokeType(3.0f,
            juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    {
        juce::Path fillArc;
        fillArc.addArc(rx + 3.0f, ry + 3.0f, rw - 6.0f, rw - 6.0f,
            startAng, angle, true);
        g.setColour(accentCol);
        g.strokePath(fillArc, juce::PathStrokeType(3.0f,
            juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    {
        juce::Path pointer;
        const float pointerLen = radius * 0.55f;
        const float pointerW = 2.0f;
        pointer.addRectangle(-pointerW * 0.5f, -radius + 4.0f, pointerW, pointerLen);
        pointer.applyTransform(juce::AffineTransform::rotation(angle)
            .translated(centreX, centreY));
        g.setColour(AnalogColour::White);
        g.fillPath(pointer);
    }
    g.setColour(accentCol);
    g.fillEllipse(centreX - 3.0f, centreY - 3.0f, 6.0f, 6.0f);
}

void AnalogLAF::drawToggleButton(juce::Graphics& g, juce::ToggleButton& btn,
    bool, bool)
{
    const bool isOn = btn.getToggleState();
    const auto bounds = btn.getLocalBounds().toFloat();

    const float rx = 4.0f, h = bounds.getHeight() - 8.0f;
    juce::Rectangle<float> pill(rx, 4.0f, h * 2.0f, h);
    g.setColour(isOn ? AnalogColour::Teal : AnalogColour::PanelBdr);
    g.fillRoundedRectangle(pill, h * 0.5f);

    const float thumbX = isOn ? pill.getRight() - h - 2.0f : pill.getX() + 2.0f;
    g.setColour(isOn ? AnalogColour::BG : AnalogColour::DimText);
    g.fillEllipse(thumbX, pill.getY() + 2.0f, h - 4.0f, h - 4.0f);

    g.setColour(isOn ? AnalogColour::IvoryText : AnalogColour::DimText);
    g.setFont(monoFont(11.0f));
    g.drawText(btn.getButtonText(),
        (int)pill.getRight() + 6, 0,
        btn.getWidth() - (int)pill.getRight() - 6, btn.getHeight(),
        juce::Justification::centredLeft, true);
}

void AnalogLAF::drawComboBox(juce::Graphics& g, int width, int height, bool,
    int buttonX, int buttonY, int buttonW, int buttonH,
    juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();
    g.setColour(AnalogColour::Panel);
    g.fillRoundedRectangle(bounds, 3.0f);
    g.setColour(AnalogColour::PanelBdr);
    g.drawRoundedRectangle(bounds.reduced(0.5f), 3.0f, 1.0f);

    // arrow
    const float ax = (float)(buttonX + buttonW / 2);
    const float ay = (float)(buttonY + buttonH / 2);
    juce::Path arrow;
    arrow.addTriangle(ax - 4.f, ay - 2.f, ax + 4.f, ay - 2.f, ax, ay + 4.f);
    g.setColour(box.findColour(juce::ComboBox::arrowColourId));
    g.fillPath(arrow);

    // text
    g.setColour(box.findColour(juce::ComboBox::textColourId));
    g.setFont(monoFont(11.0f));
    g.drawText(box.getText(), 6, 0, width - 20, height,
        juce::Justification::centredLeft, true);
}

//==============================================================================
// AnalogKnob
//==============================================================================
AnalogKnob::AnalogKnob(const juce::String& labelStr, const juce::String& pid,
    juce::AudioProcessorValueTreeState& apvts, juce::Colour accentCol)
    : paramID(pid), apvtsRef(apvts), accent(accentCol)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setPopupDisplayEnabled(true, false, nullptr);
    slider.setColour(juce::Slider::rotarySliderFillColourId, accentCol);
    slider.setColour(juce::Slider::thumbColourId, accentCol);
    addAndMakeVisible(slider);

    nameLabel.setText(labelStr, juce::dontSendNotification);
    nameLabel.setJustificationType(juce::Justification::centred);
    nameLabel.setColour(juce::Label::textColourId, AnalogColour::DimText);
    nameLabel.setFont(AnalogLAF::monoFont(10.0f));
    addAndMakeVisible(nameLabel);

    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setColour(juce::Label::textColourId, accentCol);
    valueLabel.setFont(AnalogLAF::monoFont(10.0f));
    addAndMakeVisible(valueLabel);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, pid, slider);

    slider.onValueChange = [this]()
        {
            if (auto* param = apvtsRef.getParameter(paramID))
                valueLabel.setText(param->getText(param->getValue(), 6),
                    juce::dontSendNotification);
        };
    slider.onValueChange();
}

void AnalogKnob::resized()
{
    auto b = getLocalBounds();
    const int labelH = 14;
    nameLabel.setBounds(b.removeFromTop(labelH));
    valueLabel.setBounds(b.removeFromBottom(labelH));
    slider.setBounds(b.reduced(2));
}

void AnalogKnob::paint(juce::Graphics&) {}

//==============================================================================
// AnalogSection
//==============================================================================
AnalogSection::AnalogSection(const juce::String& titleText, juce::Colour accentCol)
    : title(titleText), accent(accentCol)
{
}

void AnalogSection::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    // Body
    g.setColour(AnalogColour::Panel);
    g.fillRoundedRectangle(b, 6.0f);

    // Border in accent color
    g.setColour(accent.withAlpha(0.5f));
    g.drawRoundedRectangle(b.reduced(0.5f), 6.0f, 1.0f);

    // Header strip
    juce::Rectangle<float> hdr(b.getX(), b.getY(), b.getWidth(), 20.0f);
    g.setColour(AnalogColour::Header);
    g.fillRoundedRectangle(hdr, 6.0f);
    g.fillRect(hdr.withTop(hdr.getBottom() - 4.0f));

    // Title text
    g.setColour(accent);
    g.setFont(AnalogLAF::displayFont(10.5f));
    g.drawText(title, hdr.toNearestInt(), juce::Justification::centred, false);

    // Accent underline
    g.setColour(accent.withAlpha(0.85f));
    g.fillRect(b.getX() + 8.0f, b.getY() + 19.0f, b.getWidth() - 16.0f, 1.5f);
}

//==============================================================================
// AnalogMeter
//==============================================================================
AnalogMeter::AnalogMeter(AnalogImperfectionProcessor& p) : proc(p)
{
    startTimerHz(30);
}

AnalogMeter::~AnalogMeter()
{
    stopTimer();
}

void AnalogMeter::timerCallback()
{
    const float inL = proc.levelInRmsL.load();
    const float inR = proc.levelInRmsR.load();
    const float outL = proc.levelPeakL.load();
    const float outR = proc.levelPeakR.load();

    dispInL = juce::jmax(dispInL * 0.8f, inL);
    dispInR = juce::jmax(dispInR * 0.8f, inR);
    dispOutL = juce::jmax(dispOutL * 0.8f, outL);
    dispOutR = juce::jmax(dispOutR * 0.8f, outR);

    repaint();
}

void AnalogMeter::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setColour(AnalogColour::BG);
    g.fillRoundedRectangle(b, 3.0f);
    g.setColour(AnalogColour::PanelBdr);
    g.drawRoundedRectangle(b.reduced(0.5f), 3.0f, 1.0f);

    // 4 bars: inL, inR, outL, outR — drawn L to R
    const int barW = (getWidth() - 8) / 4;
   

    auto drawBar = [&](int x, float lin, juce::Colour col)
        {
            const float db = 20.0f * std::log10(juce::jmax(1e-6f, lin));
            const float norm = juce::jlimit(0.0f, 1.0f, (db + 60.0f) / 60.0f);
            const int h = getHeight() - 14;
            const int filled = (int)(norm * (float)h);
            const int top = 4 + h - filled;

            g.setColour(col.darker(1.6f));
            g.fillRect(x, 4, barW - 1, h);
            g.setColour(col);
            g.fillRect(x, top, barW - 1, filled);
        };

    drawBar(4, dispInL, AnalogColour::TealDim);
    drawBar(4 + barW, dispInR, AnalogColour::TealDim);
    drawBar(4 + barW * 2, dispOutL, AnalogColour::Teal);
    drawBar(4 + barW * 3, dispOutR, AnalogColour::Teal);

    g.setFont(AnalogLAF::monoFont(7.5f));
    g.setColour(AnalogColour::DimText);
    g.drawText("IN L", 4, getHeight() - 10, barW, 10, juce::Justification::centred);
    g.drawText("IN R", 4 + barW, getHeight() - 10, barW, 10, juce::Justification::centred);
    g.drawText("OUT L", 4 + barW * 2, getHeight() - 10, barW, 10, juce::Justification::centred);
    g.drawText("OUT R", 4 + barW * 3, getHeight() - 10, barW, 10, juce::Justification::centred);
}

//==============================================================================
// Editor constructor
//==============================================================================
AnalogImperfectionEditor::AnalogImperfectionEditor(AnalogImperfectionProcessor& p)
    : juce::AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&laf);

    using namespace AnalogParams;
    auto& a = p.apvts;

    // ===== Sections =====
    addAndMakeVisible(secFloor);
    addAndMakeVisible(secMains);
    addAndMakeVisible(secQuant);
    addAndMakeVisible(secSat);
    addAndMakeVisible(secDrift);
    addAndMakeVisible(secOutput);

    // ===== Floor =====
    floorAmountKnob = std::make_unique<AnalogKnob>("AMOUNT", FLOOR_AMOUNT, a, AnalogColour::Teal);
    floorShelfKnob = std::make_unique<AnalogKnob>("HF SHELF", FLOOR_HIGHSHELF, a, AnalogColour::Teal);
    floorLpKnob = std::make_unique<AnalogKnob>("LP", FLOOR_LP, a, AnalogColour::Teal);

    addAndMakeVisible(*floorAmountKnob);
    addAndMakeVisible(*floorShelfKnob);
    addAndMakeVisible(*floorLpKnob);

    floorPinkBtn.setLookAndFeel(&laf);
    floorPinkAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        a, FLOOR_PINK, floorPinkBtn);
    addAndMakeVisible(floorPinkBtn);

    // ===== Mains =====
    mainsAmountKnob = std::make_unique<AnalogKnob>("AMOUNT", MAINS_AMOUNT, a, AnalogColour::Amber);
    mainsH2Knob = std::make_unique<AnalogKnob>("2nd H", MAINS_H2, a, AnalogColour::Amber);
    mainsH3Knob = std::make_unique<AnalogKnob>("3rd H", MAINS_H3, a, AnalogColour::Amber);

    addAndMakeVisible(*mainsAmountKnob);
    addAndMakeVisible(*mainsH2Knob);
    addAndMakeVisible(*mainsH3Knob);

    setupCombo(mainsFreqCombo, { "50 Hz", "60 Hz" });
    mainsFreqAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        a, MAINS_FREQ, mainsFreqCombo);
    addAndMakeVisible(mainsFreqCombo);

    // ===== Quantize =====
    quantPitchKnob = std::make_unique<AnalogKnob>("PITCH", QUANT_PITCH, a, AnalogColour::Magenta);
    quantPitchStepsKnob = std::make_unique<AnalogKnob>("P STEPS", QUANT_PITCH_STEPS, a, AnalogColour::Magenta);
    quantWaveKnob = std::make_unique<AnalogKnob>("WAVE", QUANT_WAVE, a, AnalogColour::Magenta);
    quantWaveStepsKnob = std::make_unique<AnalogKnob>("W STEPS", QUANT_WAVE_STEPS, a, AnalogColour::Magenta);
    quantMixKnob = std::make_unique<AnalogKnob>("MIX", QUANT_MIX, a, AnalogColour::Magenta);

    addAndMakeVisible(*quantPitchKnob);
    addAndMakeVisible(*quantPitchStepsKnob);
    addAndMakeVisible(*quantWaveKnob);
    addAndMakeVisible(*quantWaveStepsKnob);
    addAndMakeVisible(*quantMixKnob);

    // ===== Saturation =====
    satDriveKnob = std::make_unique<AnalogKnob>("OTA DRIVE", SAT_DRIVE, a, AnalogColour::Phosphor);
    satCmosKnob = std::make_unique<AnalogKnob>("CMOS", SAT_CMOS, a, AnalogColour::Phosphor);
    satMixKnob = std::make_unique<AnalogKnob>("MIX", SAT_MIX, a, AnalogColour::Phosphor);

    addAndMakeVisible(*satDriveKnob);
    addAndMakeVisible(*satCmosKnob);
    addAndMakeVisible(*satMixKnob);

    // ===== Drift =====
    driftAmountKnob = std::make_unique<AnalogKnob>("AMOUNT", DRIFT_AMOUNT, a, AnalogColour::Violet);
    driftRateKnob = std::make_unique<AnalogKnob>("RATE", DRIFT_RATE, a, AnalogColour::Violet);

    addAndMakeVisible(*driftAmountKnob);
    addAndMakeVisible(*driftRateKnob);

    // ===== Output =====
    outputKnob = std::make_unique<AnalogKnob>("GAIN", OUTPUT, a, AnalogColour::Teal);
    addAndMakeVisible(*outputKnob);

    bypassBtn.setLookAndFeel(&laf);
    bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        a, BYPASS, bypassBtn);
    addAndMakeVisible(bypassBtn);

    // ===== A/B =====
    abButton.onClick = [this]() { audioProcessor.swapAB(); };
    captureBtn.onClick = [this]() { audioProcessor.captureAState(); };
    addAndMakeVisible(abButton);
    addAndMakeVisible(captureBtn);

    // ===== Meter =====
    meter = std::make_unique<AnalogMeter>(audioProcessor);
    addAndMakeVisible(*meter);

    // ===== Preset bar =====
    presetSelector.setLookAndFeel(&laf);
    presetSelector.setTextWhenNothingSelected("SELECT PRESET");

    auto styleBtn = [this](juce::TextButton& b)
        {
            b.setLookAndFeel(&laf);
            b.setColour(juce::TextButton::buttonColourId, AnalogColour::Panel);
            b.setColour(juce::TextButton::textColourOffId, AnalogColour::Teal);
        };
    styleBtn(prevPresetButton);
    styleBtn(nextPresetButton);
    styleBtn(savePresetButton);
    styleBtn(deletePresetButton);
    styleBtn(exportPresetButton);
    styleBtn(openFolderButton);

    addAndMakeVisible(prevPresetButton);
    addAndMakeVisible(nextPresetButton);
    addAndMakeVisible(presetSelector);
    addAndMakeVisible(savePresetButton);
    addAndMakeVisible(deletePresetButton);
    addAndMakeVisible(exportPresetButton);
    addAndMakeVisible(openFolderButton);

    refreshPresetList();

    prevPresetButton.onClick = [this] { audioProcessor.loadPreviousPreset(); };
    nextPresetButton.onClick = [this] { audioProcessor.loadNextPreset(); };
    presetSelector.onChange = [this]
        {
            if (updatingPresetSelector) return;
            onPresetSelected(presetSelector.getSelectedId());
        };

    savePresetButton.onClick = [this] { onSavePresetClicked(); };
    deletePresetButton.onClick = [this] { onDeletePresetClicked(); };
    exportPresetButton.onClick = [this] { onExportPresetClicked(); };
    openFolderButton.onClick = [this] { onOpenFolderClicked(); };

    audioProcessor.onPresetChanged = [this](int newIndex) {
        onProcessorPresetChanged(newIndex);
        };

    startTimerHz(10);

    setSize(940, 620);
    setResizable(false, false);
}

AnalogImperfectionEditor::~AnalogImperfectionEditor()
{
    stopTimer();

    audioProcessor.onPresetChanged = nullptr;

    presetSelector.setLookAndFeel(nullptr);
    prevPresetButton.setLookAndFeel(nullptr);
    nextPresetButton.setLookAndFeel(nullptr);
    savePresetButton.setLookAndFeel(nullptr);
    deletePresetButton.setLookAndFeel(nullptr);
    exportPresetButton.setLookAndFeel(nullptr);
    openFolderButton.setLookAndFeel(nullptr);

    floorPinkBtn.setLookAndFeel(nullptr);
    bypassBtn.setLookAndFeel(nullptr);

    setLookAndFeel(nullptr);
}

//==============================================================================
// setupCombo
//==============================================================================
void AnalogImperfectionEditor::setupCombo(juce::ComboBox& box, const juce::StringArray& items)
{
    box.setLookAndFeel(&laf);
    int id = 1;
    for (auto& item : items)
        box.addItem(item, id++);
    box.setSelectedId(1, juce::dontSendNotification);
}

//==============================================================================
// drawHeader
//==============================================================================
void AnalogImperfectionEditor::drawHeader(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(AnalogColour::Header);
    g.fillRect(area);

    // Bottom accent line — gradient
    juce::ColourGradient accent(
        AnalogColour::Teal.withAlpha(0.8f), 0.0f, (float)(area.getBottom() - 1),
        AnalogColour::Magenta.withAlpha(0.6f), (float)area.getWidth(), (float)(area.getBottom() - 1),
        false);
    g.setGradientFill(accent);
    g.fillRect(0, area.getBottom() - 2, area.getWidth(), 2);

    // Title
    g.setFont(AnalogLAF::displayFont(20.0f));
    g.setColour(AnalogColour::IvoryText);
    g.drawText("ANALOG IMPERFECTION", area.getX() + 16, area.getY(),
        280, area.getHeight(), juce::Justification::centredLeft);

    // Subtitle
    g.setFont(AnalogLAF::monoFont(9.0f));
    g.setColour(AnalogColour::DimText);
    g.drawText("Floor / Mains / Quantize / Saturation / Drift",
        area.getX() + 280, area.getY(),
        320, area.getHeight(), juce::Justification::centredLeft);

    // Version
    g.drawText("v1.0.0", area.getRight() - 60, area.getY(),
        54, area.getHeight(), juce::Justification::centredRight);
}

//==============================================================================
// paint
//==============================================================================
void AnalogImperfectionEditor::paint(juce::Graphics& g)
{
    g.fillAll(AnalogColour::BG);
    drawHeader(g, getLocalBounds().removeFromTop(44));

    // Subtle scanline pattern
    g.setColour(juce::Colours::white.withAlpha(0.012f));
    for (int y = 44; y < getHeight(); y += 3)
        g.drawHorizontalLine(y, 0.0f, (float)getWidth());
}

//==============================================================================
// resized
//
// Layout (940 x 620):
//
//  [ Header: 44px                                          ]
//
//  Row 1 (170px): [ FLOOR (3 knobs + pink toggle) ][ MAINS (3 knobs + freq combo) ]
//  Row 2 (170px): [ QUANTIZE (5 knobs)             ][ SATURATION (3 knobs)      ]
//  Row 3 (140px): [ DRIFT (2 knobs)                ][ OUTPUT (1 knob + bypass)  ]
//
//  All rows full width, split into two panels each with a small gap.
//==============================================================================
void AnalogImperfectionEditor::resized()
{
    const int headerH = 44;
    const int pad = 10;
    const int barH = 24;

    auto full = getLocalBounds();
    auto header = full.removeFromTop(headerH);
    auto body = full.reduced(pad);

    // ---------------------------------------------------------------------
    // Header
    // ---------------------------------------------------------------------
    {
        auto top = header.reduced(pad, (headerH - barH) / 2);

        // Right cluster
        meter->setBounds(top.removeFromRight(120).reduced(0, 1));
        top.removeFromRight(10);

        bypassBtn.setBounds(top.removeFromRight(90).reduced(0, 2));
        top.removeFromRight(6);

        abButton.setBounds(top.removeFromRight(84));
        top.removeFromRight(4);

        captureBtn.setBounds(top.removeFromRight(96));

        // Left cluster — preset bar (skip the drawn title area)
        top.removeFromLeft(290);

        prevPresetButton.setBounds(top.removeFromLeft(26));
        top.removeFromLeft(2);
        presetSelector.setBounds(top.removeFromLeft(200));
        top.removeFromLeft(2);
        nextPresetButton.setBounds(top.removeFromLeft(26));
        top.removeFromLeft(10);

        savePresetButton.setBounds(top.removeFromLeft(52));
        top.removeFromLeft(4);
        deletePresetButton.setBounds(top.removeFromLeft(46));
        top.removeFromLeft(4);
        exportPresetButton.setBounds(top.removeFromLeft(46));
        top.removeFromLeft(4);
        openFolderButton.setBounds(top.removeFromLeft(46));
    }

    // ---------------------------------------------------------------------
    // Body — three rows, each split into two panels
    // ---------------------------------------------------------------------
    const int rowGap = pad;
    const int panelGap = pad;

    // Compute row heights proportional to the panel content
    const int available = body.getHeight() - 2 * rowGap;
    const int row1H = (int)(available * 0.36f);
    const int row2H = (int)(available * 0.36f);
    const int row3H = available - row1H - row2H;

    auto row1 = body.removeFromTop(row1H);
    body.removeFromTop(rowGap);
    auto row2 = body.removeFromTop(row2H);
    body.removeFromTop(rowGap);
    auto row3 = body.removeFromTop(row3H);

    // Helper: place knobs evenly in a horizontal strip
    auto placeKnobs = [](juce::Rectangle<int> strip,
        std::initializer_list<AnalogKnob*> knobs)
        {
            const int n = (int)knobs.size();
            if (n == 0) return;
            const int slotW = strip.getWidth() / n;
            auto it = knobs.begin();
            for (int i = 0; i < n; ++i, ++it)
            {
                (*it)->setBounds(strip.removeFromLeft(slotW).reduced(2));
            }
        };

    // ===== ROW 1 =====
    // Left: Floor, Right: Mains. 60 / 40 split — Floor has 3 knobs + toggle,
    // Mains has 3 knobs + combo.  Roughly even.
    {
        const int leftW = (row1.getWidth() - panelGap) * 55 / 100;
        auto leftPanel = row1.removeFromLeft(leftW);
        row1.removeFromLeft(panelGap);
        auto rightPanel = row1;

        // --- FLOOR ---
        secFloor.setBounds(leftPanel);
        {
            auto inner = leftPanel.reduced(10, 0);
            inner.removeFromTop(26);  // section header

            // Row 1: 3 knobs
            auto knobStrip = inner.removeFromTop(juce::jmin(inner.getHeight() - 30, 120));
            placeKnobs(knobStrip, { floorAmountKnob.get(), floorShelfKnob.get(), floorLpKnob.get() });

            // Row 2: pink toggle
            inner.removeFromTop(8);
            auto togRow = inner.removeFromTop(24).reduced(10, 0);
            floorPinkBtn.setBounds(togRow.removeFromLeft(110));
        }

        // --- MAINS ---
        secMains.setBounds(rightPanel);
        {
            auto inner = rightPanel.reduced(10, 0);
            inner.removeFromTop(26);

            auto knobStrip = inner.removeFromTop(juce::jmin(inner.getHeight() - 30, 120));
            placeKnobs(knobStrip, { mainsAmountKnob.get(), mainsH2Knob.get(), mainsH3Knob.get() });

            inner.removeFromTop(8);
            auto comboRow = inner.removeFromTop(24).reduced(10, 0);
            mainsFreqCombo.setBounds(comboRow.removeFromLeft(120));
        }
    }

    // ===== ROW 2 =====
    // Left: Quantize (5 knobs), Right: Saturation (3 knobs).  60 / 40 split.
    {
        const int leftW = (row2.getWidth() - panelGap) * 60 / 100;
        auto leftPanel = row2.removeFromLeft(leftW);
        row2.removeFromLeft(panelGap);
        auto rightPanel = row2;

        // --- QUANTIZE ---
        secQuant.setBounds(leftPanel);
        {
            auto inner = leftPanel.reduced(10, 0);
            inner.removeFromTop(26);
            auto knobStrip = inner.removeFromTop(juce::jmin(inner.getHeight(), 130));
            placeKnobs(knobStrip, {
                quantPitchKnob.get(),
                quantPitchStepsKnob.get(),
                quantWaveKnob.get(),
                quantWaveStepsKnob.get(),
                quantMixKnob.get()
                });
        }

        // --- SATURATION ---
        secSat.setBounds(rightPanel);
        {
            auto inner = rightPanel.reduced(10, 0);
            inner.removeFromTop(26);
            auto knobStrip = inner.removeFromTop(juce::jmin(inner.getHeight(), 130));
            placeKnobs(knobStrip, {
                satDriveKnob.get(),
                satCmosKnob.get(),
                satMixKnob.get()
                });
        }
    }

    // ===== ROW 3 =====
    // Left: Drift (2 knobs), Right: Output (1 knob + bypass toggle).
    {
        const int leftW = (row3.getWidth() - panelGap) * 40 / 100;
        auto leftPanel = row3.removeFromLeft(leftW);
        row3.removeFromLeft(panelGap);
        auto rightPanel = row3;

        // --- DRIFT ---
        secDrift.setBounds(leftPanel);
        {
            auto inner = leftPanel.reduced(10, 0);
            inner.removeFromTop(26);
            auto knobStrip = inner.removeFromTop(juce::jmin(inner.getHeight(), 100));
            placeKnobs(knobStrip, { driftAmountKnob.get(), driftRateKnob.get() });
        }

        // --- OUTPUT ---
        secOutput.setBounds(rightPanel);
        {
            auto inner = rightPanel.reduced(10, 0);
            inner.removeFromTop(26);

            // Layout: knob on left, bypass on right
            auto content = inner;
            auto knobArea = content.removeFromLeft(juce::jmin(120, content.getWidth() / 2));
            outputKnob->setBounds(knobArea);

            content.removeFromLeft(10);
            auto tog = content.removeFromTop(24).withY(content.getCentreY() - 12);
            bypassBtn.setBounds(tog.removeFromLeft(120));
        }
    }
}

//==============================================================================
// Preset management
//==============================================================================
void AnalogImperfectionEditor::refreshPresetList()
{
    updatingPresetSelector = true;
    presetSelector.clear(juce::dontSendNotification);

    auto& manager = audioProcessor.getPresetManager();
    const auto& all = manager.getAllPresets();

    for (size_t i = 0; i < all.size(); ++i)
    {
        const auto& info = all[i];
        juce::String display;
        if (!info.isFactory)
            display << juce::String::charToString((juce::juce_wchar)0x2022) << " ";
        display << info.name;
        if (info.category.isNotEmpty())
            display << "   [" << info.category << "]";
        presetSelector.addItem(display, (int)i + 1);
    }

    int currentIdx = audioProcessor.getCurrentPresetIndex();
    int comboId = (currentIdx >= 0 && currentIdx < presetSelector.getNumItems())
        ? currentIdx + 1
        : (presetSelector.getNumItems() > 0 ? 1 : 0);

    if (comboId > 0)
        presetSelector.setSelectedId(comboId, juce::dontSendNotification);

    lastKnownPresetIndex = currentIdx;
    updatingPresetSelector = false;
}

void AnalogImperfectionEditor::onPresetSelected(int comboItemId)
{
    if (comboItemId <= 0) return;
    const int managerIndex = comboItemId - 1;
    audioProcessor.loadPresetByIndex(managerIndex);
}

void AnalogImperfectionEditor::onProcessorPresetChanged(int newIndex)
{
    updatingPresetSelector = true;
    if (newIndex >= 0 && newIndex < presetSelector.getNumItems())
        presetSelector.setSelectedId(newIndex + 1, juce::dontSendNotification);
    updatingPresetSelector = false;
    lastKnownPresetIndex = newIndex;
}

void AnalogImperfectionEditor::timerCallback()
{
    int current = audioProcessor.getCurrentPresetIndex();
    if (current != lastKnownPresetIndex)
        onProcessorPresetChanged(current);
}

void AnalogImperfectionEditor::onSavePresetClicked()
{
    auto* window = new juce::AlertWindow("Save Preset",
        "Enter a name for the new preset:",
        juce::MessageBoxIconType::NoIcon);
    window->addTextEditor("name", "", "Name:");

    const int currentIdx = presetSelector.getSelectedId() - 1;
    if (currentIdx >= 0)
    {
        auto& manager = audioProcessor.getPresetManager();
        if (currentIdx < manager.getNumPresets())
        {
            const auto& info = manager.getPresetInfo(currentIdx);
            window->getTextEditor("name")->setText(info.name + " (copy)",
                juce::dontSendNotification);
            window->getTextEditor("name")->selectAll();
        }
    }

    window->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
    window->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    window->enterModalState(true,
        juce::ModalCallbackFunction::create([this, window](int result)
            {
                juce::String name = window->getTextEditorContents("name").trim();
                if (result == 0 || name.isEmpty()) return;
                auto& manager = audioProcessor.getPresetManager();
                manager.saveUserPreset(name, "User", "", true);
                refreshPresetList();
            }), true);
}

void AnalogImperfectionEditor::onDeletePresetClicked()
{
    const int currentIdx = presetSelector.getSelectedId() - 1;
    if (currentIdx < 0) return;
    auto& manager = audioProcessor.getPresetManager();
    const auto& info = manager.getPresetInfo(currentIdx);
    if (info.isFactory)
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::InfoIcon,
            "Cannot Delete", "Factory presets cannot be deleted.");
        return;
    }
    manager.deletePreset(currentIdx);
    refreshPresetList();
}

void AnalogImperfectionEditor::onExportPresetClicked()
{
    juce::String suggestedName = "Preset";
    const int currentIdx = presetSelector.getSelectedId() - 1;
    if (currentIdx >= 0)
    {
        auto& manager = audioProcessor.getPresetManager();
        if (currentIdx < manager.getNumPresets())
            suggestedName = manager.getPresetInfo(currentIdx).name;
    }

    auto chooser = std::make_shared<juce::FileChooser>(
        "Export Preset As...",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile(suggestedName + ".chainchorus"),
        "*.chainchorus");

    chooser->launchAsync(
        juce::FileBrowserComponent::saveMode |
        juce::FileBrowserComponent::warnAboutOverwriting,
        [this, chooser, suggestedName](const juce::FileChooser& fc)
        {
            auto chosen = fc.getResult();
            if (chosen == juce::File()) return;
            if (!chosen.hasFileExtension("chainchorus"))
                chosen = chosen.withFileExtension("chainchorus");
            audioProcessor.getPresetManager().exportPresetToFile(chosen,
                suggestedName, "User", "");
        });
}

void AnalogImperfectionEditor::onOpenFolderClicked()
{
    audioProcessor.getPresetManager().getUserPresetDirectory().revealToUser();
}