// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "ChiliWin.h"
#include "pch.h"
#include <sstream>
#include <vector>
#include "reshade.hpp"
#include "config.hpp"
#include "App.h"
#include "DX11Graphics.h"
#include <d3d11.h>
#include <d3dcompiler.h>


#define MAX_BACKBUF_COUNT   3
//Render target format, DX and reshade have its own definations
#define  RTVFormat DXGI_FORMAT_R8G8B8A8_UNORM


using namespace reshade;

using namespace reshade::api;

using namespace Microsoft::WRL;

#pragma comment  (lib,"d3d11.lib")
#pragma comment (lib,"D3DCompiler.lib")
#pragma comment(lib, "dxgi.lib")



extern bool save_texture_image(const resource_desc &desc, const subresource_data &data);





static reshade::api::device *re_device = nullptr;
App *app;
Window *window;
HWND g_hWnd = nullptr;
HINSTANCE g_hInstance;
LPCWSTR WinClassName = L"MyWindowClass";



LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_DESTROY:
        DestroyWindow(hWnd);
        return 0;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
};


HWND CreateWindowInDll()
{
    WNDCLASSEXW winClass = {};
    winClass.cbSize = sizeof(WNDCLASSEXW);
    winClass.style = CS_GLOBALCLASS;
    winClass.lpfnWndProc = &HandleMsg;
    winClass.hInstance = g_hInstance;
    winClass.lpszClassName = L"MyWindowClass";


    if (!RegisterClassExW(&winClass)) {
        MessageBoxA(0, "RegisterClassEx failed", "Fatal Error", MB_OK);

    }



    return g_hWnd;
}

struct __declspec(uuid("2FA5FB3D-7873-4E67-9DDA-5D449DB2CB47")) frame_capture
{

    // Create multiple host resources, to buffer copies from device to host over multiple frames
    reshade::api::resource host_resources[1];
    uint64_t copy_finished_fence_value = 1;
    uint64_t copy_initiated_fence_value = 1;
    reshade::api::fence copy_finished_fence = {};


    std::vector<reshade::api::resource_view> rtvs;
    reshade::api::resource_view dsv;

    reshade::api::resource green_screen; // 用于存储渲染目标的资源
    reshade::api::resource_view green_screen_rtv;



    reshade::api::resource_view RTV_SRV[MAX_BACKBUF_COUNT] = { 0 };

    reshade::api::resource_view RTV[MAX_BACKBUF_COUNT] = { 0 };

    reshade::api::swapchain *pOurswapchain = { 0 };

    DX11Graphics dx11Graphics;

    bool Init = false;

    bool gameSRVCreated = false;
};

DX11Graphics dx11Graphics;

