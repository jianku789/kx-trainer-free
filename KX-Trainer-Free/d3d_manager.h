#ifndef D3D_MANAGER_H
#define D3D_MANAGER_H

#include <d3d11.h>
#include <windows.h>

namespace D3DManager {

    // 为给定窗口初始化D3D设备、上下文和交换链
    bool Initialize(HWND hWnd);

    // 清理所有D3D资源
    void Shutdown();

    // 处理交换链和渲染目标的窗口大小调整
    void HandleResize(UINT width, UINT height);

    // 核心D3D对象的访问器
    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();
    IDXGISwapChain* GetSwapChain();
    ID3D11RenderTargetView* GetMainRenderTargetView();

} // namespace D3DManager

#endif // D3D_MANAGER_H