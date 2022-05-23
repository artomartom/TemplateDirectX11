#pragma once

#include "pch.hpp"
#include "Renderer.hpp"

using ::Microsoft::WRL::ComPtr;
using namespace ::DirectX;

struct ViewPortSizeBuffer
{
    explicit ViewPortSizeBuffer(float x, float y)
        : ViewPortSize{x, y}, Resolution{1.0f, x / y} {};
    DirectX::XMFLOAT2 ViewPortSize{};
    DirectX::XMFLOAT2 Resolution{};
};

struct FrameBuffer
{

    explicit FrameBuffer(long long st, double deltaT)
        : t{st / 1000.f,                 /// sec.milices
            (st % (1000 * 60)) / 1000.f, // //sec.milices % 60 min
            deltaT,                      // time delta
            deltaT} {/* Log<Console>::Write(t.x, t.y, t.z);*/};
    ::DirectX::XMFLOAT4 t{};
};

HRESULT Renderer::Initialize()
{
    HRESULT hr{};
    const static std::array<D3D11_INPUT_ELEMENT_DESC, 1> layoutDescs{
        D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    if (H_FAIL(hr = m_pDeviceResource->CreateVertexShaderFromFile(L"Vertex.so", layoutDescs.data(), layoutDescs.size(), &m_pVertexShader, &m_pInputLayout)))
        return hr;
    if (H_FAIL(hr = m_pDeviceResource->CreatePixelShaderFromFile(L"Pixel.so", &m_pPixelShader)))
        return hr;

    /**
     *     Create Constant Buffers
     */
    {

        // ViewPortSizeBuffer   /*    */ m_pViewPortSizeBuffer
        // FrameBuffer   /*           */ m_pFrameBuffer

        D3D11_BUFFER_DESC d_constBuffer{};

        d_constBuffer.Usage = D3D11_USAGE_DEFAULT;
        d_constBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        d_constBuffer.CPUAccessFlags = 0;
        d_constBuffer.ByteWidth = (sizeof(ViewPortSizeBuffer) + 15) / 16 * 16;
        if (H_FAIL(hr = m_pDeviceResource->GetDevice()->CreateBuffer(&d_constBuffer, nullptr, &m_pViewPortSizeBuffer)))
            return hr;

        d_constBuffer.Usage = D3D11_USAGE_DEFAULT;
        d_constBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        d_constBuffer.CPUAccessFlags = 0;
        d_constBuffer.ByteWidth = (sizeof(FrameBuffer) + 15) / 16 * 16;
        if (H_FAIL(hr = m_pDeviceResource->GetDevice()->CreateBuffer(&d_constBuffer, nullptr, &m_pFrameBuffer)))
            return hr;
    };

    return S_OK;
};

void Renderer::SetPipeLine() const noexcept
{

    ID3D11Buffer *buffers[]{nullptr,  // no vertex buffer
                            nullptr}; // no instance buffer
    UINT strides[]{0u, 0u};
    UINT offsets[]{0u, 0u};

    ID3D11Buffer *constBuffers[]{
        m_pViewPortSizeBuffer.Get(),
        m_pFrameBuffer.Get(),
    };

    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pContext->IASetInputLayout(m_pInputLayout.Get());
    m_pContext->IASetVertexBuffers(0u, 1u, buffers, strides, offsets);

    m_pContext->VSSetConstantBuffers(0, _countof(constBuffers), constBuffers);
    m_pContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0u);

    m_pContext->PSSetConstantBuffers(0, _countof(constBuffers), constBuffers);
    m_pContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0u);
};

void Renderer::UpdateViewPortSizeBuffer(float width, float height) const noexcept
{

    ViewPortSizeBuffer viewPortSize{width, height};
    m_pContext->UpdateSubresource(
        m_pViewPortSizeBuffer.Get(),
        0, nullptr, &viewPortSize, 0, 0);
};

void Renderer::UpdateFrameBuffer() noexcept
{

    // Update Timer
    m_timer.Count();
    FrameBuffer constantBuffer(m_timer.GetCount<long long>(), m_timer.GetDelta<double>());
    m_pContext->UpdateSubresource(
        m_pFrameBuffer.Get(),
        0, nullptr, &constantBuffer, 0, 0);
};

void Renderer::Draw() const noexcept
{

    m_pContext->ClearRenderTargetView(Renderer::m_pRTV.Get(), m_RTVClearColor);
    m_pContext->RSSetViewports(1, &m_viewPort);
    m_pContext->OMSetRenderTargets(1u, Renderer::m_pRTV.GetAddressOf(), nullptr);
    m_pContext->Draw(6u, 0u);
};