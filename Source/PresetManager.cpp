#include "PresetManager.h"
#include "PluginProcessor.h"

//==============================================================================
// Factory preset bank
//
// Categories: Clean, Tape, Vintage, Console, Aggressive, Creative
//==============================================================================
namespace
{
    using P = std::pair<const char*, float>;

    PresetManager::FactoryPreset makePreset(const char* name,
        const char* category,
        std::initializer_list<P> pairs)
    {
        PresetManager::FactoryPreset fp;
        fp.name = name;
        fp.category = category;
        fp.author = "Factory";
        for (auto& p : pairs)
            fp.params.push_back({ juce::String(p.first), p.second });
        return fp;
    }
}

const std::vector<PresetManager::FactoryPreset>&
PresetManager::getFactoryPresets()
{
    using namespace AnalogParams;

    static const std::vector<FactoryPreset> presets = []()
        {
            std::vector<FactoryPreset> v;

            // =========================================================
            // CLEAN / SUBTLE
            // =========================================================
            v.push_back(makePreset("Init (Clean)", "Clean", {
                { FLOOR_AMOUNT, 0.f },{ FLOOR_PINK, 0.f },{ FLOOR_HIGHSHELF, 0.f },
                { FLOOR_LP, 1.f },{ MAINS_AMOUNT, 0.f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.5f },{ MAINS_H3, 0.5f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.f },{ QUANT_WAVE_STEPS, 8.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.f },{ SAT_CMOS, 0.f },{ SAT_MIX, 1.f },
                { DRIFT_AMOUNT, 0.f },{ DRIFT_RATE, 0.3f },
                { OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Whisper Floor", "Clean", {
                { FLOOR_AMOUNT, 0.15f },{ FLOOR_PINK, 0.f },{ FLOOR_HIGHSHELF, 0.f },
                { FLOOR_LP, 0.85f },{ MAINS_AMOUNT, 0.f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.5f },{ MAINS_H3, 0.5f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.f },{ QUANT_WAVE_STEPS, 8.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.f },{ SAT_CMOS, 0.f },{ SAT_MIX, 1.f },
                { DRIFT_AMOUNT, 0.f },{ DRIFT_RATE, 0.3f },
                { OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Soft OTA Warmth", "Clean", {
                { FLOOR_AMOUNT, 0.05f },{ FLOOR_PINK, 0.f },{ FLOOR_HIGHSHELF, 0.f },
                { FLOOR_LP, 1.f },{ MAINS_AMOUNT, 0.f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.5f },{ MAINS_H3, 0.5f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.f },{ QUANT_WAVE_STEPS, 8.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.2f },{ SAT_CMOS, 0.f },{ SAT_MIX, 0.6f },
                { DRIFT_AMOUNT, 0.f },{ DRIFT_RATE, 0.3f },
                { OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            // =========================================================
            // TAPE / VINTAGE
            // =========================================================
            v.push_back(makePreset("Tape Hiss", "Tape", {
                { FLOOR_AMOUNT, 0.35f },{ FLOOR_PINK, 1.f },{ FLOOR_HIGHSHELF, 0.5f },
                { FLOOR_LP, 0.9f },{ MAINS_AMOUNT, 0.f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.5f },{ MAINS_H3, 0.5f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.f },{ QUANT_WAVE_STEPS, 8.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.1f },{ SAT_CMOS, 0.f },{ SAT_MIX, 1.f },
                { DRIFT_AMOUNT, 0.1f },{ DRIFT_RATE, 0.1f },
                { OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Vintage Console", "Vintage", {
                { FLOOR_AMOUNT, 0.25f },{ FLOOR_PINK, 1.f },{ FLOOR_HIGHSHELF, 0.3f },
                { FLOOR_LP, 0.8f },{ MAINS_AMOUNT, 0.15f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.6f },{ MAINS_H3, 0.4f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.f },{ QUANT_WAVE_STEPS, 8.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.25f },{ SAT_CMOS, 0.f },{ SAT_MIX, 0.7f },
                { DRIFT_AMOUNT, 0.15f },{ DRIFT_RATE, 0.15f },
                { OUTPUT, -1.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Radio Broadcast", "Vintage", {
                { FLOOR_AMOUNT, 0.4f },{ FLOOR_PINK, 1.f },{ FLOOR_HIGHSHELF, 0.2f },
                { FLOOR_LP, 0.65f },{ MAINS_AMOUNT, 0.3f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.7f },{ MAINS_H3, 0.5f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.f },{ QUANT_WAVE_STEPS, 8.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.4f },{ SAT_CMOS, 0.2f },{ SAT_MIX, 0.8f },
                { DRIFT_AMOUNT, 0.3f },{ DRIFT_RATE, 0.4f },
                { OUTPUT, -2.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Old Tape Machine", "Tape", {
                { FLOOR_AMOUNT, 0.5f },{ FLOOR_PINK, 1.f },{ FLOOR_HIGHSHELF, 0.6f },
                { FLOOR_LP, 0.75f },{ MAINS_AMOUNT, 0.2f },{ MAINS_FREQ, 0.5f },
                { MAINS_H2, 0.5f },{ MAINS_H3, 0.5f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.f },{ QUANT_WAVE_STEPS, 8.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.35f },{ SAT_CMOS, 0.1f },{ SAT_MIX, 0.9f },
                { DRIFT_AMOUNT, 0.5f },{ DRIFT_RATE, 0.2f },
                { OUTPUT, -3.f },{ BYPASS, 0.f }
                }));

            // =========================================================
            // CONSOLE / MAINS CHARACTER
            // =========================================================
            v.push_back(makePreset("60 Hz Hum", "Console", {
                { FLOOR_AMOUNT, 0.f },{ FLOOR_PINK, 0.f },{ FLOOR_HIGHSHELF, 0.f },
                { FLOOR_LP, 1.f },{ MAINS_AMOUNT, 0.8f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.6f },{ MAINS_H3, 0.3f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.f },{ QUANT_WAVE_STEPS, 8.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.f },{ SAT_CMOS, 0.f },{ SAT_MIX, 1.f },
                { DRIFT_AMOUNT, 0.f },{ DRIFT_RATE, 0.3f },
                { OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("50 Hz Hum", "Console", {
                { FLOOR_AMOUNT, 0.f },{ FLOOR_PINK, 0.f },{ FLOOR_HIGHSHELF, 0.f },
                { FLOOR_LP, 1.f },{ MAINS_AMOUNT, 0.8f },{ MAINS_FREQ, 0.f },
                { MAINS_H2, 0.6f },{ MAINS_H3, 0.3f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.f },{ QUANT_WAVE_STEPS, 8.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.f },{ SAT_CMOS, 0.f },{ SAT_MIX, 1.f },
                { DRIFT_AMOUNT, 0.f },{ DRIFT_RATE, 0.3f },
                { OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Studio Floor", "Console", {
                { FLOOR_AMOUNT, 0.3f },{ FLOOR_PINK, 0.f },{ FLOOR_HIGHSHELF, 0.4f },
                { FLOOR_LP, 0.9f },{ MAINS_AMOUNT, 0.08f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.4f },{ MAINS_H3, 0.2f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.f },{ QUANT_WAVE_STEPS, 8.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.1f },{ SAT_CMOS, 0.f },{ SAT_MIX, 1.f },
                { DRIFT_AMOUNT, 0.05f },{ DRIFT_RATE, 0.2f },
                { OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            // =========================================================
            // AGGRESSIVE / BITCRUSH
            // =========================================================
            v.push_back(makePreset("Hard Bitcrush", "Aggressive", {
                { FLOOR_AMOUNT, 0.f },{ FLOOR_PINK, 0.f },{ FLOOR_HIGHSHELF, 0.f },
                { FLOOR_LP, 1.f },{ MAINS_AMOUNT, 0.f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.5f },{ MAINS_H3, 0.5f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 1.0f },{ QUANT_WAVE_STEPS, 4.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.6f },{ SAT_CMOS, 0.5f },{ SAT_MIX, 1.f },
                { DRIFT_AMOUNT, 0.f },{ DRIFT_RATE, 0.3f },
                { OUTPUT, -3.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Smooth Bitcrush", "Aggressive", {
                { FLOOR_AMOUNT, 0.f },{ FLOOR_PINK, 0.f },{ FLOOR_HIGHSHELF, 0.f },
                { FLOOR_LP, 1.f },{ MAINS_AMOUNT, 0.f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.5f },{ MAINS_H3, 0.5f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.8f },{ QUANT_WAVE_STEPS, 8.f },{ QUANT_MIX, 0.7f },
                { SAT_DRIVE, 0.3f },{ SAT_CMOS, 0.2f },{ SAT_MIX, 0.8f },
                { DRIFT_AMOUNT, 0.f },{ DRIFT_RATE, 0.3f },
                { OUTPUT, -1.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Stepped Wave", "Aggressive", {
                { FLOOR_AMOUNT, 0.1f },{ FLOOR_PINK, 0.f },{ FLOOR_HIGHSHELF, 0.f },
                { FLOOR_LP, 0.9f },{ MAINS_AMOUNT, 0.05f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.5f },{ MAINS_H3, 0.3f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.9f },{ QUANT_WAVE_STEPS, 6.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.4f },{ SAT_CMOS, 0.3f },{ SAT_MIX, 0.9f },
                { DRIFT_AMOUNT, 0.2f },{ DRIFT_RATE, 0.4f },
                { OUTPUT, -2.f },{ BYPASS, 0.f }
                }));

            // =========================================================
            // CREATIVE / SOUND DESIGN
            // =========================================================
            v.push_back(makePreset("Unstable Circuit", "Creative", {
                { FLOOR_AMOUNT, 0.4f },{ FLOOR_PINK, 1.f },{ FLOOR_HIGHSHELF, 0.5f },
                { FLOOR_LP, 0.7f },{ MAINS_AMOUNT, 0.5f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.8f },{ MAINS_H3, 0.6f },
                { QUANT_PITCH, 0.2f },{ QUANT_PITCH_STEPS, 8.f },
                { QUANT_WAVE, 0.3f },{ QUANT_WAVE_STEPS, 12.f },{ QUANT_MIX, 0.6f },
                { SAT_DRIVE, 0.5f },{ SAT_CMOS, 0.4f },{ SAT_MIX, 1.f },
                { DRIFT_AMOUNT, 0.8f },{ DRIFT_RATE, 0.6f },
                { OUTPUT, -3.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Ghost Radio", "Creative", {
                { FLOOR_AMOUNT, 0.6f },{ FLOOR_PINK, 0.f },{ FLOOR_HIGHSHELF, 0.7f },
                { FLOOR_LP, 0.4f },{ MAINS_AMOUNT, 0.6f },{ MAINS_FREQ, 0.5f },
                { MAINS_H2, 0.9f },{ MAINS_H3, 0.7f },
                { QUANT_PITCH, 0.5f },{ QUANT_PITCH_STEPS, 6.f },
                { QUANT_WAVE, 0.5f },{ QUANT_WAVE_STEPS, 6.f },{ QUANT_MIX, 0.8f },
                { SAT_DRIVE, 0.7f },{ SAT_CMOS, 0.6f },{ SAT_MIX, 1.f },
                { DRIFT_AMOUNT, 0.9f },{ DRIFT_RATE, 0.8f },
                { OUTPUT, -4.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Extreme Degradation", "Creative", {
                { FLOOR_AMOUNT, 0.7f },{ FLOOR_PINK, 1.f },{ FLOOR_HIGHSHELF, 0.9f },
                { FLOOR_LP, 0.5f },{ MAINS_AMOUNT, 0.8f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 1.0f },{ MAINS_H3, 0.8f },
                { QUANT_PITCH, 0.8f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 1.0f },{ QUANT_WAVE_STEPS, 3.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.9f },{ SAT_CMOS, 0.8f },{ SAT_MIX, 1.f },
                { DRIFT_AMOUNT, 1.0f },{ DRIFT_RATE, 0.5f },
                { OUTPUT, -6.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Slow Thermal Drift", "Creative", {
                { FLOOR_AMOUNT, 0.2f },{ FLOOR_PINK, 1.f },{ FLOOR_HIGHSHELF, 0.3f },
                { FLOOR_LP, 0.9f },{ MAINS_AMOUNT, 0.1f },{ MAINS_FREQ, 1.f },
                { MAINS_H2, 0.4f },{ MAINS_H3, 0.3f },
                { QUANT_PITCH, 0.f },{ QUANT_PITCH_STEPS, 4.f },
                { QUANT_WAVE, 0.f },{ QUANT_WAVE_STEPS, 8.f },{ QUANT_MIX, 1.f },
                { SAT_DRIVE, 0.3f },{ SAT_CMOS, 0.2f },{ SAT_MIX, 1.f },
                { DRIFT_AMOUNT, 1.0f },{ DRIFT_RATE, 0.05f },
                { OUTPUT, -1.f },{ BYPASS, 0.f }
                }));

            return v;
        }();

    return presets;
}

//==============================================================================
// Constructor
//==============================================================================
PresetManager::PresetManager(juce::AudioProcessorValueTreeState& a)
    : apvts(a)
{
    refresh();
}

//==============================================================================
// refresh
//==============================================================================
void PresetManager::refresh()
{
    presets.clear();

    // Factory presets first
    const auto& factory = getFactoryPresets();
    for (int i = 0; i < (int)factory.size(); ++i)
    {
        PresetInfo info;
        info.name = factory[(size_t)i].name;
        info.category = factory[(size_t)i].category;
        info.author = factory[(size_t)i].author;
        info.isFactory = true;
        info.factoryIndex = i;
        presets.push_back(info);
    }

    // Then user presets from disk
    scanUserPresets();
}

//==============================================================================
// scanUserPresets
//==============================================================================
void PresetManager::scanUserPresets()
{
    auto dir = getUserPresetDirectory();
    if (!dir.isDirectory()) return;

    juce::Array<juce::File> files;
    dir.findChildFiles(files, juce::File::findFiles, false, "*.chainchorus");

    // Sort alphabetically
    std::sort(files.begin(), files.end(),
        [](const juce::File& a, const juce::File& b)
        {
            return a.getFileNameWithoutExtension().compareNatural(
                b.getFileNameWithoutExtension()) < 0;
        });

    for (auto& f : files)
    {
        juce::String name, category, author;
        if (readUserPresetMetadata(f, name, category, author))
        {
            PresetInfo info;
            info.name = name;
            info.category = category;
            info.author = author;
            info.isFactory = false;
            info.factoryIndex = -1;
            info.file = f;
            presets.push_back(info);
        }
    }
}

//==============================================================================
// Query helpers
//==============================================================================
int PresetManager::getNumPresets() const
{
    return (int)presets.size();
}

const PresetManager::PresetInfo& PresetManager::getPresetInfo(int index) const
{
    jassert(index >= 0 && index < (int)presets.size());
    return presets[(size_t)index];
}

//==============================================================================
// getCurrentPresetIndex — best-effort match against current APVTS state
//==============================================================================
int PresetManager::getCurrentPresetIndex() const
{
    const auto& factory = getFactoryPresets();
    for (int i = 0; i < (int)factory.size(); ++i)
    {
        bool match = true;
        for (auto& kv : factory[(size_t)i].params)
        {
            auto* param = apvts.getParameter(kv.first);
            if (!param) { match = false; break; }
            const float natural = param->getNormalisableRange().convertFrom0to1(
                param->getValue());
            if (std::abs(natural - kv.second) > 1e-3f)
            {
                match = false;
                break;
            }
        }
        if (match) return i;
    }
    return -1;
}

//==============================================================================
// loadPreset
//==============================================================================
bool PresetManager::loadPreset(int index)
{
    if (index < 0 || index >= (int)presets.size()) return false;
    const auto& info = presets[(size_t)index];

    if (info.isFactory)
    {
        const auto& factory = getFactoryPresets();
        if (info.factoryIndex < 0 || info.factoryIndex >= (int)factory.size())
            return false;

        for (auto& kv : factory[(size_t)info.factoryIndex].params)
        {
            if (auto* p = apvts.getParameter(kv.first))
                p->setValueNotifyingHost(
                    p->getNormalisableRange().convertTo0to1(kv.second));
        }
        return true;
    }
    else
    {
        return loadPresetFromFile(info.file);
    }
}

//==============================================================================
// loadPresetFromFile
//==============================================================================
bool PresetManager::loadPresetFromFile(const juce::File& file)
{
    if (!file.existsAsFile()) return false;
    auto xml = juce::XmlDocument::parse(file);
    if (xml == nullptr) return false;
    return applyPresetXml(*xml);
}

//==============================================================================
// saveUserPreset
//==============================================================================
juce::File PresetManager::saveUserPreset(const juce::String& name,
    const juce::String& category,
    const juce::String& author,
    bool allowOverwrite)
{
    auto dir = getUserPresetDirectory();
    if (!dir.isDirectory())
        dir.createDirectory();

    auto safeName = juce::File::createLegalFileName(name);
    auto target = dir.getChildFile(safeName + ".chainchorus");

    if (target.existsAsFile() && !allowOverwrite)
        return {};

    if (auto xml = writePresetXml(name, category, author))
        xml->writeTo(target);

    refresh();
    return target;
}

//==============================================================================
// exportPresetToFile
//==============================================================================
bool PresetManager::exportPresetToFile(const juce::File& destination,
    const juce::String& name,
    const juce::String& category,
    const juce::String& author)
{
    if (auto xml = writePresetXml(name, category, author))
        return xml->writeTo(destination);
    return false;
}

//==============================================================================
// deletePreset
//==============================================================================
bool PresetManager::deletePreset(int index)
{
    if (index < 0 || index >= (int)presets.size()) return false;
    const auto& info = presets[(size_t)index];
    if (info.isFactory) return false;

    if (info.file.existsAsFile())
    {
        info.file.deleteFile();
        refresh();
        return true;
    }
    return false;
}

//==============================================================================
// getUserPresetDirectory
//==============================================================================
juce::File PresetManager::getUserPresetDirectory() const
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("AnalogImperfection")
        .getChildFile("Presets");
}

//==============================================================================
// XML serialization
//==============================================================================
void PresetManager::writeParametersToXml(juce::XmlElement& parent) const
{
    auto* paramsXml = parent.createNewChildElement("PARAMETERS");
    for (auto* param : apvts.processor.getParameters())
    {
        if (auto* p = dynamic_cast<juce::RangedAudioParameter*>(param))
        {
            auto* pXml = paramsXml->createNewChildElement("PARAM");
            pXml->setAttribute("id", p->getParameterID());
            pXml->setAttribute("value", (double)p->getValue());
        }
    }
}

void PresetManager::applyParametersFromXml(const juce::XmlElement& parent)
{
    if (auto* paramsXml = parent.getChildByName("PARAMETERS"))
    {
        for (auto* pXml : paramsXml->getChildWithTagNameIterator("PARAM"))
        {
            auto id = pXml->getStringAttribute("id");
            auto val = (float)pXml->getDoubleAttribute("value");
            if (auto* p = apvts.getParameter(id))
                p->setValueNotifyingHost(val);
        }
    }
}

//==============================================================================
// XML write / read
//==============================================================================
std::unique_ptr<juce::XmlElement> PresetManager::writePresetXml(
    const juce::String& name,
    const juce::String& category,
    const juce::String& author) const
{
    auto xml = std::make_unique<juce::XmlElement>("ANALOGIMPERFECTION_PRESET");
    xml->setAttribute("version", 1);
    xml->setAttribute("name", name);
    xml->setAttribute("category", category);
    xml->setAttribute("author", author);
    writeParametersToXml(*xml);
    return xml;
}

bool PresetManager::applyPresetXml(const juce::XmlElement& xml)
{
    if (!xml.hasTagName("ANALOGIMPERFECTION_PRESET")) return false;
    applyParametersFromXml(xml);
    return true;
}

bool PresetManager::readUserPresetMetadata(const juce::File& file,
    juce::String& name,
    juce::String& category,
    juce::String& author) const
{
    auto xml = juce::XmlDocument::parse(file);
    if (xml == nullptr) return false;
    if (!xml->hasTagName("ANALOGIMPERFECTION_PRESET")) return false;

    name = xml->getStringAttribute("name", file.getFileNameWithoutExtension());
    category = xml->getStringAttribute("category", "User");
    author = xml->getStringAttribute("author", "User");
    return true;
}