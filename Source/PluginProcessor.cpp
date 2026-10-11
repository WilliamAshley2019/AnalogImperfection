#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PresetManager.h"

//==============================================================================
// Parameter layout
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
AnalogImperfectionProcessor::createParameterLayout()
{
    using namespace AnalogParams;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // --- Floor ---
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ FLOOR_AMOUNT, 1 }, "Floor Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.3f));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ FLOOR_PINK, 1 }, "Pink Noise", false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ FLOOR_HIGHSHELF, 1 }, "HF Shelf",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ FLOOR_LP, 1 }, "Floor LP",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));

    // --- Mains ripple ---
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ MAINS_AMOUNT, 1 }, "Mains Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ MAINS_FREQ, 1 }, "Mains Freq",
        juce::StringArray{ "50 Hz", "60 Hz" }, 1));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ MAINS_H2, 1 }, "2nd Harmonic",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ MAINS_H3, 1 }, "3rd Harmonic",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    // --- Quantize ---
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ QUANT_PITCH, 1 }, "Pitch Quantize",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{ QUANT_PITCH_STEPS, 1 }, "Pitch Steps",
        2, 24, 4));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ QUANT_WAVE, 1 }, "Wave Quantize",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{ QUANT_WAVE_STEPS, 1 }, "Wave Steps",
        2, 32, 8));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ QUANT_MIX, 1 }, "Quantize Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));

    // --- Saturation ---
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ SAT_DRIVE, 1 }, "OTA Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.3f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ SAT_CMOS, 1 }, "CMOS Clip",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ SAT_MIX, 1 }, "Saturation Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));

    // --- Thermal drift ---
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ DRIFT_AMOUNT, 1 }, "Drift Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ DRIFT_RATE, 1 }, "Drift Rate",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.3f));

    // --- Output ---
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ OUTPUT, 1 }, "Output",
        juce::NormalisableRange<float>(-24.0f, 12.0f, 0.01f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ BYPASS, 1 }, "Bypass", false));

    return layout;
}

//==============================================================================
// Constructor / destructor
//==============================================================================
AnalogImperfectionProcessor::AnalogImperfectionProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "AnalogImperfectionState", createParameterLayout())
{
    for (auto* param : apvts.processor.getParameters())
    {
        if (auto* p = dynamic_cast<juce::RangedAudioParameter*>(param))
            apvts.addParameterListener(p->getParameterID(), this);
    }

    presetManager = std::make_unique<PresetManager>(apvts);
}

AnalogImperfectionProcessor::~AnalogImperfectionProcessor()
{
    // Destroy preset manager before removing listeners
    presetManager.reset();

    onPresetChanged = nullptr;

    for (auto* param : apvts.processor.getParameters())
    {
        if (auto* p = dynamic_cast<juce::RangedAudioParameter*>(param))
            apvts.removeParameterListener(p->getParameterID(), this);
    }
}

//==============================================================================
// prepareToPlay / releaseResources
//==============================================================================
void AnalogImperfectionProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = juce::jlimit(8000.0, 384000.0, sampleRate);
    currentBlockSize = juce::jmax(1, samplesPerBlock);

    noiseL.Init((float)currentSampleRate);
    noiseR.Init((float)currentSampleRate);
    noiseR.mSeed = 0x9E3779B9u;

    rippleL.SetMainsHz(60.f, (float)currentSampleRate);
    rippleR.SetMainsHz(60.f, (float)currentSampleRate);
    rippleR.mPhase = 0.25f;  // slight phase offset for stereo width
    rippleL.Reset();
    rippleR.Reset();

    floorLpZL = floorLpZR = 0.f;

    smoothedOutput.reset(currentSampleRate, 0.02);
    smoothedDrift.reset(currentSampleRate, 0.05);

    driftPhase = 0.f;

    // Initial config
    updateNoiseConfig();
    updateMainsConfig();
    updateFloorLpFilter();
    updateDriftRate();

    levelPeakL.store(0.0f);
    levelPeakR.store(0.0f);

    isPrepared = true;
}

void AnalogImperfectionProcessor::releaseResources() { isPrepared = false; }

bool AnalogImperfectionProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();
    const auto& in = layouts.getMainInputChannelSet();
    if (out != juce::AudioChannelSet::stereo() && out != juce::AudioChannelSet::mono())
        return false;
    if (in != juce::AudioChannelSet::stereo() && in != juce::AudioChannelSet::mono())
        return false;
    return true;
}

