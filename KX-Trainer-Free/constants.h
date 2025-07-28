#pragma once

#include <windows.h>

namespace Constants {
    // 应用程序信息
    constexpr int APP_VERSION = 20100;
    constexpr char APP_NAME[] = "KX Trainer";
    constexpr char API_URL[] = "https://kxtools.xyz/api/v1/version/kx-trainer-free";

    // 进程
    namespace Process {
        constexpr wchar_t* GW2_PROCESS_NAME_W = L"Gw2-64.exe";
        constexpr const char* GW2_PROCESS_NAME_A = "Gw2-64.exe";
    }

    // 扫描配置
    namespace Scan {
        // x64中有效用户空间指针的最小预期值，用作完整性检查
        constexpr uintptr_t BASE_ADDRESS_MIN_VALUE = 0x10000;

        // 相对于模式匹配地址的偏移量，用于找到基指针的位置
        constexpr uintptr_t POINTER_LOCATION_OFFSET = 0x8;

        constexpr int MAX_BASE_SCAN_ATTEMPTS = 15;
        constexpr int BASE_SCAN_RETRY_DELAY_MS = 1000;
    }

    // 模式和掩码
    namespace Patterns {
        static inline char BASE_SCAN_PATTERN[] = "\x01\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x01";
        static inline char BASE_SCAN_MASK[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
        static inline char FOG_PATTERN[] = "\xF3\x0F\x11\x00\x24\x00\xF3\x0F\x11\x5C\x24\x00\xF3\x0F\x10\x9C\x24";
        static inline char FOG_MASK[] = "xxx?x?xxxxx?xxxxx";
        static inline char OBJECT_CLIPPING_PATTERN[] = "\xD3\x0F\x29\x54\x24\x60\x0F\x28\xCA";
        static inline char OBJECT_CLIPPING_MASK[] = "?xxxxxxxx";
        static inline char FULL_STRAFE_PATTERN[] = "\x75\x00\x00\x28\x4C\x24\x00\x0F\x59\x8B";
        static inline char FULL_STRAFE_MASK[] = "x??xxx?xxx";
    }

    // 偏移量
    namespace Offsets {
        constexpr unsigned int BYTE1 = 0x50;
        constexpr unsigned int BYTE2 = 0x88;
        constexpr unsigned int BYTE3 = 0x10;
        constexpr unsigned int BYTE4 = 0x68;
    }

    // 热键
    namespace Hotkeys {
        // 推荐的默认绑定
        constexpr int KEY_SAVEPOS = VK_F5;          // F5
        constexpr int KEY_LOADPOS = VK_F6;          // F6
        constexpr int KEY_INVISIBILITY = VK_F7;     // F7
        constexpr int KEY_WALLCLIMB = VK_F8;        // F8
        constexpr int KEY_CLIPPING = VK_F9;         // F9
        constexpr int KEY_OBJECT_CLIPPING = VK_F10; // F10
        constexpr int KEY_FULL_STRAFE = VK_INSERT;  // Insert
        constexpr int KEY_NO_FOG = VK_HOME;         // Home
        constexpr int KEY_SUPER_SPRINT = VK_RSHIFT; // Right Shift
        constexpr int KEY_SPRINT = VK_LSHIFT;       // Left Shift
        constexpr int KEY_FLY = VK_LCONTROL;        // Left Ctrl
        
        // 多位置加载热键
        constexpr int KEY_LOAD_POS_SLOT_0 = VK_F1;  // F1
        constexpr int KEY_LOAD_POS_SLOT_1 = VK_F2;  // F2
        constexpr int KEY_LOAD_POS_SLOT_2 = VK_F3;  // F3
        constexpr int KEY_LOAD_POS_SLOT_3 = VK_F4;  // F4
        constexpr int KEY_LOAD_POS_SLOT_4 = VK_F11; // F11
    }

    // 设置
    namespace Settings {
        constexpr float SPRINT_SPEED = 12.22f;                     // 冲刺速度
        constexpr float NORMAL_SPEED = 9.1875f;                    // 正常速度
        constexpr float SUPER_SPRINT_SPEED = 30.0f;                // 超级冲刺速度
        constexpr float FLY_SPEED = 20.0f;                         // 飞行速度
        constexpr float FLY_NORMAL_SPEED = -40.625f;               // 飞行下降速度
        constexpr float WALLCLIMB_SPEED = 20.0f;                   // 墙壁攀爬速度
        constexpr float WALLCLIMB_NORMAL_SPEED = 2.1875f;          // 墙壁攀爬正常速度
        constexpr byte OBJECT_CLIPPING_ON = 0xDB;                  // 对象碰撞检测开启
        constexpr byte OBJECT_CLIPPING_OFF = 0xD3;                 // 对象碰撞检测关闭
        constexpr float INVISIBILITY_ON = 2.7f;                    // 隐身开启值
        constexpr float INVISIBILITY_OFF = 1.0f;                   // 隐身关闭值
        constexpr float CLIPPING_ON = 99999.0f;                    // 碰撞检测开启值
        constexpr float CLIPPING_OFF = 0.0f;                       // 碰撞检测关闭值
        constexpr byte FULL_STRAFE_ON = 0x75;                      // 全方向扫射开启
        constexpr byte FULL_STRAFE_OFF = 0x0F;                     // 全方向扫射关闭
        constexpr byte NO_FOG_ON = 0x54;                           // 无雾效果开启
        constexpr byte NO_FOG_OFF = 0x5C;                          // 无雾效果关闭
    }
}