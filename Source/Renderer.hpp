
#ifndef VISUAL_RENDER_HPP
#define VISUAL_RENDER_HPP
#pragma once
#include "DeviceResource.hpp"

class Renderer
{

    const ::Microsoft::WRL::ComPtr<ID3D11DeviceContext> &GetContext() const { return m_pContext; };
    const ::Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &GetRenderTargetView() const { return m_pRTV; };

protected:
    Renderer() = default;
    HRESULT Initialize();
    void SetPipeLine() const noexcept;
    void UpdateViewPortSizeBuffer(float width, float height) const noexcept;
    void UpdateFrameBuffer() noexcept;
    void SetViewPort(float width, float height) noexcept;
    void Draw() const noexcept;

    std::unique_ptr<DeviceResource> m_pDeviceResource{};

    D3D11_VIEWPORT m_viewPort{0.f, 0.f, 0.f, 0.f, 0.f, 1.f};
    DirectX::XMFLOAT4 RTVClearColor{0.0f, 0.0f, 0.0f, 0.99f};

    Time::Timer m_timer{};

    ::Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext{};
    ::Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pRenderTarget{};
    ::Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRTV{};

    ::Microsoft::WRL::ComPtr<ID3D11Buffer> m_pViewPortSizeBuffer{}; // constant buffer:Changes On resizing
    ::Microsoft::WRL::ComPtr<ID3D11Buffer> m_pFrameBuffer{};        // constant buffer:changes every update

    ::Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout{};
    ::Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader{};
    ::Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader{};
};

#endif // VISUAL_RENDER_HPP