#pragma once

#include "pch.hpp"

#include "DeviceResource.hpp"
#include "TargetWindow.hpp"

#ifndef IMGUI_DISABLE
#include "ImGui//imgui.h"
#include "ImGui//imgui_impl_win32.h"
#include "ImGui//imgui_impl_dx11.h"
#endif // IMGUI_DISABLE

using namespace Window;
using ::Microsoft::WRL::ComPtr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class lolez : public Window::Impl
{
public:
  void init(ID3D11Device *pDevice, ID3D11DeviceContext *pContext)
  {
    m_pDevice = pDevice;
    m_pContext = pContext;
    // if (H_FAIL(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0, D3D11_SDK_VERSION, &m_pDevice, 0, &m_pContext)))
    //   return;

    ::SetWindowLongPtrW(Handle::Get(), (-4), (LONG_PTR)ImGui_ImplWin32_WndProcHandler); // GWL_WNDPROC
    ::SetWindowPos(Handle::Get(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();
    ImGui_ImplWin32_Init(Handle::Get());
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);
    m_ready = true;
  };

  template <typename Func>
  void RenderFrame(Func func)
  {
    static bool show_demo_window{};
    using namespace ImGui;
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    NewFrame();

    //    SetNextWindowSize(ImVec2(m_rect.right, m_rect.bottom));
    func();

    Render();
    ImGui_ImplDX11_RenderDrawData(GetDrawData());
  };

  ~lolez()
  {
    if (Handle::Get() != 0)
    {
      ImGui_ImplDX11_Shutdown();
      ImGui_ImplWin32_Shutdown();
      ImGui::DestroyContext();
      ::DestroyWindow(Handle::Get());
      ::UnregisterClassW(L"IMGUI class", 0);
    };
    
  }

private:
  ID3D11Device *m_pDevice{};
  ID3D11DeviceContext *m_pContext{};

  bool m_done{false};
  bool m_ready{false};
};

class App : public Impl
{
public:
  App()
  {
    InitWindow(RECT{100, 100, 800, 1000}, nullptr, this);
    InitWindow(RECT{200, 300, 500, 600}, this, &m_target);
    m_shouldClose = m_target.m_shouldClose;
  };

  static int AppEntry(HINSTANCE hinst)
  {
    DBG_ONLY(_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF));
    App app{};
    if (app.m_shouldClose)
    {
      Error<File>::Write(L"App Creation canceled");
      return 1;
    };
   //lolez m_Gui{};
   //InitWindow(RECT{200, 300, 500, 600}, &app, &m_Gui);
   //m_Gui.init(app.m_target.GetDevice().Get(), app.m_target.GetContext().Get());
     

    while (PeekEvent(app) != WM_QUIT)
    {
      app.m_target.Draw();

     //m_Gui.RenderFrame([&]()
     //                  {
     //                          ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.
     //                        //  ImGui::ColorEdit3("clear color",  Renderer::m_RTVClearColor); // Edit 3 floats representing a color
     //                          ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
     //                          ImGui::End(); });
    };
    MessageBeep(5);
    return 0;
  };
  

  bool m_shouldClose{false};

protected:
  bool m_shouldDraw{true};
  bool m_isVisible{false};

private:
  TargetWnd m_target{};
};

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
int wWinMain(_In_ HINSTANCE hinst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) { return Invoke(&App::AppEntry, hinst); };
int main() { return Invoke(&App::AppEntry, (HINSTANCE)&__ImageBase); };