bool InitializeSwapChain(HWND hwnd, ID3D11Device *d3d11Device, int width, int height, ComPtr<IDXGISwapChain1> &swapChain) {
    // 获取 DXGI 工厂
    HRESULT hr;

    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain1;
    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;

    Microsoft::WRL::ComPtr<IDXGIFactory1> pFactory;
    hr = CreateDXGIFactory1(IID_PPV_ARGS(pFactory.GetAddressOf()));
    if (FAILED(hr)) {
        throw std::runtime_error("IDXGIFactoryクラスの作成に失敗しました。");
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
    swapChainDesc.OutputWindow = g_hWnd;
    swapChainDesc.BufferCount = 3;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //DXGI_SWAP_EFFECT_DISCARD DXGI_SWAP_EFFECT_SEQUENTIAL

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//DXGI_USAGE_BACK_BUFFER DXGI_USAGE_RENDER_TARGET_OUTPUT
    swapChainDesc.Flags = 0;
    //swapChainDesc.Flags = 0;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    //フルスクリーンとウィンドモードの切り替えがしたい場合は、まずウィンドウモードとして生成することを推奨しているみたい
    swapChainDesc.Windowed = true;
    swapChainDesc.BufferDesc.Width = 0;
    swapChainDesc.BufferDesc.Height = 0;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;


    UINT swapCreateFlags = 0u;
#ifndef NDEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        swapCreateFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &pSwapChain,
        &pDevice,
        nullptr,
        &pContext
    );


    /*hr = pFactory->CreateSwapChain(d3d11Device, &swapChainDesc, &pSwapChain);
    if (FAILED(hr)) {
        throw std::runtime_error("IDXGISwapChainの作成に失敗");
    }*/

    if (pSwapChain) {
        reshade::log::message(reshade::log::level::info, ("host_resource_index: 1"));
    }
    else {
        reshade::log::message(reshade::log::level::info, ("host_resource_index: 2"));
    }

    return true;
    //hr = pFactory->CreateSwapChain(d3d11Device, &swapChainDesc,swapChmpSwapChain.GetAddressOf());
    //if (FAILED(hr)) {
    //  throw std::runtime_error("IDXGISwapChainの作成に失敗");
    //}

 //   ComPtr<IDXGIDevice> dxgiDevice;
 //    hr = d3d11Device->QueryInterface(__uuidof(IDXGIDevice), &dxgiDevice);
 //   if (FAILED(hr)) {
 //       MessageBox(hwnd, "Failed to get DXGI device!", "Error", MB_OK);
 //       return false;
 //   }

 //   ComPtr<IDXGIAdapter> dxgiAdapter;
 //   hr = dxgiDevice->GetAdapter(&dxgiAdapter);
 //   if (FAILED(hr)) {
 //       MessageBox(hwnd, "Failed to get DXGI adapter!", "Error", MB_OK);
 //       return false;
 //   }

 //   ComPtr<IDXGIFactory2> dxgiFactory;
 //   hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);
 //   if (FAILED(hr)) {
 //       MessageBox(hwnd, "Failed to get DXGI factory!", "Error", MB_OK);
 //       return false;
 //   }

 //   // 配置 SwapChain 描述
 //   DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
 //   swapChainDesc.Width = width;
 //   swapChainDesc.Height = height;
 //   swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
 //   swapChainDesc.Stereo = FALSE;
 //   swapChainDesc.SampleDesc.Count = 1;
 //   swapChainDesc.SampleDesc.Quality = 0;
 //   swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
 //   swapChainDesc.BufferCount = 2;
 //   //swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
 //   swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
 //  // swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

 //   if (!IsWindow(g_hWnd) || !d3d11Device) {
 //       MessageBox(nullptr, "Invalid HWND!", "Error", MB_OK);

 //   }
 //   HWND a = nullptr;
 //   // 创建 SwapChain
 //   hr = dxgiFactory->CreateSwapChainForHwnd(
 //       d3d11Device,
 //       g_hWnd,
 //       &swapChainDesc,
 //       nullptr, // 可选的全屏参数
 //       nullptr, // 输出限制参数
 //       &swapChain
 //   );

 //   if (FAILED(hr)) {
 //       MessageBox(hwnd, "Failed to create SwapChain!", "Error", MB_OK);
 //       return false;
 //   }

    return true;
}


//static reshade::api::resource host_resource = { 0 };

static bool Firston_INIT = false;
static void on_init(reshade::api::device *device)
{
    re_device = device;
    while (g_hWnd == NULL || !IsWindow(g_hWnd))
    {
        Sleep(10);
    }
    device_api api = device->get_api();
    switch (api)
    {
    case reshade::api::device_api::d3d9:
        reshade::log::message(reshade::log::level::info, ("device - API: 1"));
        break;
    case reshade::api::device_api::d3d10:
        reshade::log::message(reshade::log::level::info, ("device - API: 2"));
        break;
    case reshade::api::device_api::d3d11:
        reshade::log::message(reshade::log::level::info, ("device - API: 3"));
        break;
    case reshade::api::device_api::d3d12:
        reshade::log::message(reshade::log::level::info, ("device - API: 4"));
        break;
    case reshade::api::device_api::opengl:
        reshade::log::message(reshade::log::level::info, ("device - API: 5"));
        break;
    case reshade::api::device_api::vulkan:
        reshade::log::message(reshade::log::level::info, ("device - API: 6"));
        break;
    default:
        break;
    }

    ID3D11Device *d3d_device = reinterpret_cast<ID3D11Device *>(device->get_native());
    IUnknown *d3d_device1 = reinterpret_cast<IUnknown *>(device->get_native());
    // 获取 DXGI 工厂
    IDXGIFactory *factory = nullptr;
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void **>(&factory));
    if (FAILED(hr)) {
        // 处理错误
        return;
    }

    // 获取 DXGI 适配器
    IDXGIAdapter *adapter = nullptr;
    factory->EnumAdapters(0, &adapter);

    // 获取设备和交换链的描述信息
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 2;  // 双缓冲
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // 常用的纹理格式
    swapChainDesc.BufferDesc.Width = 1024;  // 设置你窗口的宽度
    swapChainDesc.BufferDesc.Height = 768;  // 设置你窗口的高度
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // 渲染目标
    swapChainDesc.OutputWindow = g_hWnd;  // 设置窗口句柄
    swapChainDesc.SampleDesc.Count = 1;  // 多重采样
    swapChainDesc.Windowed = TRUE;  // 窗口模式

    // 创建交换链
    IDXGISwapChain *swapChain111 = nullptr;

    hr = factory->CreateSwapChain((IUnknown *)device->get_native(), &swapChainDesc, &swapChain111);
    if (FAILED(hr)) {
        // 处理错误
        return;
    }

    // 后续可以继续使用 `swapChain` 来进行渲染操作

    // 释放工厂和适配器对象
    if (adapter) adapter->Release();
    if (factory) factory->Release();


}

