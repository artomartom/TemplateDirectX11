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
    const static std::array<D3D11_INPUT_ELEMENT_DESC, 1> LayoutDescs{
        D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    if (H_FAIL(hr = m_pDeviceResource->CreateVertexShaderFromFile(L"Vertex.so", LayoutDescs.data(), LayoutDescs.size(), &m_pVertexShader, &m_pInputLayout)))
        return hr;
    if (H_FAIL(hr = m_pDeviceResource->CreatePixelShaderFromFile(L"Pixel.so", &m_pPixelShader)))
        return hr;
    // ComPtr<ID3DBlob> VSByteCode{};
    //  if (H_FAIL(hr = m_pDeviceResource->CompileSOFromFile(L" main.hlsl", "VSMain", "vs_5_0", &VSByteCode)))
    //      return hr;
    // if (H_FAIL(hr = D3DReadFileToBlob(L"Vertex.so", &VSByteCode)))
    //     return hr;
    // if (H_FAIL(hr = m_pDeviceResource->CreateVertexShaderFromBlob(LayoutDescs.data(), LayoutDescs.size(), VSByteCode.Get(), &m_pVertexShader, &m_pInputLayout)))
    //     return hr;
    // ComPtr<ID3DBlob> PSByteCode{};
    //  if (H_FAIL(hr = m_pDeviceResource->CompileSOFromFile(L" main.hlsl", "PSMain", "ps_5_0", &PSByteCode)))
    //      return hr;
    // if (H_FAIL(hr = D3DReadFileToBlob(L"Pixel.so", &PSByteCode)))
    //     return hr;
    // if (H_FAIL(hr = m_pDeviceResource->CreatePixelShaderFromBlob(PSByteCode.Get(), &m_pPixelShader)))
    //     return hr;
    /**
     *     Create Constant Buffers
     */
    {

        // ViewPortSizeBuffer   /*    */ m_pViewPortSizeBuffer
        // FrameBuffer   /*           */ m_pFrameBuffer

        D3D11_BUFFER_DESC d_ConstBuffer{};

        d_ConstBuffer.Usage = D3D11_USAGE_DEFAULT;
        d_ConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        d_ConstBuffer.CPUAccessFlags = 0;
        d_ConstBuffer.ByteWidth = (sizeof(ViewPortSizeBuffer) + 15) / 16 * 16;
        if (H_FAIL(hr = m_pDeviceResource->GetDevice()->CreateBuffer(&d_ConstBuffer, nullptr, &m_pViewPortSizeBuffer)))
            return hr;

        d_ConstBuffer.Usage = D3D11_USAGE_DEFAULT;
        d_ConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        d_ConstBuffer.CPUAccessFlags = 0;
        d_ConstBuffer.ByteWidth = (sizeof(FrameBuffer) + 15) / 16 * 16;
        if (H_FAIL(hr = m_pDeviceResource->GetDevice()->CreateBuffer(&d_ConstBuffer, nullptr, &m_pFrameBuffer)))
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

void Renderer::UpdateViewPortSizeBuffer(float Width, float Height) const noexcept
{

    ViewPortSizeBuffer ViewPortSize{Width, Height};
    m_pContext->UpdateSubresource(
        m_pViewPortSizeBuffer.Get(),
        0, nullptr, &ViewPortSize, 0, 0);
};

void Renderer::UpdateFrameBuffer() noexcept
{

    // Update Timer
    FrameBuffer constantBuffer(Timer.Count<long>(), Timer.GetDelta<double>());
    m_pContext->UpdateSubresource(
        m_pFrameBuffer.Get(),
        0, nullptr, &constantBuffer, 0, 0);
};

void Renderer::Draw() const noexcept
{

    m_pContext->ClearRenderTargetView(Renderer::m_pRTV.Get(), &RTVClearColor.x);
    m_pContext->RSSetViewports(1, &m_ViewPort);
    m_pContext->OMSetRenderTargets(1u, Renderer::m_pRTV.GetAddressOf(), nullptr);
    m_pContext->Draw(6u, 0u);
};