//==============================================================================
// parameterChanged
//==============================================================================
void AnalogImperfectionProcessor::parameterChanged(const juce::String& paramID, float)
{
    using namespace AnalogParams;

    if (paramID == FLOOR_PINK ||
        paramID == FLOOR_AMOUNT ||
        paramID == FLOOR_HIGHSHELF)
    {
        updateNoiseConfig();
    }
    else if (paramID == FLOOR_LP)
    {
        updateFloorLpFilter();
    }
    else if (paramID == MAINS_FREQ ||
        paramID == MAINS_H2 ||
        paramID == MAINS_H3 ||
        paramID == MAINS_AMOUNT)
    {
        updateMainsConfig();
    }
    else if (paramID == DRIFT_RATE)
    {
        updateDriftRate();
    }
}

//==============================================================================
// Config updates
//==============================================================================
void AnalogImperfectionProcessor::updateNoiseConfig()
{
    using namespace AnalogParams;

    const float amount = apvts.getRawParameterValue(FLOOR_AMOUNT)->load();
    const bool pink = apvts.getRawParameterValue(FLOOR_PINK)->load() > 0.5f;
    const float shelf = apvts.getRawParameterValue(FLOOR_HIGHSHELF)->load();

    noiseL.mPinkEnabled = pink;
    noiseR.mPinkEnabled = pink;

    if (shelf > 0.001f)
    {
        // Map 0..1 to 0..12 dB high shelf at ~3 kHz
        const float boostDb = shelf * 12.f;
        noiseL.SetHighShelf(3000.f, boostDb, (float)currentSampleRate);
        noiseR.SetHighShelf(3000.f, boostDb, (float)currentSampleRate);
    }
    else
    {
        noiseL.mShelfGain = 0.f;
        noiseR.mShelfGain = 0.f;
    }

    // Store amount for the process loop
    // (we scale the raw noise by this in processBlock)
    // Nothing to do here — amount is read per block.
    juce::ignoreUnused(amount);
}

void AnalogImperfectionProcessor::updateMainsConfig()
{
    using namespace AnalogParams;

    const int freqIdx = (int)apvts.getRawParameterValue(MAINS_FREQ)->load();
    const float h2 = apvts.getRawParameterValue(MAINS_H2)->load();
    const float h3 = apvts.getRawParameterValue(MAINS_H3)->load();

    const float mainsHz = (freqIdx == 0) ? 50.f : 60.f;
    rippleL.SetMainsHz(mainsHz, (float)currentSampleRate);
    rippleR.SetMainsHz(mainsHz, (float)currentSampleRate);

    // Scale harmonics: base always 1.0, h2 and h3 add extra weight
    rippleL.SetAmplitudes(1.f, h2, h3);
    rippleR.SetAmplitudes(1.f, h2, h3);
}

void AnalogImperfectionProcessor::updateFloorLpFilter()
{
    using namespace AnalogParams;

    const float norm = apvts.getRawParameterValue(FLOOR_LP)->load();
    // 0 -> 500 Hz, 1 -> 20 kHz, exponential
    const float cutoffHz = 500.f * std::pow(40.f, norm);
    const float fc = std::min(cutoffHz, (float)currentSampleRate * 0.45f);
    floorLpCoeff = 1.f - std::exp(-2.f * (float)M_PI * fc / (float)currentSampleRate);
}

void AnalogImperfectionProcessor::updateDriftRate()
{
    using namespace AnalogParams;
    const float norm = apvts.getRawParameterValue(DRIFT_RATE)->load();
    // 0.01 Hz (very slow) to 1 Hz (audible wobble)
    driftRateHz = 0.01f * std::pow(100.f, norm);
}

