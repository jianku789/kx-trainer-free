#pragma once

// 返回Windows虚拟键码的可读字符串表示
// VK码为0时返回"None"
// 未知码返回"VK 0xXX"
const char* GetKeyName(int vk_code);