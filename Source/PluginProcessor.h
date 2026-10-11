#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <JuceHeader.h>
#include <atomic>
#include <array>
#include "PresetManager.h"

//==============================================================================
// Analog Imperfection
// Console hiss / mains hum / bitcrush / OTA + CMOS drive / thermal drift
// Derived from Ultra Stinger's character primitives
// AlphaAudio by William Ashley Music
//==============================================================================

namespace kr106
{
    //==============================================================================
    // Analog noise floor — white / pink, with optional high shelf
    //==============================================================================
    namespace analog_noise
    {
        constexpr float kDryBroadbandGain = 1.5e-3f;
        constexpr float kDryRipple120 = 1.8e-5f;
        constexpr float kDryRipple240 = 8.9e-6f;
        constexpr float kDryRipple360 = 6.3e-6f;
        constexpr float kWetBroadbandGain = 1.8e-3f;
        constexpr float kWetShelfCornerHz = 3000.f;
        constexpr float kWetShelfBoostDb = 6.f;
        constexpr float kWetRipple120 = 7.9e-5f;
        constexpr float kWetRipple240 = 2.2e-5f;
        constexpr float kWetRipple360 = 9.8e-6f;
    }

    struct AnalogFloorNoise
    {
        uint32_t mSeed = 0x12345678u;
        float mLpState = 0.f;
        float mLpCoeff = 0.f;
        float mShelfState = 0.f;
        float mShelfCoeff = 0.f;
        float mShelfGain = 0.f;
        float mPink0 = 0, mPink1 = 0, mPink2 = 0, mPink3 = 0, mPink4 = 0, mPink5 = 0, mPink6 = 0;
        bool mPinkEnabled = false;

        void Init(float sampleRate, float cutoffHz = 20000.f)
        {
            const float fc = std::min(cutoffHz, sampleRate * 0.45f);
            mLpCoeff = 1.f - expf(-2.f * (float)M_PI * fc / sampleRate);
            mShelfCoeff = 0.f;
            mShelfGain = 0.f;
        }

        void SetHighShelf(float shelfFcHz, float hfBoostDb, float sampleRate)
        {
            const float fc = std::min(shelfFcHz, sampleRate * 0.45f);
            mShelfCoeff = 1.f - expf(-2.f * (float)M_PI * fc / sampleRate);
            mShelfGain = powf(10.f, hfBoostDb / 20.f) - 1.f;
        }

        float Process()
        {
            mSeed = mSeed * 196314165u + 907633515u;
            float white = (2.f * (float)mSeed / (float)0xFFFFFFFFu) - 1.f;

            if (mPinkEnabled)
            {
                mPink0 = 0.99886f * mPink0 + white * 0.0555179f;
                mPink1 = 0.99332f * mPink1 + white * 0.0750759f;
                mPink2 = 0.96900f * mPink2 + white * 0.1538520f;
                mPink3 = 0.86650f * mPink3 + white * 0.3104856f;
                mPink4 = 0.55000f * mPink4 + white * 0.5329522f;
                mPink5 = -0.7616f * mPink5 - white * 0.0168980f;
                float pink = mPink0 + mPink1 + mPink2 + mPink3 + mPink4 + mPink5 + mPink6 + white * 0.5362f;
                mPink6 = white * 0.115926f;
                float pinkOut = pink * 0.11f;
                if (mShelfGain != 0.f)
                {
                    mShelfState += mShelfCoeff * (pinkOut - mShelfState);
                    return pinkOut + mShelfGain * (pinkOut - mShelfState);
                }
                return pinkOut;
            }
            mLpState += mLpCoeff * (white - mLpState);
            if (mShelfGain != 0.f)
            {
                mShelfState += mShelfCoeff * (mLpState - mShelfState);
                return mLpState + mShelfGain * (mLpState - mShelfState);
            }
            return mLpState;
        }
    };

    //==============================================================================
    // Rail ripple — mains hum (60/50 Hz + 2nd + 3rd harmonics)
    //==============================================================================
    struct RailRipple
    {
        float mPhase = 0.f, mInc = 0.f;
        float mA1 = 0.f, mA2 = 0.f, mA3 = 0.f;

        void SetMainsHz(float mainsHz, float sampleRate) { mInc = (2.f * mainsHz) / sampleRate; }
        void SetAmplitudes(float a1, float a2, float a3) { mA1 = a1; mA2 = a2; mA3 = a3; }
        void Reset() { mPhase = 0.f; }
        float Process()
        {
            mPhase += mInc;
            if (mPhase >= 1.f) mPhase -= 1.f;
            const float tp = 2.f * (float)M_PI * mPhase;
            return mA1 * sinf(tp) + mA2 * sinf(2.f * tp) + mA3 * sinf(3.f * tp);
        }
    };

    //==============================================================================
    // OTA saturation — the same nonlinearity used in the KR-106 VCF
    //==============================================================================
    inline float OTASat(float x)
    {
        if (x > 3.f) return 1.f;
        if (x < -3.f) return -1.f;
        float x2 = x * x;
        return x * (27.f + x2) / (27.f + 9.f * x2);
    }