static void on_initcommand_queue(reshade::api::command_queue *queue)
{
    while (g_hWnd == NULL || !IsWindow(g_hWnd))
    {
        Sleep(10);
    }

    queue->get_device();
    /* ID3D11Device *d3d_device = reinterpret_cast<ID3D11Device *>(device->get_native());
     IUnknown *d3d_device1 = reinterpret_cast<IUnknown *>(device->get_native());*/
     // 获取 DXGI 工厂
    IDXGIFactory *factory = nullptr;
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void **>(&factory));
    if (FAILED(hr)) {
        // 处理错误
        return;
    }

    // 获取 DXGI 适配器
    IDXGIAdapter *adapter = nullptr;
    factory->EnumAdapters(0, &adapter);

    // 获取设备和交换链的描述信息
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 2;  // 双缓冲
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // 常用的纹理格式
    swapChainDesc.BufferDesc.Width = 1024;  // 设置你窗口的宽度
    swapChainDesc.BufferDesc.Height = 768;  // 设置你窗口的高度
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // 渲染目标
    swapChainDesc.OutputWindow = g_hWnd;  // 设置窗口句柄
    swapChainDesc.SampleDesc.Count = 1;  // 多重采样
    swapChainDesc.Windowed = TRUE;  // 窗口模式

    // 创建交换链
    IDXGISwapChain *swapChain111 = nullptr;

    hr = factory->CreateSwapChain((IUnknown *)queue, &swapChainDesc, &swapChain111);
    if (FAILED(hr)) {
        // 处理错误
        return;
    }

    // 后续可以继续使用 `swapChain` 来进行渲染操作

    // 释放工厂和适配器对象
    if (adapter) adapter->Release();
    if (factory) factory->Release();
}

