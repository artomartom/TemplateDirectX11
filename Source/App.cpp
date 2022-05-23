#pragma once

#include "pch.hpp"
#include "Renderer.hpp"
#include "DeviceResource.hpp"

#ifndef IMGUI_DISABLE
#include "ImGuiMngr.hpp"
#endif // IMGUI_DISABLE

#define CASE(message, action) \
  case message:               \
    action;                   \
    break

using ::Microsoft::WRL::ComPtr;

class App : public CoreApp, public Renderer
{

public:
  App()
  {
    //    m_style|= WS_CLIPCHILDREN;//Excludes the area occupied by child windows when drawing occurs within the parent window.
  }
  static int AppEntry(HINSTANCE hinst)
  {
    DBG_ONLY(_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF));
    peekRun(Window::CoreWindow<App>{hinst, {550, 50, 1200, 700}});

    MessageBeep(5);
    return 0;
  };
  void OnWindowActivate(_In_ const ::Window::ActivateArgs &args) noexcept
  {

    if (CoreApp::m_isVisible != args.isMinimized)
    {
      CoreApp::m_isVisible = args.isMinimized;
      m_shouldDraw = !CoreApp::m_isVisible;
    };
  };

  void OnKeyStroke(_In_ const ::Window::KeyEventArgs &args) noexcept
  {
    switch (args.virtualKey)
    {
      CASE(VK_ESCAPE, { CoreApp::Close(); });
      CASE(VK_SPACE, { Renderer::m_timer.Switch(); });
    }
  };
  void OnCursorMove(_In_ const ::Window::CursorArgs &args) noexcept {/*Log<File>::Write(args.pos.x, args.pos.y); */};
  void OnCreate(_In_ const ::Window::CreationArgs &args) noexcept
  {
    HRESULT hr{};

    SIZE RTSize{RECTWIDTH(args.rect), RECTHEIGHT(args.rect)};
    m_pDeviceResource = std::make_unique<DeviceResource>(&m_pContext, &hr);
    if (H_OK(hr))
    {
      // CreateSizeDependentDeviceResources is not called from here because of OnSizeChanged message being send automatically  after OnCreate's success
      if (H_OK(hr = Renderer::Initialize()))
      {
        Renderer::SetPipeLine();
#ifndef IMGUI_DISABLE
        m_pImGui = std::make_unique<IMGUI::IMGUI>(m_handle, m_pDeviceResource->GetDevice().Get(), m_pContext.Get());
#endif // IMGUI_DISABLE
        return;
      };
    };

    DBG_ONLY(
        {
          m_pDeviceResource->PullDebugMessage();
        });
    m_shouldClose = true;
  };
  void OnSizeChanged(_In_ const ::Window::SizeChangedArgs &args) noexcept
  {
    float newWidth{static_cast<float>(args.newSize.width)};
    float newHeight{static_cast<float>(args.newSize.height)};

    switch (args.changeType)
    {
      CASE(::Window::SizeChangedArgs::ChangeType::Minimized, {m_pDeviceResource->GetSwapChain()->SetFullscreenState(false, nullptr); break; });
      CASE(::Window::SizeChangedArgs::ChangeType::Maximized, {m_pDeviceResource->GetSwapChain()->SetFullscreenState(true, nullptr); break; });
    }
    if (m_viewPort.Width == newWidth && m_viewPort.Height == newHeight)
    {
      return;
    }
    else
    {
      Renderer::m_viewPort.Width = newWidth;
      Renderer::m_viewPort.Height = newHeight;
      m_pDeviceResource->CreateSizeDependentDeviceResources(m_handle, m_viewPort, m_pContext.Get(), &m_pRenderTarget, &m_pRTV);
      Renderer::UpdateViewPortSizeBuffer(newWidth, newHeight);
    }
  };

  void Draw() noexcept
  {

    if (m_shouldDraw && !m_shouldClose)
    {
      Renderer::UpdateFrameBuffer();

      Renderer::Draw();

#ifndef IMGUI_DISABLE
      m_pImGui->RenderFrame([&]()
                            {
                               
                              ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.
                              ImGui::ColorEdit3("clear color",  Renderer::m_RTVClearColor); // Edit 3 floats representing a color
                              ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
                              ImGui::End(); });
#endif // IMGUI_DISABLE

      H_FAIL(m_pDeviceResource->Present());
    };
    DBG_ONLY(m_pDeviceResource->DebugInterface::PullDebugMessage());
  };

  void OnClose() noexcept
  {
    /**
     * You may not release a swap chain in full-screen mode
     * because doing so may create thread contention (which
     * will cause DXGI to raise a non-continuable exception).
     */
    m_pDeviceResource->GetSwapChain()->SetFullscreenState(false, nullptr);
  };

private:
  bool m_shouldDraw{true};
  bool m_shouldClose{false};
#ifndef IMGUI_DISABLE
  std::unique_ptr<IMGUI::IMGUI> m_pImGui{};
#endif // IMGUI_DISABLE
  template <class TCoreWindow>
  friend int __stdcall peekRun(TCoreWindow &&window)
  {
    if (window.m_shouldClose)
    {
      Error<File>::Write(L"App Creation canceled");
      return 1;
    };

    ::MSG messages{};

    while (messages.message != WM_QUIT)
    {

      ::PeekMessageW(&messages, 0, 0, 0, PM_REMOVE);
      ::TranslateMessage(&messages);
      ::DispatchMessageW(&messages);

      window.App::Draw();
    };

    return 0;
  };
};

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
int wWinMain(_In_ HINSTANCE hinst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) { return Invoke(&App::AppEntry, hinst); };
int main() { return Invoke(&App::AppEntry, (HINSTANCE)&__ImageBase); };