    //==============================================================================
    // CMOS soft clip
    //==============================================================================
    inline float cmosClip(float v)
    {
        return v / (1.0f + std::abs(v));
    }

} // namespace kr106

//==============================================================================
// Parameter IDs
//==============================================================================
namespace AnalogParams
{
    // --- Floor ---
    static constexpr const char* FLOOR_AMOUNT = "floor_amount";   // 0..1 broadband gain
    static constexpr const char* FLOOR_PINK = "floor_pink";     // bool
    static constexpr const char* FLOOR_HIGHSHELF = "floor_shelf";    // 0..1 extra HF
    static constexpr const char* FLOOR_LP = "floor_lp";       // 0..1 LP cutoff (0=2k,1=20k)

    // --- Mains ripple ---
    static constexpr const char* MAINS_AMOUNT = "mains_amount";   // 0..1
    static constexpr const char* MAINS_FREQ = "mains_freq";     // 0=50Hz, 1=60Hz
    static constexpr const char* MAINS_H2 = "mains_h2";       // 0..1 second harmonic weight
    static constexpr const char* MAINS_H3 = "mains_h3";       // 0..1 third harmonic weight

    // --- Quantize ---
    static constexpr const char* QUANT_PITCH = "quant_pitch";    // 0..1 pitch quantize
    static constexpr const char* QUANT_PITCH_STEPS = "quant_pitch_steps"; // 4..24 steps/oct
    static constexpr const char* QUANT_WAVE = "quant_wave";     // 0..1 waveform quantize
    static constexpr const char* QUANT_WAVE_STEPS = "quant_wave_steps"; // 2..32 levels
    static constexpr const char* QUANT_MIX = "quant_mix";      // 0..1 wet/dry

    // --- Saturation ---
    static constexpr const char* SAT_DRIVE = "sat_drive";     // 0..1 OTA drive
    static constexpr const char* SAT_CMOS = "sat_cmos";      // 0..1 CMOS amount
    static constexpr const char* SAT_MIX = "sat_mix";       // 0..1 wet/dry

    // --- Thermal drift ---
    static constexpr const char* DRIFT_AMOUNT = "drift_amount";  // 0..1 drift depth
    static constexpr const char* DRIFT_RATE = "drift_rate";    // 0..1 drift speed

    // --- Output ---
    static constexpr const char* OUTPUT = "output";        // dB, -24..+12
    static constexpr const char* BYPASS = "bypass";        // bool
}

//==============================================================================
// Processor
//==============================================================================
class AnalogImperfectionProcessor : public juce::AudioProcessor,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    AnalogImperfectionProcessor();
    ~AnalogImperfectionProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Analog Imperfection"; }
    bool acceptsMidi()  const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.5; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void parameterChanged(const juce::String& paramID, float newValue) override;

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Preset manager
    PresetManager& getPresetManager() { return *presetManager; }
    const PresetManager& getPresetManager() const { return *presetManager; }

    void loadPreviousPreset();
    void loadNextPreset();
    void loadPresetByIndex(int index);
    int  getCurrentPresetIndex() const { return currentPresetIndex; }

    std::function<void(int)> onPresetChanged;

    // A/B snapshot
    void captureAState();
    void swapAB();
    bool hasAState() const { return aStateValid; }

    // Output meter
    std::atomic<float> levelPeakL{ 0.0f };
    std::atomic<float> levelPeakR{ 0.0f };

    // In / out meter for the editor display
    std::atomic<float> levelInRmsL{ 0.0f };
    std::atomic<float> levelInRmsR{ 0.0f };

private:
    double currentSampleRate = 44100.0;
    int    currentBlockSize = 512;
    bool   isPrepared = false;

    // --- Per-channel analog noise generators ---
    kr106::AnalogFloorNoise noiseL, noiseR;
    kr106::RailRipple       rippleL, rippleR;

    // --- Per-channel LP filter for the noise floor ---
    float floorLpZL = 0.f, floorLpZR = 0.f;
    float floorLpCoeff = 0.f;

    // --- Thermal drift LFO ---
    float driftPhase = 0.f;
    float driftRateHz = 0.05f;

    // --- Smoothed output gain ---
    juce::SmoothedValue<float> smoothedOutput;
    juce::SmoothedValue<float> smoothedDrift;

    // --- A/B snapshot ---
    juce::ValueTree aState;
    bool aStateValid = false;

    // --- Preset manager ---
    std::unique_ptr<PresetManager> presetManager;
    int currentPresetIndex = -1;

    // Helpers
    void updateNoiseConfig();
    void updateMainsConfig();
    void updateFloorLpFilter();
    void updateDriftRate();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogImperfectionProcessor)
};

// Backward-compatible alias so the editor's existing references still compile
using ChainChorusAudioProcessor = AnalogImperfectionProcessor;