static void on_present(command_queue *queue, swapchain *swapchain, const rect *, const rect *, uint32_t, const rect *)
{

    uint32_t BackBufferCount = swapchain->get_back_buffer_count();
    reshade::log::message(reshade::log::level::info, ("BackBufferCount: " + std::to_string(BackBufferCount)).c_str());

    /* frame_capture &data = swapchain->get_device()->get_private_data<frame_capture>();
     ID3D11Device *d3d11_device = ((ID3D11Device *)re_device->get_native());
     if (d3d11_device && g_hWnd && data.Init == false)
     {
         data.dx11Graphics.Init_Resource(d3d11_device, g_hWnd, g_hInstance);
         data.Init = true;
     }*/


    while (g_hWnd == NULL || !IsWindow(g_hWnd))
    {
        Sleep(10);
    }
    //ID3D11Device *d3d11_device = (ID3D11Device *)swapchain->get_device();
    if (Firston_INIT == false)
    {
        reshade::api::device *device = swapchain->get_device();
        frame_capture &data = device->create_private_data<frame_capture>();

        ComPtr<IDXGISwapChain1> swapChain;
        Firston_INIT = data.dx11Graphics.CreateDeviceAndSwapChain(g_hWnd);
        //Firston_INIT = InitializeSwapChain(g_hWnd, d3d11_device, 800, 600, swapChain);
    }
    else
    {

        reshade::api::device *dev = nullptr;
        dev = swapchain->get_device();
        frame_capture &devData = dev->get_private_data <frame_capture>();
        reshade::api::resource_desc desc = dev->get_resource_desc(swapchain->get_current_back_buffer());


        ID3D11Resource *pbackbuf;
        reshade::api::resource d3dres = swapchain->get_current_back_buffer();
        pbackbuf = (ID3D11Resource *)(d3dres.handle);

        D3D11_TEXTURE2D_DESC backBufferDesc;
        ComPtr<ID3D11Texture2D> backBufferTexture;
        HRESULT hr = pbackbuf->QueryInterface(IID_PPV_ARGS(&backBufferTexture));
        if (FAILED(hr))
        {
            return;
        }

        backBufferTexture->GetDesc(&backBufferDesc);

        devData.dx11Graphics.CreateSRV_forGameRTV(backBufferDesc, backBufferTexture, (ID3D11Device *)dev->get_native());
        devData.gameSRVCreated = true;



        reshade::api::command_list *cmd_list = queue->get_immediate_command_list();
        const reshade::api::resource back_buffer = swapchain->get_current_back_buffer();

        cmd_list->barrier(back_buffer, reshade::api::resource_usage::present, reshade::api::resource_usage::shader_resource_pixel | reshade::api::resource_usage::shader_resource);

        queue->flush_immediate_command_list();

        //queue->wait_idle();

        devData.dx11Graphics.On_Present();

        cmd_list->barrier(back_buffer, reshade::api::resource_usage::shader_resource_pixel | reshade::api::resource_usage::shader_resource, reshade::api::resource_usage::present);

    }
}

