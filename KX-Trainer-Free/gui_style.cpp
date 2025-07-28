#define NOMINMAX

#include "gui_style.h"
#include "imgui/imgui.h"
#include <algorithm> // std::min/max所需

// 字体加载所需的头文件
#include <windows.h>
#include <string>
#include <ShlObj.h>       // SHGetFolderPath所需
#pragma comment(lib, "Shell32.lib") // 链接Shell32.lib

namespace GUIStyle {

    // 辅助函数，将RGB转换为ImVec4（alpha默认为1.0f）
    inline ImVec4 RgbToVec4(int r, int g, int b) {
        return ImVec4(static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, 1.0f);
    }

    // 获取系统字体目录路径
    std::string GetSystemFontsPath() {
        char fontsPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_FONTS, NULL, 0, fontsPath))) {
            return std::string(fontsPath);
        }
        return ""; // 失败时返回空字符串
    }

    // 加载主要应用程序字体（Bahnschrift）
    // 应在ImGui::CreateContext()之后和渲染器初始化之前调用
    // 如果自定义字体加载成功则返回true，否则返回false
    bool LoadAppFont(float fontSize) {
        ImGuiIO& io = ImGui::GetIO();
        bool success = false;

        // 首先添加默认字体作为后备
        io.Fonts->AddFontDefault();

        std::string fontsDir = GetSystemFontsPath();
        if (!fontsDir.empty()) {
            std::string fontPath = fontsDir + "\\bahnschrift.ttf"; // 使用Bahnschrift

            ImFont* customFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize);

            if (customFont) {
                // 将加载的字体设置为ImGui使用的默认字体
                io.FontDefault = customFont;
                success = true;
            }
            else {
                // 如果未找到Bahnschrift则记录或通知（并非所有Windows版本都有）
                MessageBoxA(NULL, ("无法从以下位置加载Bahnschrift字体: " + fontPath + ". 使用默认字体.").c_str(), "字体警告", MB_OK | MB_ICONWARNING);
                // 后备到默认字体已由AddFontDefault()处理
            }
        }
        else {
            MessageBoxA(NULL, "无法确定系统字体目录路径!", "字体错误", MB_OK | MB_ICONERROR);
            // 后备处理
        }
        return success;
    }

    void ApplyCustomStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // 定义调色板颜色
        const ImVec4 richBlack = RgbToVec4(17, 19, 37);   // #111325
        const ImVec4 oxfordBlue = RgbToVec4(26, 31, 52);   // #1a1f34
        const ImVec4 spaceCadet = RgbToVec4(37, 43, 69);   // #252b45
        const ImVec4 coolGray = RgbToVec4(128, 138, 184);// #808ab8
        const ImVec4 neonBlue = RgbToVec4(0, 98, 255);   // #0062ff
        const ImVec4 azure = RgbToVec4(51, 129, 255); // #3381ff
        const ImVec4 aliceBlue = RgbToVec4(229, 236, 244);// #e5ecf4

        // 计算派生颜色
        ImVec4 spaceCadetHover = ImVec4(
            std::min(spaceCadet.x * 1.3f, 1.0f),
            std::min(spaceCadet.y * 1.3f, 1.0f),
            std::min(spaceCadet.z * 1.3f, 1.0f),
            1.0f
        );
        ImVec4 spaceCadetActive = ImVec4(
            std::max(spaceCadet.x * 0.9f, 0.0f),
            std::max(spaceCadet.y * 0.9f, 0.0f),
            std::max(spaceCadet.z * 0.9f, 0.0f),
            1.0f
        );
        ImVec4 neonBlueActive = ImVec4(
            std::max(neonBlue.x * 0.9f, 0.0f),
            std::max(neonBlue.y * 0.9f, 0.0f),
            std::max(neonBlue.z * 0.9f, 0.0f),
            1.0f
        );


        // 布局和圆角
        style.WindowPadding = ImVec2(8.0f, 8.0f);
        style.FramePadding = ImVec2(5.0f, 4.0f);
        style.ItemSpacing = ImVec2(6.0f, 4.0f);
        style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
        style.ScrollbarSize = 14.0f;
        style.GrabMinSize = 12.0f;
        style.WindowRounding = 4.0f;
        style.ChildRounding = 2.0f;
        style.FrameRounding = 3.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabRounding = 3.0f;
        style.TabRounding = 4.0f;

        // 应用颜色（最小透明度）
        colors[ImGuiCol_Text] = aliceBlue;
        colors[ImGuiCol_TextDisabled] = coolGray;
        colors[ImGuiCol_WindowBg] = richBlack;
        colors[ImGuiCol_ChildBg] = oxfordBlue;
        colors[ImGuiCol_PopupBg] = richBlack;
        colors[ImGuiCol_Border] = spaceCadet;
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_FrameBg] = spaceCadet;
        colors[ImGuiCol_FrameBgHovered] = spaceCadetHover;
        colors[ImGuiCol_FrameBgActive] = spaceCadetActive;
        colors[ImGuiCol_TitleBg] = richBlack;
        colors[ImGuiCol_TitleBgActive] = oxfordBlue;
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(richBlack.x, richBlack.y, richBlack.z, 0.85f);
        colors[ImGuiCol_MenuBarBg] = oxfordBlue;
        colors[ImGuiCol_ScrollbarBg] = richBlack;
        colors[ImGuiCol_ScrollbarGrab] = coolGray;
        colors[ImGuiCol_ScrollbarGrabHovered] = aliceBlue;
        colors[ImGuiCol_ScrollbarGrabActive] = azure;
        colors[ImGuiCol_CheckMark] = neonBlue;
        colors[ImGuiCol_SliderGrab] = neonBlue;
        colors[ImGuiCol_SliderGrabActive] = azure;
        colors[ImGuiCol_Button] = neonBlue;
        colors[ImGuiCol_ButtonHovered] = azure;
        colors[ImGuiCol_ButtonActive] = neonBlueActive;
        colors[ImGuiCol_Header] = spaceCadet;
        colors[ImGuiCol_HeaderHovered] = spaceCadetHover;
        colors[ImGuiCol_HeaderActive] = spaceCadetHover;
        colors[ImGuiCol_Separator] = spaceCadet;
        colors[ImGuiCol_SeparatorHovered] = azure;
        colors[ImGuiCol_SeparatorActive] = neonBlue;
        colors[ImGuiCol_ResizeGrip] = ImVec4(coolGray.x, coolGray.y, coolGray.z, 0.5f); // 保持抓手微妙
        colors[ImGuiCol_ResizeGripHovered] = coolGray;
        colors[ImGuiCol_ResizeGripActive] = neonBlue;
        colors[ImGuiCol_Tab] = oxfordBlue;
        colors[ImGuiCol_TabHovered] = azure;
        colors[ImGuiCol_TabActive] = neonBlue;
        colors[ImGuiCol_TabUnfocused] = ImVec4(oxfordBlue.x, oxfordBlue.y, oxfordBlue.z, 0.8f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(neonBlue.x, neonBlue.y, neonBlue.z, 0.6f);
        colors[ImGuiCol_DockingPreview] = ImVec4(coolGray.x, coolGray.y, coolGray.z, 0.7f); // 预览使用灰色
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        colors[ImGuiCol_PlotLines] = coolGray;
        colors[ImGuiCol_PlotLinesHovered] = azure;
        colors[ImGuiCol_PlotHistogram] = neonBlue;
        colors[ImGuiCol_PlotHistogramHovered] = azure;
        colors[ImGuiCol_TableHeaderBg] = oxfordBlue;
        colors[ImGuiCol_TableBorderStrong] = spaceCadet;
        colors[ImGuiCol_TableBorderLight] = ImVec4(spaceCadet.x, spaceCadet.y, spaceCadet.z, 0.6f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // 透明
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(aliceBlue.x, aliceBlue.y, aliceBlue.z, 0.07f); // 微妙的交替背景
        colors[ImGuiCol_TextSelectedBg] = ImVec4(azure.x, azure.y, azure.z, 0.40f); // 保持选择半透明
        colors[ImGuiCol_DragDropTarget] = ImVec4(neonBlue.x, neonBlue.y, neonBlue.z, 0.95f);
        colors[ImGuiCol_NavHighlight] = azure;
        colors[ImGuiCol_NavWindowingHighlight] = aliceBlue;
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(coolGray.x, coolGray.y, coolGray.z, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(richBlack.x, richBlack.y, richBlack.z, 0.75f);


        // 如果启用，确保视口窗口不透明且无圆角
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.ChildRounding = 0.0f; // 也应用于带视口的子窗口
            style.PopupRounding = 0.0f; // 也应用于带视口的弹出窗口
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
    }

} // namespace GUIStyle