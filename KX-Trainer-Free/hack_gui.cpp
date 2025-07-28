#include "hack_gui.h"
#include "hack.h"
#include "constants.h"
#include "status_ui.h"
#include "key_utils.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <mutex>

HackGUI::HackGUI(Hack& hack) : m_hack(hack), m_rebinding_hotkey_id(HotkeyID::NONE) {
    // 定义所有可用的热键及其默认属性
    m_hotkeys = {
        {HotkeyID::SAVE_POS,             "Save Position",   Constants::Hotkeys::KEY_SAVEPOS,         HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.savePosition(); }},  // 保存位置
        {HotkeyID::LOAD_POS,             "Load Position",   Constants::Hotkeys::KEY_LOADPOS,         HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.loadPosition(); }},  // 加载位置
        {HotkeyID::TOGGLE_INVISIBILITY,  "Invisibility",    Constants::Hotkeys::KEY_INVISIBILITY,    HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.toggleInvisibility(!h.IsInvisibilityEnabled()); }},  // 切换隐身
        {HotkeyID::TOGGLE_WALLCLIMB,     "Wall Climb",      Constants::Hotkeys::KEY_WALLCLIMB,       HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.toggleWallClimb(!h.IsWallClimbEnabled()); }},  // 切换爬墙
        {HotkeyID::TOGGLE_CLIPPING,      "Clipping",        Constants::Hotkeys::KEY_CLIPPING,        HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.toggleClipping(!h.IsClippingEnabled()); }},  // 切换穿墙
        {HotkeyID::TOGGLE_OBJECT_CLIPPING,"Object Clipping", Constants::Hotkeys::KEY_OBJECT_CLIPPING, HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.toggleObjectClipping(!h.IsObjectClippingEnabled()); }},  // 切换对象穿墙
        {HotkeyID::TOGGLE_FULL_STRAFE,   "Full Strafe",     Constants::Hotkeys::KEY_FULL_STRAFE,     HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.toggleFullStrafe(!h.IsFullStrafeEnabled()); }},  // 切换全速移动
        {HotkeyID::TOGGLE_NO_FOG,        "No Fog",          Constants::Hotkeys::KEY_NO_FOG,          HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.toggleFog(!h.IsFogEnabled()); }},  // 切换无雾
        {HotkeyID::HOLD_SUPER_SPRINT,    "Super Sprint",    Constants::Hotkeys::KEY_SUPER_SPRINT,    HotkeyTriggerType::ON_HOLD,  [](Hack& h, bool held) { h.handleSuperSprint(held); }},  // 超级冲刺（按住）
        {HotkeyID::TOGGLE_SPRINT_PREF,   "Sprint",          Constants::Hotkeys::KEY_SPRINT,          HotkeyTriggerType::ON_PRESS, [this](Hack& /*h*/, bool) { this->m_sprintEnabled = !this->m_sprintEnabled; }}, // 切换GUI偏好设置中的冲刺标志
        {HotkeyID::HOLD_FLY,             "Fly",             Constants::Hotkeys::KEY_FLY,             HotkeyTriggerType::ON_HOLD,  [](Hack& h, bool held) { h.handleFly(held); }},  // 飞行（按住）
        // 多位置加载热键
        {HotkeyID::LOAD_POS_SLOT_0,      "Load Slot 0",     Constants::Hotkeys::KEY_LOAD_POS_SLOT_0, 1|2, HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.loadPosition(0); }}, // Ctrl+Alt+F1
        {HotkeyID::LOAD_POS_SLOT_1,      "Load Slot 1",     Constants::Hotkeys::KEY_LOAD_POS_SLOT_1, 1|2, HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.loadPosition(1); }}, // Ctrl+Alt+F2
        {HotkeyID::LOAD_POS_SLOT_2,      "Load Slot 2",     Constants::Hotkeys::KEY_LOAD_POS_SLOT_2, HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.loadPosition(2); }},  // 加载槽位2
        {HotkeyID::LOAD_POS_SLOT_3,      "Load Slot 3",     Constants::Hotkeys::KEY_LOAD_POS_SLOT_3, HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.loadPosition(3); }},  // 加载槽位3
        {HotkeyID::LOAD_POS_SLOT_4,      "Load Slot 4",     Constants::Hotkeys::KEY_LOAD_POS_SLOT_4, HotkeyTriggerType::ON_PRESS, [](Hack& h, bool) { h.loadPosition(4); }}   // 加载槽位4
    };

    // TODO: 从配置文件加载保存的currentKeyCode值，覆盖HotkeyInfo构造函数中设置的默认值
}

