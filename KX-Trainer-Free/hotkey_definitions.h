#pragma once

#include <functional>
#include <utility>

class Hack; // 为动作函数签名所需的前向声明

// 热键ID枚举，定义所有可用的热键
enum class HotkeyID {
    NONE = -1, // 表示没有热键正在重新绑定
    SAVE_POS,  // 保存位置
    LOAD_POS,  // 加载位置
    TOGGLE_INVISIBILITY,  // 切换隐身
    TOGGLE_WALLCLIMB,     // 切换爬墙
    TOGGLE_CLIPPING,      // 切换穿墙
    TOGGLE_OBJECT_CLIPPING, // 切换对象穿墙
    TOGGLE_FULL_STRAFE,   // 切换全速移动
    TOGGLE_NO_FOG,        // 切换无雾
    HOLD_SUPER_SPRINT,    // 超级冲刺（按住）
    TOGGLE_SPRINT_PREF,   // 切换GUI偏好设置中的冲刺标志
    HOLD_FLY,             // 飞行（按住）
    // 多位置加载热键
    LOAD_POS_SLOT_0,      // 加载槽位0
    LOAD_POS_SLOT_1,      // 加载槽位1
    LOAD_POS_SLOT_2,      // 加载槽位2
    LOAD_POS_SLOT_3,      // 加载槽位3
    LOAD_POS_SLOT_4       // 加载槽位4
};

// 热键触发类型枚举
enum class HotkeyTriggerType {
    ON_PRESS, // 按下时触发一次
    ON_HOLD   // 按住时持续触发
};

// 热键信息结构体，存储单个热键的所有相关信息
struct HotkeyInfo {
    HotkeyID id;           // 热键ID
    const char* name;      // 用户可见的名称（例如"保存位置"）
    int defaultKeyCode;    // 默认虚拟键码，来自Constants::Hotkeys
    int currentKeyCode;    // 当前分配的虚拟键码（0表示未绑定）
    int modifierKeys;      // 修饰键（Ctrl, Alt, Shift）- 位掩码
    HotkeyTriggerType triggerType;  // 触发类型
    std::function<void(Hack&, bool)> action; // 动作函数，bool参数表示按下/按住状态

    // 默认构造函数，如果不用emplace_back初始化所有参数则需要
    HotkeyInfo() : id(HotkeyID::NONE), name(""), defaultKeyCode(0), currentKeyCode(0), modifierKeys(0), triggerType(HotkeyTriggerType::ON_PRESS), action(nullptr) {}

    // 带修饰键的构造函数，便于初始化
    HotkeyInfo(HotkeyID _id, const char* _name, int _defaultKey, int _modifierKeys, HotkeyTriggerType _type, std::function<void(Hack&, bool)> _action)
        : id(_id), name(_name), defaultKeyCode(_defaultKey), currentKeyCode(0), modifierKeys(_modifierKeys), // 默认未绑定（0）
          triggerType(_type), action(std::move(_action)) {}
          
    // 向后兼容的构造函数（不带修饰键）
    HotkeyInfo(HotkeyID _id, const char* _name, int _defaultKey, HotkeyTriggerType _type, std::function<void(Hack&, bool)> _action)
        : id(_id), name(_name), defaultKeyCode(_defaultKey), currentKeyCode(0), modifierKeys(0), // 默认未绑定（0）
          triggerType(_type), action(std::move(_action)) {}
};
