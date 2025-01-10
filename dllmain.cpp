﻿// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "ChiliWin.h"
#include "pch.h"
#include <sstream>
#include <vector>
#include "reshade.hpp"
#include "config.hpp"
#include "App.h"
#include "DX11Graphics.h"

#define MAX_BACKBUF_COUNT	3

//Render target format, DX and reshade have its own definations
#define  RTVFormat DXGI_FORMAT_R8G8B8A8_UNORM

using namespace reshade;
using namespace reshade::api;

extern bool save_texture_image(const resource_desc &desc, const subresource_data &data);





static reshade::api::device* re_device = nullptr;
App *app;
Window *window;
HWND g_hWnd;
HINSTANCE g_hInstance;
static int width = 2048;
static int height = 768;



LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{



	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0); return 0;
		break;


	case WM_KILLFOCUS:
		break;
		/********************KEYBOARD MESSAGE******************************/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// stifle this keyboard message if imgui wants to capture
		break;


	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
};


HWND CreateWindowInDll()
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsg;
	//wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInstance;
	wc.hIcon = nullptr;
	/*wc.hIcon = static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON2),
		IMAGE_ICON, 32, 32, 0
		));*/
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = "Chili Direct3D Engine Window";
	wc.hIconSm = nullptr;
	/*wc.hIconSm = static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON2),
		IMAGE_ICON, 16, 16, 0
		));*/

	if (!RegisterClassEx(&wc))
	{
		MessageBoxA(NULL, "Error register Window class", "Error", MB_OK);

	};

	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;

	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{

	};
	// create window & get hWnd
	g_hWnd = CreateWindow(
		"Chili Direct3D Engine Window", "Dog Shit",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, g_hInstance, NULL
	);
	// show window
	if (g_hWnd == nullptr)
	{
		reshade::log::message(reshade::log::level::error, "Create Chili Window Failed");
		// Init ImGui Win32 Impl
	}
	reshade::log::message(reshade::log::level::error, "Create Chili Window Successed");
	ShowWindow(g_hWnd, SW_SHOWDEFAULT);

	UpdateWindow(g_hWnd);

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
};




//static reshade::api::resource host_resource = { 0 };

static void on_init(reshade::api::device *device)
{
	re_device = device;
	while (g_hWnd == NULL)
		Sleep(10);

    frame_capture &data = device->create_private_data<frame_capture>();
    // Create a fence that is used to communicate status of copies between device and host
    /*if (!device->create_fence(0, reshade::api::fence_flags::none, &data.copy_finished_fence))
    {
        reshade::log::message(reshade::log::level::error, "Failed to create copy fence!");
    }*/
	
	ID3D11Device *d3d11_device = ((ID3D11Device *)re_device->get_native());
	data.dx11Graphics.Init_Resource(d3d11_device,g_hWnd);
}

static void on_present(command_queue *queue, swapchain *swapchain, const rect *, const rect *, uint32_t, const rect *)
{ 

	uint32_t BackBufferCount = swapchain->get_back_buffer_count();
	reshade::log::message(reshade::log::level::info, ("BackBufferCount: " + std::to_string(BackBufferCount)).c_str());

	/*if (&window->Gfx())
	{
		window->Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
		window->Gfx().EndFrame();
	}*/

	

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
			
			app->CopyRTVResource((ID3D11Device *)device->get_native(),(ID3D11Resource *)data.host_resources[0].handle, desc);
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
	HWND  wnd = CreateWindowInDll();
	MSG msg;
	while (GetMessage(&msg, wnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}



extern "C" __declspec(dllexport) const char *NAME = "AAA Game Expander";
extern "C" __declspec(dllexport) const char *DESCRIPTION = "AAA Expander Game Graphics to 3840.";



BOOL APIENTRY DllMain(HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
)
{
	

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		
		// 注册插件
		if (!reshade::register_addon(hModule))
		{
			std::stringstream s;
			s << "Failed to register AAA Expander addon!";
			reshade::log::message(reshade::log::level::error, s.str().c_str());
			return FALSE;
		}
		reshade::log::message(reshade::log::level::info, "Succeed to register AAA Expander addon!");

		g_hInstance = hModule;
		/*app = new App();
		app->SetDllInstance(hModule); */
		//CreateThread(NULL, 0,WindowThreadProc, NULL, 0, NULL);
		CreateThread(NULL, 0, WindowThreadProc, NULL, 0, NULL);

		

		reshade::register_event<reshade::addon_event::init_device>(on_init);
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