//==============================================================================
// processBlock
//==============================================================================
void AnalogImperfectionProcessor::processBlock(juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    if (!isPrepared) return;

    const int numSamples = buffer.getNumSamples();
    const int numCh = buffer.getNumChannels();
    if (numSamples <= 0 || numCh < 1) return;

    using namespace AnalogParams;

    const bool bypassed = apvts.getRawParameterValue(BYPASS)->load() > 0.5f;
    const float outDb = apvts.getRawParameterValue(OUTPUT)->load();

    // Per-block param reads
    const float floorAmt = apvts.getRawParameterValue(FLOOR_AMOUNT)->load();
    const float mainsAmt = apvts.getRawParameterValue(MAINS_AMOUNT)->load();
    const float quantPitch = apvts.getRawParameterValue(QUANT_PITCH)->load();
    const int   quantPitchSteps = (int)apvts.getRawParameterValue(QUANT_PITCH_STEPS)->load();
    const float quantWave = apvts.getRawParameterValue(QUANT_WAVE)->load();
    const int   quantWaveSteps = (int)apvts.getRawParameterValue(QUANT_WAVE_STEPS)->load();
    const float quantMix = apvts.getRawParameterValue(QUANT_MIX)->load();
    const float satDrive = apvts.getRawParameterValue(SAT_DRIVE)->load();
    const float satCmos = apvts.getRawParameterValue(SAT_CMOS)->load();
    const float satMix = apvts.getRawParameterValue(SAT_MIX)->load();
    const float driftAmt = apvts.getRawParameterValue(DRIFT_AMOUNT)->load();

    smoothedOutput.setTargetValue(juce::Decibels::decibelsToGain(outDb));
    smoothedDrift.setTargetValue(driftAmt);

    // Input RMS for meter
    {
        float sumL = 0.f, sumR = 0.f;
        for (int i = 0; i < numSamples; ++i)
        {
            sumL += buffer.getSample(0, i) * buffer.getSample(0, i);
            if (numCh >= 2)
                sumR += buffer.getSample(1, i) * buffer.getSample(1, i);
        }
        levelInRmsL.store(std::sqrt(sumL / (float)numSamples));
        levelInRmsR.store(std::sqrt(sumR / (float)numSamples));
    }

    float* left = buffer.getWritePointer(0);
    float* right = (numCh >= 2) ? buffer.getWritePointer(1) : left;

    // Drift LFO phase increment
    const float driftInc = driftRateHz / (float)currentSampleRate;

    // Wave quantizer step count (inverse)
    const float waveStep = (quantWaveSteps > 1)
        ? (1.0f / (float)(quantWaveSteps - 1)) : 1.0f;

    // Pitch quantizer: convert semitones-per-octave to steps
    const float pitchStepsPerOct = (float)quantPitchSteps;

    for (int i = 0; i < numSamples; ++i)
    {
        float inL = left[i];
        float inR = right[i];

        if (bypassed)
        {
            const float g = smoothedOutput.getNextValue();
            left[i] = inL * g;
            if (numCh >= 2) right[i] = inR * g;
            continue;
        }

        // --------------------------------------------------------------
        // 1. THERMAL DRIFT — slow LFO modulates the drive amount slightly.
        //    This is subtle on its own but adds "vintage instability."
        // --------------------------------------------------------------
        driftPhase += driftInc;
        if (driftPhase >= 1.f) driftPhase -= 1.f;
        const float driftLfo = std::sin(2.f * (float)M_PI * driftPhase);
        const float driftAmtSmoothed = smoothedDrift.getNextValue();
        const float driftMod = 1.0f + driftLfo * 0.15f * driftAmtSmoothed;

        // --------------------------------------------------------------
        // 2. PITCH QUANTIZER — quantize the instantaneous *amplitude* to a
        //    step grid, which produces the same metallic aliasing that a
        //    VCO binary quantizer creates. When applied to a full signal,
        //    it acts as a stepped waveshaper.
        // --------------------------------------------------------------
        if (quantPitch > 0.0001f)
        {
            // Scale up, round, scale down
            const float scale = pitchStepsPerOct;
            const float qL = std::round(inL * scale) / scale;
            const float qR = std::round(inR * scale) / scale;
            inL = inL + quantPitch * (qL - inL);
            inR = inR + quantPitch * (qR - inR);
        }

        // --------------------------------------------------------------
        // 3. WAVE QUANTIZER — Wasp character, rounds waveform to N levels
        // --------------------------------------------------------------
        if (quantWave > 0.0001f)
        {
            const float qL = std::round(inL / waveStep) * waveStep;
            const float qR = std::round(inR / waveStep) * waveStep;
            inL = inL + quantWave * (qL - inL);
            inR = inR + quantWave * (qR - inR);
        }

        // Blend the quantized path against dry
        const float qMix = quantMix;
        inL = inL * qMix + left[i] * (1.f - qMix);
        inR = inR * qMix + right[i] * (1.f - qMix);

        // --------------------------------------------------------------
        // 4. OTA SATURATION — asymmetric soft saturation
        // --------------------------------------------------------------
        if (satDrive > 0.0001f)
        {
            const float drive = 1.f + satDrive * 8.f * driftMod;
            const float satL = kr106::OTASat(inL * drive);
            const float satR = kr106::OTASat(inR * drive);
            inL = inL + satMix * (satL - inL);
            inR = inR + satMix * (satR - inR);
        }

        // --------------------------------------------------------------
        // 5. CMOS SOFT CLIP — the same curve as the CMOS filter mode
        // --------------------------------------------------------------
        if (satCmos > 0.0001f)
        {
            const float cmL = kr106::cmosClip(inL * (1.f + satCmos * 4.f));
            const float cmR = kr106::cmosClip(inR * (1.f + satCmos * 4.f));
            const float norm = 1.f / (1.f + satCmos * 4.f);
            inL = inL + satCmos * (cmL * norm - inL);
            inR = inR + satCmos * (cmR * norm - inR);
        }

        // --------------------------------------------------------------
        // 6. ANALOG FLOOR — broadband hiss + optional pink + shelf
        // --------------------------------------------------------------
        if (floorAmt > 0.0001f)
        {
            // Gain curve: 0..1 -> 0..~-30 dBFS noise
            const float gain = floorAmt * floorAmt * 0.08f;

            float nL = noiseL.Process() * gain;
            float nR = noiseR.Process() * gain;

            // LP filter the noise floor
            floorLpZL += floorLpCoeff * (nL - floorLpZL);
            floorLpZR += floorLpCoeff * (nR - floorLpZR);

            inL += floorLpZL;
            inR += floorLpZR;
        }

        // --------------------------------------------------------------
        // 7. MAINS RIPPLE — hum injection on top of the signal
        // --------------------------------------------------------------
        if (mainsAmt > 0.0001f)
        {
            // Scale: 0..1 -> 0..~-50 dBFS hum (very subtle at low amounts)
            const float humGain = mainsAmt * mainsAmt * 0.003f;
            inL += rippleL.Process() * humGain;
            inR += rippleR.Process() * humGain;
        }

        // --------------------------------------------------------------
        // 8. Output gain
        // --------------------------------------------------------------
        const float g = smoothedOutput.getNextValue();
        left[i] = inL * g;
        if (numCh >= 2) right[i] = inR * g;
    }

    // Output meter
    float peakL = 0.f, peakR = 0.f;
    for (int i = 0; i < numSamples; ++i)
    {
        peakL = juce::jmax(peakL, std::abs(left[i]));
        if (numCh >= 2) peakR = juce::jmax(peakR, std::abs(right[i]));
    }
    levelPeakL.store(peakL);
    levelPeakR.store(peakR);
}

