#include "d3d_manager.h"
#include <dxgi.h>
#include <cstdio> // 用于初始化失败时的错误日志记录

namespace D3DManager {

    // 内部状态
    static ID3D11Device* g_pd3dDevice = nullptr;
    static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
    static IDXGISwapChain* g_pSwapChain = nullptr;
    static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
    static HWND                     g_hWnd = nullptr;

    // 前向声明
    static void CreateRenderTargetInternal();
    static void CleanupRenderTargetInternal();

    bool Initialize(HWND hWnd) {
        if (!hWnd) return false;
        g_hWnd = hWnd;

        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2; // 使用2个缓冲区用于翻转模式

        RECT rc;
        ::GetClientRect(hWnd, &rc);
        // 确保宽度/高度至少为1
        sd.BufferDesc.Width = (rc.right - rc.left > 0) ? (rc.right - rc.left) : 1;
        sd.BufferDesc.Height = (rc.bottom - rc.top > 0) ? (rc.bottom - rc.top) : 1;

        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 推荐的现代交换效果
        sd.Flags = 0;

        UINT createDeviceFlags = 0;
#ifdef _DEBUG
        //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG; // 如果安装并需要D3D调试层则启用
#endif

        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };

        HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            createDeviceFlags, featureLevelArray, ARRAYSIZE(featureLevelArray),
            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice,
            &featureLevel, &g_pd3dDeviceContext);

        // 如果硬件失败则回退到WARP驱动
        if (FAILED(res)) {
            fprintf(stderr, "D3D11CreateDeviceAndSwapChain (硬件) 失败: 0x%lx\n", res);
            res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
                featureLevelArray, ARRAYSIZE(featureLevelArray), D3D11_SDK_VERSION,
                &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
            if (FAILED(res)) {
                fprintf(stderr, "D3D11CreateDeviceAndSwapChain (WARP) 失败: 0x%lx\n", res);
                Shutdown();
                return false;
            }
            fprintf(stderr, "使用WARP (软件) D3D11驱动.\n");
        }

        CreateRenderTargetInternal();
        return true;
    }

    void Shutdown() {
        CleanupRenderTargetInternal();
        if (g_pSwapChain) {
            g_pSwapChain->SetFullscreenState(FALSE, NULL); // 在释放前确保窗口模式
            g_pSwapChain->Release();
            g_pSwapChain = nullptr;
        }
        if (g_pd3dDeviceContext) {
            g_pd3dDeviceContext->ClearState();
            g_pd3dDeviceContext->Flush();
            g_pd3dDeviceContext->Release();
            g_pd3dDeviceContext = nullptr;
        }
        if (g_pd3dDevice) {
            g_pd3dDevice->Release();
            g_pd3dDevice = nullptr;
        }
        g_hWnd = nullptr;
    }

    void HandleResize(UINT width, UINT height) {
        if (!g_pSwapChain || width == 0 || height == 0) {
            return; // 如果无效则无法调整大小
        }

        CleanupRenderTargetInternal(); // 释放现有的RTV

        HRESULT hr = g_pSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        if (FAILED(hr)) {
            fprintf(stderr, "调整交换链缓冲区大小时出错: 0x%lx\n", hr);
            // 如果需要，考虑更强大的错误处理/重新初始化
            return;
        }

        CreateRenderTargetInternal(); // Re-create the render target view
    }

    static void CreateRenderTargetInternal() {
        if (!g_pSwapChain || !g_pd3dDevice) return;

        ID3D11Texture2D* pBackBuffer = nullptr;
        HRESULT hr = g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (SUCCEEDED(hr)) {
            hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
            pBackBuffer->Release(); // RTV持有自己的引用
            if (FAILED(hr)) {
                fprintf(stderr, "创建渲染目标视图时出错: 0x%lx\n", hr);
                g_mainRenderTargetView = nullptr;
            }
        }
        else {
            fprintf(stderr, "获取交换链缓冲区时出错: 0x%lx\n", hr);
        }
    }

    static void CleanupRenderTargetInternal() {
        if (g_mainRenderTargetView) {
            g_mainRenderTargetView->Release();
            g_mainRenderTargetView = nullptr;
        }
        // 确保上下文不再绑定RTV
        if (g_pd3dDeviceContext) {
            ID3D11RenderTargetView* nullRTV = nullptr;
            g_pd3dDeviceContext->OMSetRenderTargets(1, &nullRTV, nullptr);
            g_pd3dDeviceContext->Flush(); // 确保命令执行
        }
    }

    // --- 获取器 ---
    ID3D11Device* GetDevice() { return g_pd3dDevice; }
    ID3D11DeviceContext* GetDeviceContext() { return g_pd3dDeviceContext; }
    IDXGISwapChain* GetSwapChain() { return g_pSwapChain; }
    ID3D11RenderTargetView* GetMainRenderTargetView() { return g_mainRenderTargetView; }

} // namespace D3DManager