// 辅助函数，用于渲染单个热键控件
void HackGUI::RenderHotkeyControl(HotkeyInfo& hotkey) {
    // 计算按钮宽度
    float button_width = ImGui::GetContentRegionAvail().x * 0.3f;
    ImVec2 button_size = ImVec2(button_width, 0);

    // 显示热键名称
    ImGui::Text("%s", hotkey.name);
    ImGui::SameLine();

    // 创建键位绑定显示按钮
    std::string buttonText = GetHotkeyDisplayString(hotkey.currentKeyCode, hotkey.modifierKeys);
    if (ImGui::Button(buttonText.c_str(), button_size)) {
        // 点击时，开始重新绑定此热键
        m_rebinding_hotkey_id = hotkey.id;
    }
    ImGui::SameLine();

    // "解绑" 按钮
    if (ImGui::Button("Unbind", ImVec2(button_width * 0.5f, 0))) {
        hotkey.currentKeyCode = 0; // 0 表示未绑定
        // TODO: 在此处将更新后的热键保存到配置文件
    }
    ImGui::SameLine();

    // "重置为默认" 按钮
    if (ImGui::Button("Default", ImVec2(-1.0f, 0))) {
        hotkey.currentKeyCode = hotkey.defaultKeyCode;
        hotkey.modifierKeys = hotkey.defaultModifiers;
        // TODO: 在此处将更新后的热键保存到配置文件
    }
}

