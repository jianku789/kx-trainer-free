#pragma once

#include <string>
#include <vector>
#include <functional>
#include "hotkey_definitions.h"

class Hack;

class HackGUI {
public:
    HackGUI(Hack& hack);  // 构造函数，初始化HackGUI对象
    bool renderUI();      // 主渲染函数，用于绘制GUI界面

private:
    Hack& m_hack;  // 引用Hack类实例，用于访问游戏功能

    // 仅包含GUI特定的状态或用户偏好设置
    bool m_sprintEnabled = false; // 用户的冲刺模式偏好切换

    // 重构的热键管理
    std::vector<HotkeyInfo> m_hotkeys;  // 存储所有热键信息的向量
    HotkeyID m_rebinding_hotkey_id = HotkeyID::NONE; // 当前正在重新绑定的热键ID

    // UI渲染方法
    void RenderAlwaysOnTop();      // 渲染"总是置顶"复选框
    void RenderTogglesSection();   // 渲染切换功能部分
    void RenderActionsSection();   // 渲染操作按钮部分
    void RenderHotkeysSection();   // 渲染热键配置部分
    void RenderLogSection();       // 渲染日志显示部分
    void RenderInfoSection();      // 渲染信息/关于部分

    // 逻辑处理方法
    void HandleHotkeys();          // 处理热键检测和执行
    void HandleHotkeyRebinding();  // 处理热键重新绑定逻辑

    // 辅助方法
    void RenderHotkeyControl(HotkeyInfo& hotkey);  // 渲染单个热键控制项
};