
#pragma once

#include "../pch.hpp"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuiMngr.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace IMGUI
{

    IMGUI::~IMGUI()
    {
        if (m_Handle != 0)
        {
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
            ::DestroyWindow(m_Handle);
            ::UnregisterClassW(m_childClsName, 0);
        }
    };
    IMGUI::IMGUI(HWND parentWindowHandle, ID3D11Device *pDevice, ID3D11DeviceContext *pContext)
    {

        if (pDevice == nullptr || pContext == nullptr)
            return;

        WNDCLASSEXW wndclass{};
        wndclass.cbSize = sizeof(WNDCLASSEXW);
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = WndProc;
        wndclass.cbWndExtra = sizeof(HWND);
        wndclass.hInstance = GetModuleHandle(0);
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.lpszClassName = m_childClsName;

        if (!::RegisterClassExW(&wndclass))
        {
            Error<File>::Write(L"RegisterClassExW failed");
            return;
        }
        RECT rect{};
        GetWindowRect(parentWindowHandle, &rect);

        HWND m_Handle = ::CreateWindowExW(0, m_childClsName, L"IMGUI",
                                          WS_CHILD | WS_VISIBLE /*| WS_CAPTION | WS_SYSMENU | WS_THICKFRAME*/,
                                          0, 0, RECTHEIGHT(rect), 300,
                                          parentWindowHandle,
                                          NULL, GetModuleHandleW(0), nullptr);

        if (m_Handle == 0)
        {
            Error<File>::Write(L"CreateWindowExW failed");
            return;
        };
        SetWindowLongPtrW(m_Handle, -21, (LONG_PTR)this); // GWL_USERDATA

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        // ImGui::StyleColorsDark();
        ImGui::StyleColorsClassic();
        ImGui_ImplWin32_Init(m_Handle);
        ImGui_ImplDX11_Init(pDevice, pContext);
        m_ready = true;
        ::ShowWindow(m_Handle, SW_SHOWDEFAULT);
        ::UpdateWindow(m_Handle);
    };
    LRESULT CALLBACK IMGUI::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        IMGUI *pThis{(IMGUI *)::GetWindowLongPtrW(hwnd, -21)}; // GWL_USERDATA

        if (::ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
        {
            return true;
        }

        switch (message)
        {
        case WM_CLOSE:
            pThis->m_done = true;
            return 0;
        default:
            return DefWindowProcW(hwnd, message, wParam, lParam);
        }
    }

};