#pragma once

#include <string>
#include <vector>
#include <functional>
#include "hotkey_definitions.h"
#include "nlohmann/json.hpp"
#include "tinyxml2/tinyxml2.h"

struct JsonPosition {
    std::string name;
    float x;
    float y;
    float z;
};

struct XmlPosition {
    std::string name = "empty";
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
    std::vector<XmlPosition> m_xmlPositions; // Stores positions loaded from XML file
    std::string m_currentJsonFilePath; // Current selected JSON file path
    std::string m_currentXmlFilePath; // Current selected XML file path
    int m_selectedPositionIndex = 0; // Current selected position index
    int m_selectedXmlPositionIndex = 0; // Current selected XML position index

    // Json位置载入方法
    bool loadJsonPositions(const std::string& filePath); // Loads positions from JSON file
    void openJsonFileDialog(); // Opens file dialog to select JSON file
    void loadNextPosition(); // Load next position from JSON
    void loadPreviousPosition(); // Load previous position from JSON

    // Xml位置载入方法
    bool loadXmlPositions(const std::string& filePath); // Loads positions from XML file
    void openXmlFileDialog(); // Opens file dialog to select XML file
    void loadNextXmlPosition(); // Load next position from XML
    void loadPreviousXmlPosition(); // Load previous position from XML

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