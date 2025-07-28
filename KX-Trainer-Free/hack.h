#pragma once

#include "process_memory_manager.h"
#include <Windows.h>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
#include <array>

// 添加位置数据结构
struct Position {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    
    Position() = default;
    Position(float x, float y, float z) : x(x), y(y), z(z) {}
};

class HackInitializationError : public std::runtime_error {
public:
    HackInitializationError(const std::string& message)
        : std::runtime_error(message) {}
};

class Hack {
public:
    static const int MAX_POSITIONS = 10; // 支持最多10个位置
    
    Hack(std::function<void(const std::string&)> statusCallback);
    ~Hack();
    bool Initialize(); // Performs process attachment and initial scans. Returns true on success.

    void refreshAddresses();

    // Feature toggles/handlers
    void toggleFog(bool enable);
    void toggleObjectClipping(bool enable);
    void toggleFullStrafe(bool enable);
    void handleSprint(bool userPrefersSprint); // Apply sprint based on user preference
    void handleSuperSprint(bool enable);
    void toggleInvisibility(bool enable);
    void toggleWallClimb(bool enable);
    void toggleClipping(bool enable);
    void handleFly(bool enable);

    // Position saving/loading
    void savePosition();
    void loadPosition();
    
    // 多位置功能
    void savePosition(int slot);
    void loadPosition(int slot);
    bool isValidPosition(int slot) const;
    Position getPosition(int slot) const;
    
    // --- State Getters ---
    bool IsFogEnabled() const;
    bool IsObjectClippingEnabled() const;
    bool IsFullStrafeEnabled() const;
    bool IsSuperSprinting() const; // Reflects if key was held/active
    bool IsInvisibilityEnabled() const;
    bool IsWallClimbEnabled() const;
    bool IsClippingEnabled() const;
    bool IsFlying() const;         // Reflects if key was held/active
    // --- End State Getters ---

private:
    ProcessMemoryManager m_memoryManager;
    std::function<void(const std::string&)> m_statusCallback;

    uintptr_t m_baseAddressLocation = 0;
    uintptr_t m_fogAddress = 0;
    uintptr_t m_objectClippingAddress = 0;
    uintptr_t m_fullStrafeAddress = 0;

    // Pointer chain offsets
    std::vector<unsigned int> m_xOffsets;
    std::vector<unsigned int> m_yOffsets;
    std::vector<unsigned int> m_zOffsets;
    std::vector<unsigned int> m_zHeight1Offsets; // Invisibility
    std::vector<unsigned int> m_zHeight2Offsets; // Clipping
    std::vector<unsigned int> m_gravityOffsets;    // Fly
    std::vector<unsigned int> m_speedOffsets;
    std::vector<unsigned int> m_wallClimbOffsets;

    // Resolved dynamic addresses
    uintptr_t m_xAddr = 0;
    uintptr_t m_yAddr = 0;
    uintptr_t m_zAddr = 0;
    uintptr_t m_zHeight1Addr = 0; // Invisibility
    uintptr_t m_zHeight2Addr = 0; // Clipping
    uintptr_t m_gravityAddr = 0;    // Fly
    uintptr_t m_speedAddr = 0;
    uintptr_t m_wallClimbAddr = 0;

    // Core memory values / state
    float m_xValue = 0.0f, m_yValue = 0.0f, m_zValue = 0.0f;
    float m_xSave = 0.0f, m_ySave = 0.0f, m_zSave = 0.0f;
    
    // 多位置支持
    std::array<Position, MAX_POSITIONS> m_savedPositions;
    std::array<bool, MAX_POSITIONS> m_validPositions;
    
    float m_speed = 0.0f, m_savedSpeed = 0.0f;
    float m_invisibilityValue = 0.0f;
    float m_wallClimbValue = 0.0f;
    float m_clippingValue = 0.0f;
    float m_flyValue = 0.0f;
    byte m_objectClippingByte = 0; // Cached byte written
    byte m_fogByte = 0;            // Cached byte written
    byte m_fullStrafeByte = 0;     // Cached byte written

    // Simple feature active flags (Single Source of Truth)
    bool m_isInvisibilityActive = false;
    bool m_isClippingActive = false;
    bool m_isWallClimbActive = false;
    bool m_isFlyingActive = false;
    bool m_wasSuperSprinting = false; // Tracks hold state from last frame
    bool m_wasSprinting = false;      // Tracks if normal sprint *logic* was applied last frame

    // Helpers
    void initializeOffsets();
    void findProcess();
    void performBaseScan();
    void scanForPatterns();
    void readXYZ();
    void writeXYZ(float xValue, float yValue, float zValue);
    uintptr_t refreshAddr(const std::vector<unsigned int>& offsets);
    void reportStatus(const std::string& message);
};