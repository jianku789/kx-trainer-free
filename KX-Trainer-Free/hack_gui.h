#pragma once

#include <string>
#include <vector>
#include <functional>
#include "hotkey_definitions.h"
#include "nlohmann/json.hpp"

struct JsonPosition {
    std::string name;
    float x;
    float y;
    float z;
};

using json = nlohmann::json;

class Hack;

class HackGUI {
public:
    HackGUI(Hack& hack);
    bool renderUI();

private:
    Hack& m_hack;

    // Only GUI-specific state or user preferences here
    bool m_sprintEnabled = false; // User's preference toggle for sprint mode

    // Refactored Hotkey Management
    std::vector<HotkeyInfo> m_hotkeys;
    HotkeyID m_rebinding_hotkey_id = HotkeyID::NONE; // ID of the hotkey currently being rebound
    std::vector<JsonPosition> m_jsonPositions; // Stores positions loaded from JSON file
    std::string m_currentJsonFilePath; // Current selected JSON file path
    bool loadJsonPositions(const std::string& filePath); // Loads positions from JSON file
    void openJsonFileDialog(); // Opens file dialog to select JSON file

    // UI Rendering Methods
    void RenderAlwaysOnTop();
    void RenderTogglesSection();
    void RenderActionsSection();
    void RenderHotkeysSection();
    void RenderLogSection();
    void RenderInfoSection();

    // Logic Handling Methods
    void HandleHotkeys();
    void HandleHotkeyRebinding();

    // Helpers
    void RenderHotkeyControl(HotkeyInfo& hotkey);
};