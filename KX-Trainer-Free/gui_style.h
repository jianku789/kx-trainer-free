#ifndef GUI_STYLE_H
#define GUI_STYLE_H

namespace GUIStyle {

    // 将自定义视觉样式和颜色主题应用于ImGui
    void ApplyCustomStyle();

    // 加载主要应用程序字体
    bool LoadAppFont(float fontSize = 16.0f);

} // namespace GUIStyle

#endif