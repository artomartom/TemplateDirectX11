#include "pch.hpp"
#include "TargetWindow.hpp"

#define CASE(message, action) \
    case message:             \
        action;               \
        break

using namespace Window;

ActivateArgs::Status TargetWnd::OnWindowActivate(_In_ const ActivateArgs &args) noexcept
{
    if (m_isVisible != args.isMinimized)
    {
        m_isVisible = args.isMinimized;
        m_shouldDraw = !m_isVisible;
    };
    return ActivateArgs::Status{};
};

KeyEventArgs::Status TargetWnd::OnKeyStroke(_In_ const KeyEventArgs &args) noexcept
{
    switch (args.virtualKey)
    {
        CASE(VK_ESCAPE, {});
        CASE(VK_SPACE, { Renderer::m_timer.Switch(); });
    }
    return KeyEventArgs::Status{};
};
CreationArgs::Status TargetWnd::OnCreate(_In_ const CreationArgs &args) noexcept
{
    HRESULT hr{};
    m_pDeviceResource = std::make_unique<DeviceResource>(&m_pContext, &hr);

    if (H_OK(hr))
    {

        // CreateSizeDependentDeviceResources is not called from here because of OnSizeChanged message being send automatically  after OnCreate's success
        if (H_OK(hr = Renderer::Initialize()))
        {

            Renderer::SetPipeLine();

            return CreationArgs::Status{};
        };
    };

    DBG_ONLY(
        {
            m_pDeviceResource->PullDebugMessage();
        });
    m_shouldClose = true;
    return CreationArgs::Status{};
};
SizeChangedArgs::Status TargetWnd::OnSizeChanged(_In_ const SizeChangedArgs &args) noexcept
{
    float newWidth{static_cast<float>(args.newSize.width)};
    float newHeight{static_cast<float>(args.newSize.height)};

   // switch (args.changeType)
   // {
   //     CASE(SizeChangedArgs::ChangeType::Minimized, {m_pDeviceResource->GetSwapChain()->SetFullscreenState(false, nullptr); break; });
   //     CASE(SizeChangedArgs::ChangeType::Maximized, {m_pDeviceResource->GetSwapChain()->SetFullscreenState(true, nullptr); break; });
   // }
    if (m_viewPort.Width == newWidth && m_viewPort.Height == newHeight )
    {
        return SizeChangedArgs::Status{};
    }
    else
    {

        Renderer::m_viewPort.Width = newWidth;
        Renderer::m_viewPort.Height = newHeight;
        m_pDeviceResource->CreateSizeDependentDeviceResources(Handle::Get(), m_viewPort, m_pContext.Get(), &m_pRenderTarget, &m_pRTV);
        Renderer::UpdateViewPortSizeBuffer(newWidth, newHeight);
    }
    return SizeChangedArgs::Status{};
};

void TargetWnd::Draw() noexcept
{

    if (m_shouldDraw && !m_shouldClose)
    {

        Renderer::UpdateFrameBuffer();
        Renderer::Draw();
        H_FAIL(m_pDeviceResource->Present());
    };
    DBG_ONLY(m_pDeviceResource->DebugInterface::PullDebugMessage());
    return;
};

CloseArgs::Status TargetWnd::OnClose(const CloseArgs &args) noexcept
{
    /**
     * You may not release a swap chain in full-screen mode
     * because doing so may create thread contention (which
     * will cause DXGI to raise a non-continuable exception).
     */
    m_pDeviceResource->GetSwapChain()->SetFullscreenState(false, nullptr);
    return CloseArgs::Status{};
};
