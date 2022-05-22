
#ifndef VISUAL_DEVICE_HPP
#define VISUAL_DEVICE_HPP

#include "DebugInterface.hpp"

class DeviceResource : public DebugInterface
{

public:
  DeviceResource(_COM_Outptr_result_maybenull_ ID3D11DeviceContext **ppContext, _Out_ HRESULT *hr);

  HRESULT CreateSizeDependentDeviceResources(
      _In_ const HWND &windowHandle,
      _In_ D3D11_VIEWPORT NewViewPort,
      _In_opt_ ID3D11DeviceContext *pContext,
      __inout_opt ID3D11Texture2D **ppRTVBuffer,
      __inout_opt ID3D11RenderTargetView **ppRTV);

  HRESULT HandleDeviceRemoved() { return ERROR_CALL_NOT_IMPLEMENTED; };
  HRESULT Present() const noexcept { return m_pSwapChain->Present(1u, 0u); };

  // compile shader object
  HRESULT CompileSOFromFile(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _COM_Outptr_result_maybenull_ ID3DBlob **blob);

  // create vertex shader
  HRESULT CreateVertexShader(
      _In_reads_(count) const D3D11_INPUT_ELEMENT_DESC *pDescs,
      _In_ UINT count,
      _In_reads_(byteCodeLength) const void *pShaderByteCode,
      _In_ SIZE_T byteCodeLength,
      _COM_Outptr_result_maybenull_ ID3D11VertexShader **ppShader,
      _COM_Outptr_result_maybenull_ ID3D11InputLayout **pInputLayout);

  HRESULT CreateVertexShaderFromBlob(
      _In_reads_(count) const D3D11_INPUT_ELEMENT_DESC *pDescs,
      _In_ UINT count,
      _In_ ID3DBlob *Blob,
      _COM_Outptr_result_maybenull_ ID3D11VertexShader **ppShader,
      _COM_Outptr_result_maybenull_ ID3D11InputLayout **pInputLayout);

  HRESULT CreateVertexShaderFromFile(
      LPCWSTR filename,
      _In_reads_(count) const D3D11_INPUT_ELEMENT_DESC *pDescs,
      _In_ UINT count,
      _COM_Outptr_result_maybenull_ ID3D11VertexShader **ppShader,
      _COM_Outptr_result_maybenull_ ID3D11InputLayout **pInputLayout);

  // create pixel shader
  HRESULT CreatePixelShader(
      _In_reads_(byteCodeLength) const void *pShaderByteCode,
      _In_ SIZE_T byteCodeLength,
      _COM_Outptr_result_maybenull_ ID3D11PixelShader **ppShader);

  HRESULT CreatePixelShaderFromBlob(
      _In_ ID3DBlob *Blob,
      _COM_Outptr_result_maybenull_ ID3D11PixelShader **ppShader);

  HRESULT CreatePixelShaderFromFile(
      LPCWSTR filename,
      _COM_Outptr_result_maybenull_ ID3D11PixelShader **ppShader);

  inline UINT GetNumBackBuffer() const noexcept { return m_numBackBuffers; };
  const ::Microsoft::WRL::ComPtr<IDXGISwapChain1> &GetSwapChain() const { return m_pSwapChain; };
  const ::Microsoft::WRL::ComPtr<ID3D11Device> &GetDevice() const { return m_pDevice; };
  void GetContext(ID3D11DeviceContext **ppContext) const { m_pDevice->GetImmediateContext(ppContext); };

protected:
private:
  ::Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice{};
  ::Microsoft::WRL::ComPtr<IDXGISwapChain1> m_pSwapChain{};
  ::Microsoft::WRL::ComPtr<IDXGIFactory4> m_pDXGIFactory{};

  D3D_FEATURE_LEVEL m_thisFeatureLevel{};

  UINT m_numBackBuffers{};
};

#endif