static void on_reshade_finish_effects(reshade::api::effect_runtime *runtime, reshade::api::command_list *, reshade::api::resource_view rtv, reshade::api::resource_view)
{


    //runtime->capture_screenshot()
    frame_capture &data = runtime->get_private_data<frame_capture>();
    reshade::api::device *const device = runtime->get_device();
    reshade::api::command_queue *const queue = runtime->get_command_queue();

    const reshade::api::resource rtv_resource = device->get_resource_from_view(rtv);

    reshade::api::resource_desc desc = device->get_resource_desc(rtv_resource);
    reshade::log::message(reshade::log::level::info, ("resource_view_width: " + std::to_string(desc.texture.width)).c_str());
    reshade::log::message(reshade::log::level::info, ("resource_view_width: " + std::to_string(desc.texture.height)).c_str());

    desc.type = reshade::api::resource_type::texture_2d;
    reshade::log::message(reshade::log::level::info, ("RTV Resource - Format: " + std::to_string(static_cast<int>(desc.texture.format))).c_str());
    //desc.texture.format = format_to_default_typed(desc.texture.format, 0); // return 10 r16g16b16a16_float
 // desc.heap = reshade::api::memory_heap::gpu_to_cpu;
    desc.usage = reshade::api::resource_usage::copy_dest | reshade::api::resource_usage::shader_resource;
    desc.flags = reshade::api::resource_flags::none;
    //new add


    uint32_t originWidth = device->get_resource_desc(rtv_resource).texture.width;
    desc.texture.width = desc.texture.width * 2;




    for (size_t i = 0; i < std::size(data.host_resources); ++i)
    {
        if (!device->create_resource(desc, nullptr, reshade::api::resource_usage::copy_dest, &data.host_resources[i]))
        {
            reshade::log::message(reshade::log::level::error, "Failed to create host resource!");

            for (size_t k = 0; k < i; ++k)
            {
                device->destroy_resource(data.host_resources[k]);
                data.host_resources[k] = { 0 };
            }

            return;
        }
    }

    subresource_box left_box = {
    0,                  // 左边界
    0,                  // 上边界
    0,                  // 前边界
    originWidth,    // 右边界
    desc.texture.height,   // 下边界
    1                   // 后边界
    };


    // 拷贝 backBuffer 到目标纹理的右半部分
    subresource_box right_box = {
        originWidth,    // 左边界
        0,                  // 上边界
        0,                  // 前边界
        desc.texture.width,// 右边界
        desc.texture.height,   // 下边界
        1                   // 后边界
    };

    reshade::api::command_list *const cmd_list = queue->get_immediate_command_list();
    cmd_list->barrier(rtv_resource, reshade::api::resource_usage::render_target, reshade::api::resource_usage::copy_source);
    size_t host_resource_index = data.copy_initiated_fence_value % std::size(data.host_resources);
    reshade::log::message(reshade::log::level::info, ("host_resource_index: " + std::to_string(host_resource_index)).c_str());
    //cmd_list->copy_texture_region(rtv_resource, 0, nullptr, data.host_resources[host_resource_index], 0, nullptr);
    cmd_list->copy_texture_region(rtv_resource, 0, nullptr, data.host_resources[host_resource_index], 0, &left_box);
    cmd_list->copy_texture_region(rtv_resource, 0, nullptr, data.host_resources[host_resource_index], 0, &right_box);

    cmd_list->barrier(rtv_resource, reshade::api::resource_usage::copy_source, reshade::api::resource_usage::render_target);

    queue->flush_immediate_command_list();
    // Signal the fence once the copy has finished
    queue->signal(data.copy_finished_fence, data.copy_initiated_fence_value++);

    // Check if a previous copy has already finished (by waiting on the corresponding fence value with a timeout of zero)
    if (!device->wait(data.copy_finished_fence, data.copy_finished_fence_value, 0))
    {
        // If all copies are still underway, check if all available space to buffer another frame is already used (if yes, have to wait for the oldest copy to finish, if no, can return and handle another frame)
        if (data.copy_initiated_fence_value - data.copy_finished_fence_value >= std::size(data.host_resources))
        {
            device->wait(data.copy_finished_fence, data.copy_finished_fence_value, UINT64_MAX);
        }
        else
        {
            return;
        }
    }

    // Map the oldest finished copy for reading
    host_resource_index = data.copy_finished_fence_value % std::size(data.host_resources);
    data.copy_finished_fence_value++;


    /* subresource_data mapped_data = {};
     if (device->map_texture_region(data.host_resources[0], 0, nullptr, map_access::read_only, &mapped_data))
     {
         reshade::log::message(reshade::log::level::warning, "Start to save_texture_image!");
         save_texture_image(desc, mapped_data);

         device->unmap_texture_region(data.host_resources[0], 0);
     }*/



    if (app && app->GetWindow())
    {
        if (data.host_resources[0].handle)
        {

            app->CopyRTVResource((ID3D11Device *)device->get_native(), (ID3D11Resource *)data.host_resources[0].handle, desc);
            reshade::log::message(reshade::log::level::info, "app window alive");
        }

    }

    reshade::log::message(reshade::log::level::info, "Success to create host resource!");
    for (reshade::api::resource &host_resource : data.host_resources)
    {
        device->destroy_resource(host_resource);
        reshade::log::message(reshade::log::level::info, "destroy_resource host resource!");
    }


}

static void on_bind_render_targets_and_depth_stencil(reshade::api::command_list *cmd_list, uint32_t count, const reshade::api::resource_view *rtvs, reshade::api::resource_view dsv)
{
    auto &current_state = cmd_list->get_private_data<frame_capture>();
    current_state.rtvs.assign(rtvs, rtvs + count);
    current_state.dsv = dsv;

}

static bool on_draw(reshade::api::command_list *cmd_list, uint32_t vertices, uint32_t instances, uint32_t first_vertex, uint32_t first_instance)
{
    //cmd_list->bind_render_targets_and_depth_stencil(1, &green_screen_rtv);
    cmd_list->draw(vertices, instances, first_vertex, first_instance); // Duplicate draw call, but rendering to custom render target

    // Retrieve render target state and restore it again
    /*const auto &current_state = cmd_list->get_private_data<rtv_state>();
    cmd_list->bind_render_targets_and_depth_stencil(current_state.rtvs.size(), current_state.rtvs.data(), current_state.dsv);*/
    return false; // Return false so that original draw call is executed again afterwards, now that original render targets were restored
}