// 渲染"总是置顶"复选框并应用设置
void HackGUI::RenderAlwaysOnTop() {
    static bool always_on_top_checkbox = false;  // 复选框状态
    static bool current_window_is_topmost = false;  // 当前窗口是否置顶
    HWND current_window_hwnd = nullptr;  // 当前窗口句柄

    // 获取当前ImGui窗口的HWND
    ImGuiWindow* current_imgui_win = ImGui::GetCurrentWindowRead();
    if (current_imgui_win && current_imgui_win->Viewport) {
        current_window_hwnd = (HWND)current_imgui_win->Viewport->PlatformHandleRaw;
    }

    ImGui::Checkbox("Always on Top", &always_on_top_checkbox);  // 渲染复选框

    // 如果获取到窗口句柄，则应用置顶设置
    if (current_window_hwnd) {
        HWND insert_after = always_on_top_checkbox ? HWND_TOPMOST : HWND_NOTOPMOST;  // 根据复选框状态确定窗口层级
        bool should_be_topmost = always_on_top_checkbox;  // 是否应该置顶

        // 如果置顶状态发生变化，则更新窗口位置
        if (should_be_topmost != current_window_is_topmost) {
            ::SetWindowPos(current_window_hwnd, insert_after, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            current_window_is_topmost = should_be_topmost;
        }
    }
    ImGui::Separator();  // 添加分隔线
    ImGui::Spacing();    // 添加间距
}

// 检查注册的热键并调用相应的动作
void HackGUI::HandleHotkeys() {
    // 如果正在重新绑定热键，则不处理其他热键
    if (m_rebinding_hotkey_id != HotkeyID::NONE) {
        return;
    }

    // 遍历所有热键
    for (const auto& hotkey : m_hotkeys) {
        // 跳过未绑定或未分配动作的热键
        if (hotkey.currentKeyCode == 0 || !hotkey.action) {
            continue;
        }

        // 检查修饰键状态
        bool ctrlPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;  // Ctrl键是否按下
        bool altPressed = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;      // Alt键是否按下
        bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;   // Shift键是否按下
        
        // 检查修饰键是否匹配
        bool modifiersMatch = true;
        
        // 检查Ctrl键要求
        if (hotkey.modifierKeys & 1) { // 需要Ctrl键
            if (!ctrlPressed) modifiersMatch = false;
        } else { // 不需要Ctrl键
            if (ctrlPressed) modifiersMatch = false;
        }
        
        // 检查Alt键要求
        if (hotkey.modifierKeys & 2) { // 需要Alt键
            if (!altPressed) modifiersMatch = false;
        } else { // 不需要Alt键
            if (altPressed) modifiersMatch = false;
        }
        
        // 检查Shift键要求
        if (hotkey.modifierKeys & 4) { // 需要Shift键
            if (!shiftPressed) modifiersMatch = false;
        } else { // 不需要Shift键
            if (shiftPressed) modifiersMatch = false;
        }
        
        // 如果修饰键不匹配则跳过
        if (!modifiersMatch) continue;

        // 获取按键状态
        SHORT keyState = GetAsyncKeyState(hotkey.currentKeyCode);

        // 根据触发类型处理按键
        switch (hotkey.triggerType) {
            case HotkeyTriggerType::ON_PRESS:
                // 检查最低有效位（1表示自上次调用以来按下了按键）
                if (keyState & 1) {
                    hotkey.action(m_hack, true); // 传递按下状态
                }
                break;

            case HotkeyTriggerType::ON_HOLD:
                // 检查最高有效位（1表示当前按键被按下）
                bool isHeld = (keyState & 0x8000) != 0;
                hotkey.action(m_hack, isHeld); // 传递当前按住状态
                break;
        }
    }
}

// 处理用户正在重新绑定热键时的逻辑
void HackGUI::HandleHotkeyRebinding() {
    // 如果没有正在重新绑定的热键，则直接返回
    if (m_rebinding_hotkey_id == HotkeyID::NONE) return;

    int captured_vk = -1; // -1表示尚未捕获到按键
    int captured_modifiers = 0; // 0表示尚未捕获到修饰键

    // 首先检查特殊按键
    if (GetAsyncKeyState(VK_ESCAPE) & 1) {
        captured_vk = VK_ESCAPE; // 特殊值表示取消操作
    } else if ((GetAsyncKeyState(VK_DELETE) & 1) || (GetAsyncKeyState(VK_BACK) & 1)) {
        captured_vk = 0; // 0表示解绑
    } else {
        // 检查修饰键
        bool ctrlPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;  // Ctrl键是否按下
        bool altPressed = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;      // Alt键是否按下
        bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;   // Shift键是否按下
        
        // 捕获修饰键状态
        if (ctrlPressed) captured_modifiers |= 1;   // 记录Ctrl键
        if (altPressed) captured_modifiers |= 2;    // 记录Alt键
        if (shiftPressed) captured_modifiers |= 4;  // 记录Shift键

        // 遍历常见的键码以找到第一个按下的键
        for (int vk = VK_MBUTTON; vk < VK_OEM_CLEAR; ++vk) {
            // 跳过会干扰的键、不合适的键或已在上面处理的键
            if (vk == VK_ESCAPE || vk == VK_DELETE || vk == VK_BACK ||
                vk == VK_LBUTTON || vk == VK_RBUTTON || // 避免UI点击轻易绑定
                vk == VK_SHIFT || vk == VK_CONTROL || vk == VK_MENU || // 跳过修饰键
                vk == VK_CAPITAL || vk == VK_NUMLOCK || vk == VK_SCROLL) // 状态键不是好的选择
            {
                continue;
            }
            // 检查按键是否自上次调用以来被按下
            if (GetAsyncKeyState(vk) & 1) {
                captured_vk = vk;  // 记录捕获到的键码
                break; // 找到第一个按下的键后退出循环
            }
        }
    }

    // 如果捕获到按键或按下了操作键（Esc/Del/Back）
    if (captured_vk != -1) {
        if (captured_vk != VK_ESCAPE) { // 如果不是取消操作
            // 在向量中找到正在重新绑定的热键
            for (auto& hotkey : m_hotkeys) {
                if (hotkey.id == m_rebinding_hotkey_id) {
                    hotkey.currentKeyCode = captured_vk;      // 分配捕获到的键（0表示解绑）
                    hotkey.modifierKeys = captured_modifiers; // 分配捕获到的修饰键
                    // TODO: 在此处将更新后的热键保存到配置文件
                    break; // 找到并更新后退出循环
                }
            }
        }
        // 无论是否分配或取消，都要重置重新绑定状态
        m_rebinding_hotkey_id = HotkeyID::NONE;
    }
}

// 渲染"开关"部分，包含各种作弊功能的复选框
void HackGUI::RenderTogglesSection() {
    // 创建可折叠的标题栏，初始状态为展开
    if (ImGui::CollapsingHeader("Toggles", ImGuiTreeNodeFlags_DefaultOpen)) {
        bool tempState = false; // 用于ImGui交互的临时变量

        // 雾效开关
        tempState = m_hack.IsFogEnabled();
        if (ImGui::Checkbox("No Fog", &tempState)) { m_hack.toggleFog(tempState); }

        // 物体剪裁开关
        tempState = m_hack.IsObjectClippingEnabled();
        if (ImGui::Checkbox("Object Clipping", &tempState)) { m_hack.toggleObjectClipping(tempState); }

        // 全方向移动开关
        tempState = m_hack.IsFullStrafeEnabled();
        if (ImGui::Checkbox("Full Strafe", &tempState)) { m_hack.toggleFullStrafe(tempState); }

        // 冲刺开关控制用户偏好设置
        ImGui::Checkbox("Sprint", &m_sprintEnabled);

        // 怪物隐身开关
        tempState = m_hack.IsInvisibilityEnabled();
        if (ImGui::Checkbox("Invisibility (Mobs)", &tempState)) { m_hack.toggleInvisibility(tempState); }

        // 墙壁攀爬开关
        tempState = m_hack.IsWallClimbEnabled();
        if (ImGui::Checkbox("Wall Climb", &tempState)) { m_hack.toggleWallClimb(tempState); }

        // 穿墙开关
        tempState = m_hack.IsClippingEnabled();
        if (ImGui::Checkbox("Clipping", &tempState)) { m_hack.toggleClipping(tempState); }

        ImGui::Spacing();  // 添加间距
    }
}

// 渲染"动作"部分，包含各种操作按钮
void HackGUI::RenderActionsSection() {
    // 创建可折叠的标题栏，初始状态为展开
    if (ImGui::CollapsingHeader("Actions", ImGuiTreeNodeFlags_DefaultOpen)) {
        // 单位置保存/加载按钮（保持原有功能）
        float button_width = ImGui::GetContentRegionAvail().x * 0.48f; // 按钮宽度约为可用区域的一半
        if (ImGui::Button("Save Position", ImVec2(button_width, 0))) { m_hack.savePosition(); }  // 保存位置按钮
        ImGui::SameLine();  // 按钮同行显示
        if (ImGui::Button("Load Position", ImVec2(-1.0f, 0))) { m_hack.loadPosition(); } // 加载位置按钮，填充剩余空间
        
        // 多位置功能
        ImGui::Spacing();   // 添加间距
        ImGui::Separator(); // 添加分隔线
        ImGui::Text("Multi-Position Actions");  // 多位置操作标题
        
        // 位置槽位选择
        static int selected_slot = 0;  // 当前选中的槽位
        ImGui::Text("Slot:");         // 槽位标签
        ImGui::SameLine();             // 标签与下拉框同行显示
        ImGui::SetNextItemWidth(100.0f);  // 设置下拉框宽度
        // 槽位选择下拉框（0-9）
        ImGui::Combo("##PositionSlot", &selected_slot, "0\01\02\03\04\05\06\07\08\09\0");
        
        // 显示位置是否有效
        if (m_hack.isValidPosition(selected_slot)) {
            ImGui::SameLine();  // 与下拉框同行显示
            // 位置有效时显示绿色"Saved"文本
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Saved");
        } else {
            ImGui::SameLine();  // 与下拉框同行显示
            // 位置无效时显示黄色"Empty"文本
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Empty");
        }
        
        // 多位置保存/加载按钮
        if (ImGui::Button("Save to Slot", ImVec2(button_width, 0))) { m_hack.savePosition(selected_slot); }  // 保存到指定槽位按钮
        ImGui::SameLine();  // 按钮同行显示
        if (ImGui::Button("Load from Slot", ImVec2(-1.0f, 0))) { m_hack.loadPosition(selected_slot); }       // 从指定槽位加载按钮
        
        ImGui::Spacing();  // 添加间距
    }
}

// 渲染"热键"部分，用于配置热键设置
void HackGUI::RenderHotkeysSection() {
    // 创建可折叠的标题栏，初始状态为折叠
    if (ImGui::CollapsingHeader("Hotkeys")) {
        // 如果正在重新绑定热键，则显示提示信息
        if (m_rebinding_hotkey_id != HotkeyID::NONE) {
            const char* rebinding_name = "Unknown"; // 默认名称
            // 查找正在重新绑定的热键名称
            for(const auto& hk : m_hotkeys) {
                if (hk.id == m_rebinding_hotkey_id) {
                    rebinding_name = hk.name;
                    break;
                }
            }
            
            // 检查修饰键状态
            bool ctrlPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;  // Ctrl键是否按下
            bool altPressed = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;      // Alt键是否按下
            bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;   // Shift键是否按下
            
            // 构建修饰键字符串
            std::string modifierStr = "";
            if (ctrlPressed) modifierStr += "Ctrl+";   // 添加Ctrl+
            if (altPressed) modifierStr += "Alt+";     // 添加Alt+
            if (shiftPressed) modifierStr += "Shift+"; // 添加Shift+
            
            // 根据是否有修饰键显示不同的提示信息
            if (!modifierStr.empty()) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "正在重新绑定 '%s'。请按键（当前修饰键: %s）（ESC取消，DEL/BKSP清除）...", rebinding_name, modifierStr.c_str());
            } else {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "正在重新绑定 '%s'。请按键（ESC取消，DEL/BKSP清除）...", rebinding_name);
            }
            ImGui::Separator();  // 添加分隔线
        }

        // 在重新绑定过程中禁用其他控件
        bool disable_controls = (m_rebinding_hotkey_id != HotkeyID::NONE);
        if (disable_controls) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);  // 禁用控件
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);  // 降低透明度
        }

        // 遍历所有热键并渲染控件
        for (auto& hotkey : m_hotkeys) {
            RenderHotkeyControl(hotkey);  // 渲染单个热键控件
        }


        // 恢复控件状态
        if (disable_controls) {
            ImGui::PopItemFlag();    // 恢复控件
            ImGui::PopStyleVar();    // 恢复样式
        }
        ImGui::Separator();  // 添加分隔线
        ImGui::Spacing();    // 添加间距

        // 默认设置和解绑按钮
        if (ImGui::Button("应用推荐默认设置")) {  // 应用推荐默认设置按钮
            for (auto& hotkey : m_hotkeys) {
                hotkey.currentKeyCode = hotkey.defaultKeyCode;  // 恢复默认键位
            }
            // TODO: 将更新后的热键保存到配置文件
        }
        ImGui::SameLine();  // 按钮同行显示
        if (ImGui::Button("解绑所有")) {  // 解绑所有按钮
            for (auto& hotkey : m_hotkeys) {
                hotkey.currentKeyCode = 0; // 0 表示未绑定
            }
            // TODO: 将更新后的热键保存到配置文件
        }
        ImGui::Spacing();  // 添加间距
    }
}

