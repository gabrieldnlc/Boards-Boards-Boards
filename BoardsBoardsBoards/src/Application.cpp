#define NOMINMAX //Windows API doesn't play well with sol without this
#define SOL_SAFE_NUMERICS 1

#include <string>
#include <optional>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_syswm.h>

#include "Application.hpp"
#include "utils/LuaStack.hpp"
#include "fonts/fonts.h"

namespace board
{
	int Application::Start()
	{
        // - boilerplate code from imgui
		static ID3D11Device* g_pd3dDevice = NULL;
		static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
		static IDXGISwapChain* g_pSwapChain = NULL;
		static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

        const auto CleanupRenderTarget = [&]()
        {
            if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
        };

        const auto CreateRenderTarget = [&]()
        {
            ID3D11Texture2D* pBackBuffer;
            g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
            pBackBuffer->Release();
        };
        const auto CreateDeviceD3D = [&](HWND hWnd)
        {
            DXGI_SWAP_CHAIN_DESC sd;
            ZeroMemory(&sd, sizeof(sd));
            sd.BufferCount = 2;
            sd.BufferDesc.Width = 0;
            sd.BufferDesc.Height = 0;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = hWnd;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;
            sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            UINT createDeviceFlags = 0;
            D3D_FEATURE_LEVEL featureLevel;
            const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
            if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
                return false;

            CreateRenderTarget();
            return true;
        };
        const auto CleanupDeviceD3D = [&]()
        {
            CleanupRenderTarget();
            if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
            if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
            if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
        };        

        utils::LuaStack::Init();
        board::WidgetManager widgets;

        // Setup SDL
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
        {
            printf("Error: %s\n", SDL_GetError());
            return -1;
        }

        // Setup window
        
        
#ifdef BOARD_DEBUG
        const std::string window_title = "Boards, Boards, Boards (SDL + DX11)";
#else
        const std::string window_title = "Boards, Boards, Boards";
#endif
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MAXIMIZED);
        SDL_Window* window = SDL_CreateWindow(window_title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(window, &wmInfo);
        HWND hwnd = (HWND)wmInfo.info.win.window;

        
        if (!CreateDeviceD3D(hwnd))
        {
            CleanupDeviceD3D();
            return 1;
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;      

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0); // Disabling the BG dimming when opening a modal window
        style.Colors[ImGuiCol_WindowBg] = ImColor(61, 61, 61);

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForD3D(window);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

        io.Fonts->AddFontFromMemoryCompressedTTF(KarlaRegular_compressed_data, KarlaRegular_compressed_size, 15.f);

        // Our state
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Main loop
        bool done = false;
        while (!done)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
                if (event.type == SDL_QUIT)
                    done = true;
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                    done = true;
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(window))
                {
                    // Release all outstanding references to the swap chain's buffers before resizing.
                    CleanupRenderTarget();
                    g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
                    CreateRenderTarget();
                }
            }

            // Start the Dear ImGui frame
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            app.Render();

            // Rendering
            ImGui::Render();
            const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            g_pSwapChain->Present(1, 0); // Present with vsync
            //g_pSwapChain->Present(0, 0); // Present without vsync
        }

        // Cleanup
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        CleanupDeviceD3D();
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
	}
}