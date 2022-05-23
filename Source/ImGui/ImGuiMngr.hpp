
#pragma once
#include "imgui.h"


IMGUI_IMPL_API void     ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data);
IMGUI_IMPL_API void     ImGui_ImplWin32_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplDX11_NewFrame();
namespace IMGUI
{

    class IMGUI
    {
    public:
        IMGUI() = default;
        ~IMGUI();

        IMGUI(HWND parentWindowHandle, ID3D11Device *pDevice, ID3D11DeviceContext *pContext);
        template <typename Func>
        void RenderFrame(Func func)
        {
            // Start the Dear ImGui frame
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            func();
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        };

    private:
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    private:
        bool m_done{false};
        bool m_ready{false};

        HWND m_Handle{};
        LPCWSTR m_childClsName{L"IMGUI class"};
    };

};