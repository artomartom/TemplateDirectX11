#pragma once

#include "pch.hpp"
#include "Renderer.hpp"
#include "DeviceResource.hpp"

#define CASE(message, action) \
  case message:               \
    action;                   \
    break

using ::Microsoft::WRL::ComPtr;

class App : public CoreApp, public Renderer
{

public:
  static int AppEntry(HINSTANCE hinst)
  {
    DBG_ONLY(_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF));

    peekRun(Window::CoreWindow<App>{hinst, {50, 50, 1700, 1000}});
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

  void OnCreate(_In_ const ::Window::CreationArgs &args) noexcept
  {
    HRESULT hr{};

    SIZE RTSize{RECTWIDTH(args.Rect), RECTHEIGHT(args.Rect)};
    m_pDeviceResource = std::make_unique<DeviceResource>(&m_pContext, &hr);
    if (H_OK(hr))
    {
      // CreateSizeDependentDeviceResources is not called from here because of OnSizeChanged message being send automatically  after OnCreate's success
      if (H_OK(hr = Renderer::Initialize()))
      {
        Renderer::SetPipeLine();
        return;
      };
    };

    DBG_ONLY(
        {
          Log<Console>::Write(L"Pulling Debug Messages before closing...");
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
      /**
       *   The first argument instructs DXGI to block until VSync, putting the application
       *  to sleep until the next VSync. This ensures we don't waste any cycles rendering
       *  frames that will never be displayed to the screen.
       */
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

  template <class TCoreWindow>
  friend int __stdcall peekRun(TCoreWindow &&window)
  {
    if (window.m_shouldClose)
    {
      Log<File>::Write(L"App Creation canceled");
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