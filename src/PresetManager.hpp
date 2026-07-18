#pragma once
#include <Geode/Geode.hpp>
#include <map>
#include <string>
#include <sstream>
#include <vector>

using namespace geode::prelude;

struct SkinPreset {
    int cube;
    int ship;
    int ball;
    int ufo;
    int wave;
    int robot;
    int spider;
    int swing;
    int color1;
    int color2;
    int glowColor;
    bool glow;
    int deathEffect;
};

class PresetManager {
public:
    static std::map<std::string, SkinPreset> getPresets() {
        std::map<std::string, SkinPreset> res;


        std::string indexStr = Mod::get()->getSavedValue<std::string>("presets_index");
        std::stringstream ss(indexStr);
        std::string name;

        while (std::getline(ss, name, '|')) {
            if (name.empty()) continue;


            SkinPreset preset;
            preset.cube = Mod::get()->getSavedValue<int>("preset_" + name + "_cube");
            preset.ship = Mod::get()->getSavedValue<int>("preset_" + name + "_ship");
            preset.ball = Mod::get()->getSavedValue<int>("preset_" + name + "_ball");
            preset.ufo = Mod::get()->getSavedValue<int>("preset_" + name + "_ufo");
            preset.wave = Mod::get()->getSavedValue<int>("preset_" + name + "_wave");
            preset.robot = Mod::get()->getSavedValue<int>("preset_" + name + "_robot");
            preset.spider = Mod::get()->getSavedValue<int>("preset_" + name + "_spider");
            preset.swing = Mod::get()->getSavedValue<int>("preset_" + name + "_swing");
            preset.color1 = Mod::get()->getSavedValue<int>("preset_" + name + "_color1");
            preset.color2 = Mod::get()->getSavedValue<int>("preset_" + name + "_color2");
            preset.glowColor = Mod::get()->getSavedValue<int>("preset_" + name + "_glowColor");
            preset.glow = Mod::get()->getSavedValue<bool>("preset_" + name + "_glow");
            preset.deathEffect = Mod::get()->getSavedValue<int>("preset_" + name + "_deathEffect");

            res[name] = preset;
        }

        return res;
    }

    static void savePreset(std::string const& name, SkinPreset const& preset) {

        Mod::get()->setSavedValue("preset_" + name + "_cube", preset.cube);
        Mod::get()->setSavedValue("preset_" + name + "_ship", preset.ship);
        Mod::get()->setSavedValue("preset_" + name + "_ball", preset.ball);
        Mod::get()->setSavedValue("preset_" + name + "_ufo", preset.ufo);
        Mod::get()->setSavedValue("preset_" + name + "_wave", preset.wave);
        Mod::get()->setSavedValue("preset_" + name + "_robot", preset.robot);
        Mod::get()->setSavedValue("preset_" + name + "_spider", preset.spider);
        Mod::get()->setSavedValue("preset_" + name + "_swing", preset.swing);
        Mod::get()->setSavedValue("preset_" + name + "_color1", preset.color1);
        Mod::get()->setSavedValue("preset_" + name + "_color2", preset.color2);
        Mod::get()->setSavedValue("preset_" + name + "_glowColor", preset.glowColor);
        Mod::get()->setSavedValue("preset_" + name + "_glow", preset.glow);
        Mod::get()->setSavedValue("preset_" + name + "_deathEffect", preset.deathEffect);


        auto presets = getPresets();
        presets[name] = preset;

        std::string newIndex = "";
        for (auto const& pair : presets) {
            newIndex += pair.first + "|";
        }
        Mod::get()->setSavedValue("presets_index", newIndex);
    }

    static void deletePreset(std::string const& name) {

        auto presets = getPresets();
        presets.erase(name);

        std::string newIndex = "";
        for (auto const& pair : presets) {
            newIndex += pair.first + "|";
        }
        Mod::get()->setSavedValue("presets_index", newIndex);
    }
};