static void on_destroy(reshade::api::effect_runtime *runtime)
{
    // user_data &data = runtime->get_private_data<user_data>();

    /* if (data.host_resource != 0)
     {
         runtime->get_device()->destroy_resource(data.host_resource);
     }

     runtime->destroy_private_data<user_data>();*/

     /*if (host_resource != 0)
     {
         runtime->get_device()->destroy_resource(host_resource);

         reshade::log::message(reshade::log::level::info, "Succeed to Destroy host resource");
     }*/

    frame_capture &data = runtime->get_private_data<frame_capture>();

    reshade::api::device *const device = runtime->get_device();

    for (reshade::api::resource &host_resource : data.host_resources)
    {
        device->destroy_resource(host_resource);
        reshade::log::message(reshade::log::level::info, "destroy_resource host resource!");
    }


    device->destroy_fence(data.copy_finished_fence);

    runtime->destroy_private_data<frame_capture>();
}


static void on_init_device(reshade::api::device *device)
{
    /*if (app)
    {
        app->Go();
    }*/
}




DWORD WINAPI WindowThreadProc(LPVOID lpParam)
{
    CreateWindowInDll();
    //无消息循环时窗口会退出

    RECT initialRect = { 0, 0, 1024, 768 };
    AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
    LONG initialWidth = initialRect.right - initialRect.left;
    LONG initialHeight = initialRect.bottom - initialRect.top;

    g_hWnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
                            WinClassName,
                            L"hahah1",
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            initialWidth,
                            initialHeight,
                            0, 0, g_hInstance, 0);

    if (!g_hWnd) {
        MessageBoxA(0, "CreateWindowEx failed", "Fatal Error", MB_OK);

    }
    // show window
    if (g_hWnd == nullptr)
    {
        MessageBoxA(0, "g_hWnd failed", "Fatal Error", MB_OK);
        reshade::log::message(reshade::log::level::error, "Create Chili Window Failed");
        // Init ImGui Win32 Impl
    }
    reshade::log::message(reshade::log::level::error, "Create Chili Window Successed");
    ShowWindow(g_hWnd, SW_SHOWNORMAL);//SW_SHOWDEFAULT

    UpdateWindow(g_hWnd);

    MSG msg;
    while (GetMessage(&msg, g_hWnd, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}



extern "C" __declspec(dllexport) const char *NAME = "AAA Game Expander";
extern "C" __declspec(dllexport) const char *DESCRIPTION = "AAA Expander Game Graphics to 3840.";

bool attached = false;

BOOL APIENTRY DllMain(HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
)
{


    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        g_hInstance = hModule;
        /*app = new App();
        app->SetDllInstance(hModule); */
        //CreateThread(NULL, 0,WindowThreadProc, NULL, 0, NULL);
        CreateThread(NULL, 0, WindowThreadProc, NULL, 0, NULL);

        // 注册插件
        if (!reshade::register_addon(hModule))
        {
            std::stringstream s;
            s << "Failed to register AAA Expander addon!";
            reshade::log::message(reshade::log::level::error, s.str().c_str());
            return FALSE;
        }
        reshade::log::message(reshade::log::level::info, "Succeed to register AAA Expander addon!");


        // reshade::register_event<reshade::addon_event::init_device>(on_init);
         //init_command_queue
        reshade::register_event<reshade::addon_event::init_command_queue>(on_initcommand_queue);
        //reshade::register_event<reshade::addon_event::present>(&on_present);




        //reshade::register_event<reshade::addon_event::destroy_effect_runtime>(on_destroy);
        // 注册 create_swapchain 事件回调
        //reshade::register_event<reshade::addon_event::create_swapchain>(&on_create_swapchain);
       // reshade::log::message(reshade::log::level::info, "Successed  register ReShade AAA Expander addon!");

        //reshade::register_event<reshade::addon_event::set_fullscreen_state>(on_set_fullscreen_state);

       // reshade::register_event<reshade::addon_event::reshade_finish_effects>(on_reshade_finish_effects);
        //reshade::register_event<reshade::addon_event::bind_render_targets_and_depth_stencil>(on_bind_render_targets_and_depth_stencil);
       // reshade::register_event<reshade::addon_event::draw>(on_draw);
        break;
    }


    case DLL_PROCESS_DETACH:
    {
        // 注销插件
        reshade::unregister_addon(hModule);
        break;
    }

    }

    return TRUE;
}

