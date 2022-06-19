#pragma once
#include "Renderer.hpp"

class TargetWnd : public Window::Impl, public Renderer
{
public:
 
    virtual ::Window::ActivateArgs::Status OnWindowActivate(_In_ const ::Window::ActivateArgs &args) noexcept override;
    virtual ::Window::KeyEventArgs::Status OnKeyStroke(_In_ const ::Window::KeyEventArgs &args) noexcept override;
    virtual ::Window::CreationArgs::Status OnCreate(_In_ const ::Window::CreationArgs &args) noexcept override;
    virtual ::Window::SizeChangedArgs::Status OnSizeChanged(_In_ const ::Window::SizeChangedArgs &args) noexcept override;
    virtual ::Window::CloseArgs::Status OnClose(const ::Window::CloseArgs &args) noexcept override;
    void Draw() noexcept;

    
  bool m_shouldClose{false};

protected:
private:
  bool m_shouldDraw{true};
  bool m_isVisible{false};


 
};