//==============================================================================
// A/B snapshot
//==============================================================================
void AnalogImperfectionProcessor::captureAState()
{
    aState = apvts.copyState();
    aStateValid = true;
}

void AnalogImperfectionProcessor::swapAB()
{
    if (!aStateValid) return;
    auto current = apvts.copyState();
    apvts.replaceState(aState);
    aState = current;
}

//==============================================================================
// Preset navigation
//==============================================================================
void AnalogImperfectionProcessor::loadPresetByIndex(int index)
{
    if (!presetManager) return;
    if (index < 0 || index >= presetManager->getNumPresets()) return;

    presetManager->loadPreset(index);
    currentPresetIndex = index;

    if (onPresetChanged) onPresetChanged(index);
}

void AnalogImperfectionProcessor::loadPreviousPreset()
{
    if (!presetManager) return;
    int n = presetManager->getNumPresets();
    if (n == 0) return;
    int idx = (currentPresetIndex <= 0) ? n - 1 : currentPresetIndex - 1;
    loadPresetByIndex(idx);
}

void AnalogImperfectionProcessor::loadNextPreset()
{
    if (!presetManager) return;
    int n = presetManager->getNumPresets();
    if (n == 0) return;
    int idx = (currentPresetIndex + 1) % n;
    loadPresetByIndex(idx);
}

//==============================================================================
// State
//==============================================================================
void AnalogImperfectionProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AnalogImperfectionProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* AnalogImperfectionProcessor::createEditor()
{
    return new AnalogImperfectionEditor(*this);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AnalogImperfectionProcessor();
}