

#pragma once

#include "pch.hpp"
#include "DeviceResource.hpp"
#include <d3dcompiler.h>

using ::Microsoft::WRL::ComPtr;
using namespace ::DirectX;

DeviceResource::DeviceResource(_COM_Outptr_result_maybenull_ ID3D11DeviceContext **ppContext, _Out_ HRESULT *hr)
    : m_numBackBuffers{2}
{
   HRESULT localhr{};
   if (hr == nullptr)
      hr = &localhr;
   //
   //  Create Actual Device

   unsigned int flags{};
   DBG_ONLY(flags |= D3D11_CREATE_DEVICE_DEBUG);

   H_CHECK(*hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &m_thisFeatureLevel, ppContext),
           L"D3D11CreateDevice  failed");

   DBG_ONLY(
       {
          if (D3D11_CREATE_DEVICE_DEBUG && m_pDevice->GetCreationFlags())
             if (H_FAIL(*hr = DebugInterface::Init(m_pDevice)))
                return;
       });

   ComPtr<IDXGIDevice4> pDXGIDevice{};
   ComPtr<IDXGIAdapter> pDXGIAdapter{};

   if (H_FAIL(*hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice4), (void **)&pDXGIDevice)))
      return;

   if (H_FAIL(*hr = pDXGIDevice->GetAdapter(&pDXGIAdapter)))
      return;

   if (H_FAIL(*hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory4), (void **)&m_pDXGIFactory)))
      return;
   SETDBGNAME_COM(m_pDXGIFactory);
};

HRESULT DeviceResource::CreateSizeDependentDeviceResources(
    _In_ const HWND &windowHandle,
    _In_ D3D11_VIEWPORT NewViewPort,
    _In_opt_ ID3D11DeviceContext *pContext,
    __inout_opt ID3D11Texture2D **ppRTVBuffer,
    __inout_opt ID3D11RenderTargetView **ppRTV)
{
   HRESULT hr{};

   if (!windowHandle)
   {
      return ERROR_INVALID_HANDLE_STATE;
   };

   pContext->OMSetRenderTargets(0, nullptr, nullptr);

   if ((*ppRTV) != nullptr)
      assert((*ppRTV)->Release() == 0);
   if ((*ppRTVBuffer) != nullptr)
      assert((*ppRTVBuffer)->Release() == 0);
   pContext->Flush();

   if (m_pSwapChain)
   {

      H_FAIL(hr = m_pSwapChain->ResizeBuffers(
                 GetNumBackBuffer(),
                 NewViewPort.Width,
                 NewViewPort.Height,
                 DXGI_FORMAT_UNKNOWN,
                 0u));

      if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
      {
         H_FAIL(HandleDeviceRemoved());
      };
   }
   else
   {

      DXGI_SWAP_CHAIN_DESC1 d_swapChain{};
      d_swapChain.Width = static_cast<UINT>(NewViewPort.Width);
      d_swapChain.Height = static_cast<UINT>(NewViewPort.Height);
      d_swapChain.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
      d_swapChain.Stereo = false;
      d_swapChain.SampleDesc = {1, 0};
      d_swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      d_swapChain.BufferCount = m_numBackBuffers;
      d_swapChain.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
      d_swapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
      d_swapChain.AlphaMode = DXGI_ALPHA_MODE ::DXGI_ALPHA_MODE_UNSPECIFIED;
      d_swapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

      assert(d_swapChain.Height != 0);
      assert(d_swapChain.Format != 0);
      DXGI_SWAP_CHAIN_FULLSCREEN_DESC d_fullScreenSwapChain{};
      d_fullScreenSwapChain.RefreshRate = {0u, 1u};
      d_fullScreenSwapChain.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER ::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      d_fullScreenSwapChain.Scaling = DXGI_MODE_SCALING ::DXGI_MODE_SCALING_STRETCHED;
      d_fullScreenSwapChain.Windowed = true;
      if (H_FAIL(hr = m_pDXGIFactory->CreateSwapChainForHwnd(m_pDevice.Get(), windowHandle, &d_swapChain, &d_fullScreenSwapChain, nullptr, &m_pSwapChain)))
         return hr;
   };
   if (H_FAIL(hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)ppRTVBuffer)))
      return hr;
   if (H_FAIL(hr = m_pDevice->CreateRenderTargetView(*ppRTVBuffer, 0, ppRTV)))
      return hr;

   return S_OK;
};

