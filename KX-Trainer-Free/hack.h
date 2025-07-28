#pragma once

#include "process_memory_manager.h"
#include <Windows.h>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
#include <array>

// 位置数据结构
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
}; // 黑客初始化错误类

class Hack {
public:
    static const int MAX_POSITIONS = 10; // 支持最多10个位置
    
    Hack(std::function<void(const std::string&)> statusCallback);
    ~Hack();
    bool Initialize(); // 执行进程附加和初始扫描，成功时返回true

    void refreshAddresses(); // 刷新地址

    // 功能切换/处理程序
    void toggleFog(bool enable);              // 切换雾效
    void toggleObjectClipping(bool enable);   // 切换对象碰撞
    void toggleFullStrafe(bool enable);       // 切换全方向扫射
    void handleSprint(bool userPrefersSprint); // 根据用户偏好应用冲刺
    void handleSuperSprint(bool enable);      // 处理超级冲刺
    void toggleInvisibility(bool enable);     // 切换隐身
    void toggleWallClimb(bool enable);        // 切换墙壁攀爬
    void toggleClipping(bool enable);         // 切换碰撞检测
    void handleFly(bool enable);              // 处理飞行

    // 位置保存/加载
    void savePosition();  // 保存位置
    void loadPosition();  // 加载位置
    
    // 多位置功能
    void savePosition(int slot);      // 保存位置到指定槽位
    void loadPosition(int slot);      // 从指定槽位加载位置
    bool isValidPosition(int slot) const;  // 检查位置是否有效
    Position getPosition(int slot) const;  // 获取指定槽位的位置
    
    // --- 状态获取器 ---
    bool IsFogEnabled() const;            // 雾效是否启用
    bool IsObjectClippingEnabled() const; // 对象碰撞是否启用
    bool IsFullStrafeEnabled() const;     // 全方向扫射是否启用
    bool IsSuperSprinting() const;        // 是否正在超级冲刺
    bool IsInvisibilityEnabled() const;   // 隐身是否启用
    bool IsWallClimbEnabled() const;      // 墙壁攀爬是否启用
    bool IsClippingEnabled() const;       // 碰撞检测是否启用
    bool IsFlying() const;                // 是否正在飞行
    // --- 状态获取器结束 ---

private:
    ProcessMemoryManager m_memoryManager;  // 进程内存管理器
    std::function<void(const std::string&)> m_statusCallback;  // 状态回调函数

    uintptr_t m_baseAddressLocation = 0;   // 基地址位置
    uintptr_t m_fogAddress = 0;            // 雾效地址
    uintptr_t m_objectClippingAddress = 0; // 对象碰撞地址
    uintptr_t m_fullStrafeAddress = 0;     // 全方向扫射地址

    // 指针链偏移量
    std::vector<unsigned int> m_xOffsets;          // X坐标偏移量
    std::vector<unsigned int> m_yOffsets;          // Y坐标偏移量
    std::vector<unsigned int> m_zOffsets;          // Z坐标偏移量
    std::vector<unsigned int> m_zHeight1Offsets;   // 隐身偏移量
    std::vector<unsigned int> m_zHeight2Offsets;   // 碰撞检测偏移量
    std::vector<unsigned int> m_gravityOffsets;    // 飞行偏移量
    std::vector<unsigned int> m_speedOffsets;      // 速度偏移量
    std::vector<unsigned int> m_wallClimbOffsets;  // 墙壁攀爬偏移量

    // 解析的动态地址
    uintptr_t m_xAddr = 0;          // X坐标地址
    uintptr_t m_yAddr = 0;          // Y坐标地址
    uintptr_t m_zAddr = 0;          // Z坐标地址
    uintptr_t m_zHeight1Addr = 0;   // 隐身地址
    uintptr_t m_zHeight2Addr = 0;   // 碰撞检测地址
    uintptr_t m_gravityAddr = 0;    // 飞行地址
    uintptr_t m_speedAddr = 0;      // 速度地址
    uintptr_t m_wallClimbAddr = 0;  // 墙壁攀爬地址

    // 核心内存值/状态
    float m_xValue = 0.0f, m_yValue = 0.0f, m_zValue = 0.0f;  // 当前XYZ坐标值
    float m_xSave = 0.0f, m_ySave = 0.0f, m_zSave = 0.0f;     // 保存的XYZ坐标值
    
    // 多位置支持
    std::array<Position, MAX_POSITIONS> m_savedPositions;  // 保存的位置数组
    std::array<bool, MAX_POSITIONS> m_validPositions;      // 位置有效性数组
    
    float m_speed = 0.0f, m_savedSpeed = 0.0f;      // 当前速度和保存的速度
    float m_invisibilityValue = 0.0f;               // 隐身值
    float m_wallClimbValue = 0.0f;                  // 墙壁攀爬值
    float m_clippingValue = 0.0f;                   // 碰撞检测值
    float m_flyValue = 0.0f;                        // 飞行值
    byte m_objectClippingByte = 0;                  // 缓存的对象碰撞字节
    byte m_fogByte = 0;                             // 缓存的雾效字节
    byte m_fullStrafeByte = 0;                      // 缓存的全方向扫射字节

    // 简单功能激活标志（单一数据源）
    bool m_isInvisibilityActive = false;   // 隐身是否激活
    bool m_isClippingActive = false;       // 碰撞检测是否激活
    bool m_isWallClimbActive = false;      // 墙壁攀爬是否激活
    bool m_isFlyingActive = false;         // 飞行是否激活
    bool m_wasSuperSprinting = false;      // 跟踪上一帧的超级冲刺状态
    bool m_wasSprinting = false;           // 跟踪上一帧是否应用了普通冲刺逻辑

    // 辅助函数
    void initializeOffsets();               // 初始化偏移量
    void findProcess();                     // 查找进程
    void performBaseScan();                 // 执行基地址扫描
    void scanForPatterns();                 // 扫描模式
    void readXYZ();                         // 读取XYZ坐标
    void writeXYZ(float xValue, float yValue, float zValue);  // 写入XYZ坐标
    uintptr_t refreshAddr(const std::vector<unsigned int>& offsets);  // 刷新地址
    void reportStatus(const std::string& message);  // 报告状态
};