// 渲染可折叠的日志部分
void HackGUI::RenderLogSection() {
    // 假设StatusUI仍用于日志记录，直到完成步骤1
    // 如果完成了步骤1（ILogger），这将从logger实例中获取日志
    if (ImGui::CollapsingHeader("Log", ImGuiTreeNodeFlags_None)) { // 初始状态为折叠
        // 创建可滚动的子窗口用于显示日志
        ImGui::BeginChild("LogScrollingRegion", ImVec2(0, 100), true, ImGuiWindowFlags_HorizontalScrollbar);
        {
            // 获取当前日志消息
            std::vector<std::string> current_messages = StatusUI::GetMessages(); // <-- 暂时继续使用StatusUI

            // 遍历并显示每条日志消息
            for (const auto& msg : current_messages) {
                ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Text); // 默认颜色
                // 根据消息类型设置不同颜色
                if (msg.rfind("ERROR:", 0) == 0) color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); // 错误-红色
                else if (msg.rfind("WARN:", 0) == 0) color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f); // 警告-黄色
                else if (msg.rfind("INFO:", 0) == 0) color = ImVec4(0.5f, 1.0f, 0.5f, 1.0f); // 信息-绿色
                ImGui::TextColored(color, "%s", msg.c_str());  // 以指定颜色显示消息
            }
            // 自动滚动到底部
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - ImGui::GetTextLineHeight() * 2) {
                ImGui::SetScrollHereY(1.0f);  // 滚动到底部
            }
        }
        ImGui::EndChild();  // 结束子窗口

        // 清除日志按钮
        if (ImGui::Button("Clear Log")) {
            StatusUI::ClearMessages(); // <-- 暂时继续使用StatusUI
        }
        ImGui::Spacing();  // 添加间距
    }
}