HRESULT CreateDeviceResources()
{
   HRESULT hr{};

   return S_OK;
};

HRESULT DeviceResource::CompileSOFromFile(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _COM_Outptr_result_maybenull_ ID3DBlob **blob)
{
   if (!srcFile || !entryPoint || !profile || !blob)
      return E_INVALIDARG;
   *blob = nullptr;
   UINT flags{}; //= D3DCOMPILE_ENABLE_STRICTNESS;
   DBG_ONLY({ flags |= D3DCOMPILE_DEBUG; });

   ID3DBlob *shaderBlob = nullptr, *errorBlob = nullptr;
   HRESULT hr = D3DCompileFromFile(srcFile, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                   entryPoint, profile,
                                   flags, 0, &shaderBlob, &errorBlob);
   if (FAILED(hr))
   {
      if (errorBlob)
      {
         Error<File>::Write((char *)errorBlob->GetBufferPointer());
         errorBlob->Release();
      }
      if (shaderBlob)
         shaderBlob->Release();
   }
   else
      *blob = shaderBlob;
   return hr;
};

HRESULT DeviceResource::CreateVertexShader(
    _In_reads_(count) const D3D11_INPUT_ELEMENT_DESC *pDescs,
    _In_ UINT count,
    _In_reads_(byteCodeLength) const void *pShaderByteCode,
    _In_ SIZE_T byteCodeLength,
    _COM_Outptr_result_maybenull_ ID3D11VertexShader **ppShader,
    _COM_Outptr_result_maybenull_ ID3D11InputLayout **pInputLayout)
{
   HRESULT hr{};
   if (H_FAIL(hr = m_pDevice->CreateVertexShader(
                  pShaderByteCode, byteCodeLength, nullptr, ppShader)))
      return hr;
   if (H_FAIL(hr = m_pDevice->CreateInputLayout(pDescs, count, pShaderByteCode, byteCodeLength, pInputLayout)))
      return hr;

   return S_OK;
};

HRESULT DeviceResource::CreateVertexShaderFromBlob(
    _In_reads_(count) const D3D11_INPUT_ELEMENT_DESC *pDescs,
    _In_ UINT count,
    _In_ ID3DBlob *Blob,
    _COM_Outptr_result_maybenull_ ID3D11VertexShader **ppShader,
    _COM_Outptr_result_maybenull_ ID3D11InputLayout **pInputLayout)
{
   return CreateVertexShader(pDescs, count, Blob->GetBufferPointer(), Blob->GetBufferSize(), ppShader, pInputLayout);
};
HRESULT DeviceResource::CreateVertexShaderFromFile(
    LPCWSTR filename,
    _In_reads_(count) const D3D11_INPUT_ELEMENT_DESC *pDescs,
    _In_ UINT count,
    _COM_Outptr_result_maybenull_ ID3D11VertexShader **ppShader,
    _COM_Outptr_result_maybenull_ ID3D11InputLayout **pInputLayout)
{
   HRESULT hr{};
   ComPtr<ID3DBlob> blob{};
   if (H_FAIL(hr = D3DReadFileToBlob(filename, &blob)))
      return hr;
   return CreateVertexShaderFromBlob(pDescs, count, blob.Get(), ppShader, pInputLayout);
};

HRESULT DeviceResource::CreatePixelShader(
    _In_reads_(byteCodeLength) const void *pShaderByteCode,
    _In_ SIZE_T byteCodeLength,
    _COM_Outptr_result_maybenull_ ID3D11PixelShader **ppShader)
{
   return m_pDevice->CreatePixelShader(pShaderByteCode, byteCodeLength, nullptr, ppShader);
};

HRESULT DeviceResource::CreatePixelShaderFromBlob(
    _In_ ID3DBlob *Blob,
    _COM_Outptr_result_maybenull_ ID3D11PixelShader **ppShader)
{
   return CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), ppShader);
};

HRESULT DeviceResource::CreatePixelShaderFromFile(
    LPCWSTR filename,
    _COM_Outptr_result_maybenull_ ID3D11PixelShader **ppShader)
{
   HRESULT hr{};
   ComPtr<ID3DBlob> blob{};
   if (H_FAIL(hr = D3DReadFileToBlob(filename, &blob)))
      return hr;
   return CreatePixelShaderFromBlob(blob.Get(), ppShader);
};