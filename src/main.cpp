#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include "PresetManager.hpp"

using namespace geode::prelude;

class SkinPresetPopup : public FLAlertLayer {
protected:
    CCTextInputNode* m_inputNode;
    ScrollLayer* m_scrollLayer = nullptr;
    CCMenu* m_listMenu = nullptr;
    std::vector<std::string*> m_allocatedNames;

    bool init() {
        if (!FLAlertLayer::init(180)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();


        auto bg = CCScale9Sprite::create("GJ_square01.png", { 0, 0, 80, 80 });
        bg->setContentSize({ 380.f, 280.f });
        bg->setPosition(winSize / 2);
        m_mainLayer->addChild(bg);

        auto menu = CCMenu::create();
        m_mainLayer->addChild(menu);


        auto title = CCLabelBMFont::create("Skin Presets", "bigFont.fnt");
        title->setPosition({ winSize.width / 2, winSize.height / 2 + 120.f });
        title->setScale(0.8f);
        m_mainLayer->addChild(title);


        auto closeBtnSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
        auto closeBtn = CCMenuItemSpriteExtra::create(
            closeBtnSprite,
            this,
            menu_selector(SkinPresetPopup::onClose)
        );
        closeBtn->setPosition({ -180.f, 130.f });
        menu->addChild(closeBtn);


        m_inputNode = CCTextInputNode::create(160.f, 20.f, "Preset Name", "bigFont.fnt");
        m_inputNode->setPosition({ winSize.width / 2 - 40.f, winSize.height / 2 + 80.f });
        m_inputNode->setAllowedChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ");
        m_inputNode->setMaxLabelLength(12);
        m_mainLayer->addChild(m_inputNode);


        auto saveBtnSprite = ButtonSprite::create("Save", "goldFont.fnt", "GJ_button_02.png", .6f);
        auto saveBtn = CCMenuItemSpriteExtra::create(
            saveBtnSprite,
            this,
            menu_selector(SkinPresetPopup::onSavePreset)
        );
        saveBtn->setPosition({ 110.f, 80.f });
        menu->addChild(saveBtn);


        auto listBg = CCScale9Sprite::create("GJ_square02.png", { 0, 0, 80, 80 });
        listBg->setContentSize({ 340.f, 150.f });
        listBg->setPosition({ winSize.width / 2, winSize.height / 2 - 25.f });
        m_mainLayer->addChild(listBg);


        m_scrollLayer = ScrollLayer::create({ 330.f, 140.f });
        m_scrollLayer->setPosition({ winSize.width / 2 - 165.f, winSize.height / 2 - 95.f });
        m_mainLayer->addChild(m_scrollLayer);


        m_listMenu = CCMenu::create();
        m_listMenu->setPosition({ 0, 0 });
        m_scrollLayer->m_contentLayer->addChild(m_listMenu);

        refreshList();
        return true;
    }

    void clearAllocatedNames() {
        for (auto ptr : m_allocatedNames) {
            delete ptr;
        }
        m_allocatedNames.clear();
    }

    void refreshList() {
        if (!m_listMenu || !m_scrollLayer) return;

        m_listMenu->removeAllChildren();
        clearAllocatedNames();

        auto presets = PresetManager::getPresets();
        float itemHeight = 40.f;
        float totalHeight = std::max(140.f, presets.size() * itemHeight);


        m_scrollLayer->m_contentLayer->setContentSize({ 330.f, totalHeight });
        m_listMenu->setContentSize({ 330.f, totalHeight });

        float yOffset = totalHeight - 22.f;

        if (presets.empty()) {
            auto noPresetLabel = CCLabelBMFont::create("No presets saved", "goldFont.fnt");
            noPresetLabel->setScale(0.6f);
            noPresetLabel->setPosition({ 165.f, totalHeight / 2 });
            m_listMenu->addChild(noPresetLabel);
        }
        else {
            auto gm = GameManager::sharedState();

            for (auto const& [name, preset] : presets) {

                auto cellBg = CCScale9Sprite::create("GJ_square05.png", { 0, 0, 80, 80 });
                cellBg->setContentSize({ 320.f, 32.f });
                cellBg->setPosition({ 165.f, yOffset });
                m_listMenu->addChild(cellBg);

                auto playerIcon = SimplePlayer::create(preset.cube);
                playerIcon->updatePlayerFrame(preset.cube, IconType::Cube);
                playerIcon->setColor(gm->colorForIdx(preset.color1));
                playerIcon->setSecondColor(gm->colorForIdx(preset.color2));

                if (preset.glow) {
                    playerIcon->setGlowOutline(gm->colorForIdx(preset.glowColor));
                }
                else {
                    playerIcon->setGlowOutline({ 0, 0, 0 });
                }

                playerIcon->setScale(0.5f);
                playerIcon->setPosition({ 25.f, yOffset });
                m_listMenu->addChild(playerIcon);

                auto nameLabel = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
                nameLabel->setScale(0.4f);
                nameLabel->setAnchorPoint({ 0.f, 0.5f });
                nameLabel->setPosition({ 45.f, yOffset });
                m_listMenu->addChild(nameLabel);

                std::string* namePtr = new std::string(name);
                m_allocatedNames.push_back(namePtr);

                auto loadBtnSprite = ButtonSprite::create("Load", "bigFont.fnt", "GJ_button_01.png", .4f);
                auto loadBtn = CCMenuItemSpriteExtra::create(
                    loadBtnSprite,
                    this,
                    menu_selector(SkinPresetPopup::onLoadPreset)
                );
                loadBtn->setUserData(namePtr);
                loadBtn->setPosition({ 235.f, yOffset });
                m_listMenu->addChild(loadBtn);

                auto deleteBtnSprite = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
                deleteBtnSprite->setScale(0.65f);
                auto deleteBtn = CCMenuItemSpriteExtra::create(
                    deleteBtnSprite,
                    this,
                    menu_selector(SkinPresetPopup::onDeletePreset)
                );
                deleteBtn->setUserData(namePtr);
                deleteBtn->setPosition({ 295.f, yOffset });
                m_listMenu->addChild(deleteBtn);

                yOffset -= itemHeight;
            }
        }

        m_scrollLayer->moveToTop();
    }

    void onSavePreset(CCObject*) {
        std::string name = m_inputNode->getString();
        if (name.empty()) return;

        auto gm = GameManager::sharedState();
        SkinPreset preset{
            .cube = gm->m_playerFrame,
            .ship = gm->m_playerShip,
            .ball = gm->m_playerBall,
            .ufo = gm->m_playerBird,
            .wave = gm->m_playerDart,
            .robot = gm->m_playerRobot,
            .spider = gm->m_playerSpider,
            .swing = gm->m_playerSwing,
            .color1 = gm->m_playerColor,
            .color2 = gm->m_playerColor2,
            .glowColor = gm->m_playerGlowColor,
            .glow = gm->m_playerGlow,
            .deathEffect = gm->m_playerDeathEffect
        };

        PresetManager::savePreset(name, preset);
        m_inputNode->setString("");
        refreshList();
    }

    void onLoadPreset(CCObject* sender) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto namePtr = static_cast<std::string*>(btn->getUserData());
        if (!namePtr) return;

        auto presets = PresetManager::getPresets();
        if (presets.find(*namePtr) != presets.end()) {
            auto preset = presets[*namePtr];
            auto gm = GameManager::sharedState();

            gm->m_playerFrame = preset.cube;
            gm->m_playerShip = preset.ship;
            gm->m_playerBall = preset.ball;
            gm->m_playerBird = preset.ufo;
            gm->m_playerDart = preset.wave;
            gm->m_playerRobot = preset.robot;
            gm->m_playerSpider = preset.spider;
            gm->m_playerSwing = preset.swing;
            gm->m_playerColor = preset.color1;
            gm->m_playerColor2 = preset.color2;
            gm->m_playerGlowColor = preset.glowColor;
            gm->m_playerGlow = preset.glow;
            gm->m_playerDeathEffect = preset.deathEffect;

            FLAlertLayer::create("Loaded", (*namePtr + " applied!").c_str(), "OK")->show();
        }
    }

    void onDeletePreset(CCObject* sender) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto namePtr = static_cast<std::string*>(btn->getUserData());
        if (!namePtr) return;

        PresetManager::deletePreset(*namePtr);
        refreshList();
    }

    void onClose(CCObject*) {
        clearAllocatedNames();
        this->removeFromParentAndCleanup(true);
    }

public:
    static SkinPresetPopup* create() {
        auto ret = new SkinPresetPopup();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

class $modify(MyGarageLayer, GJGarageLayer) {
    bool init() override {
        if (!GJGarageLayer::init()) return false;

        auto buttonSprite = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
        buttonSprite->setScale(0.75f);

        auto button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(MyGarageLayer::openPresets)
        );

        auto menu = this->getChildByType<CCMenu>(0);
        if (menu) {
            menu->addChild(button);
            button->setID("skin-presets-button"_spr);

            button->setPosition({ 135.f, 100.f });
            menu->updateLayout();
        }

        return true;
    }

    void openPresets(CCObject*) {
        auto popup = SkinPresetPopup::create();
        popup->m_noElasticity = true;
        popup->show();
    }
};