// 渲染可折叠的信息/关于部分
void HackGUI::RenderInfoSection() {
    // 创建可折叠的标题栏
    if (ImGui::CollapsingHeader("Info")) {
        ImGui::Text("KX Trainer by Krixx");  // 显示作者信息
        ImGui::Text("考虑使用付费版本，详情请访问 kxtools.xyz！");  // 推广付费版本
        ImGui::Separator();  // 添加分隔线

        // GitHub链接
        ImGui::Text("GitHub:");  // 标签
        ImGui::SameLine();    // 与按钮同行显示
        // GitHub仓库按钮，点击后打开链接
        if (ImGui::Button("访问仓库")) {
            ShellExecuteA(NULL, "open", "https://github.com/Krixx1337/KX-Trainer-Free", NULL, NULL, SW_SHOWNORMAL);
        }

        // kxtools.xyz链接
        ImGui::Text("官方网站:");  // 标签
        ImGui::SameLine();     // 与按钮同行显示
        // 网站按钮，点击后打开链接
        if (ImGui::Button("访问网站")) {
             ShellExecuteA(NULL, "open", "https://kxtools.xyz", NULL, NULL, SW_SHOWNORMAL);
        }

        // Discord链接
        ImGui::Text("Discord:");  // 标签
        ImGui::SameLine();     // 与按钮同行显示
        // Discord服务器按钮，点击后打开链接
        if (ImGui::Button("加入服务器")) {
             ShellExecuteA(NULL, "open", "https://discord.gg/z92rnB4kHm", NULL, NULL, SW_SHOWNORMAL);
        }
    }
}

// 主渲染函数，用于渲染HackGUI窗口
bool HackGUI::renderUI()
{
    static bool main_window_open = true;
    bool exit_requested = false;

    const float min_window_width = 400.0f;
    ImGui::SetNextWindowSizeConstraints(ImVec2(min_window_width, 0.0f), ImVec2(FLT_MAX, FLT_MAX));

    ImGuiWindowFlags window_flags = 0;
    ImGui::Begin("KX Trainer", &main_window_open, window_flags);

    if (!main_window_open) {
        exit_requested = true; // 如果用户关闭窗口，则请求退出
    }

    RenderAlwaysOnTop();

    m_hack.refreshAddresses(); // 在读取/写入之前确保指针有效
    HandleHotkeys();           // 处理已注册的热键
    HandleHotkeyRebinding();   // 如果正在重新绑定，则处理输入

    // 根据用户偏好设置应用连续状态
    m_hack.handleSprint(m_sprintEnabled);

    // 渲染UI各个部分
    RenderTogglesSection();
    RenderActionsSection();
    RenderHotkeysSection();
    RenderLogSection();
    RenderInfoSection();

    ImGui::End();

    return exit_requested;
}