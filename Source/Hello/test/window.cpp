#pragma once

#include "pch.hpp"

#define CASE(message, action) \
  case message:               \
    action;                   \
    break

using namespace Window;

using CursorArgs::Event::DoubleClick;
using CursorArgs::Event::Down;
using CursorArgs::Event::LDoubClick;
using CursorArgs::Event::LDown;
using CursorArgs::Event::Left;
using CursorArgs::Event::LUp;
using CursorArgs::Event::MDoubClick;
using CursorArgs::Event::MDown;
using CursorArgs::Event::Middle;
using CursorArgs::Event::Move;
using CursorArgs::Event::MUp;
using CursorArgs::Event::None;
using CursorArgs::Event::RDoubClick;
using CursorArgs::Event::RDown;
using CursorArgs::Event::Right;
using CursorArgs::Event::RUp;
using CursorArgs::Event::Up;

const std::wstring translateCursorArgsMisc(enum CursorArgs::Misc In)
{

  using CursorArgs::Misc::CtrlDown;
  using CursorArgs::Misc::LBDown;
  using CursorArgs::Misc::MBDown;
  using CursorArgs::Misc::None;
  using CursorArgs::Misc::RBDown;
  using CursorArgs::Misc::ShiftDown;
  using CursorArgs::Misc::X1Down;
  using CursorArgs::Misc::X2Down;
  switch (In)
  {

    CASE(None, return L"None";);
    CASE(CtrlDown, return L"CtrlDown";);
    CASE(LBDown, return L"LBDown";);
    CASE(MBDown, return L"MBDown";);
    CASE(RBDown, return L"RBDown";);
    CASE(ShiftDown, return L"ShiftDown";);
    CASE(X1Down, return L"X1Down";);
    CASE(X2Down, return L"X2Down";);
  default:
    return L"CursorArgsMisc:undefined";
  };
};

const std::wstring translateCursorArgsEvent(enum CursorArgs::Event In)
{
  switch (In)
  {
    CASE(LUp, return L"LUp";);
    CASE(LDown, return L"LDown";);
    CASE(LDoubClick, return L"LDoubClick";);
    CASE(MUp, return L"MUp";);
    CASE(MDown, return L"MDown";);
    CASE(MDoubClick, return L"MDoubClick";);
    CASE(RUp, return L"RUp";);
    CASE(RDown, return L"RDown";);
    CASE(RDoubClick, return L"RDoubClick";);
  default:
    return L"CursorArg:undefined";
  };
};

class Test : public CoreApp
{

public:
  static int AppEntry(HINSTANCE hinst)
  {
    DBG_ONLY(_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF));

    Window::Run(Window::CoreWindow<Test>{hinst, {800, 50, 1700, 1000}});
    MessageBeep(5);
    return 0;
  };

  void OnWindowActivate(_In_ const ::Window::ActivateArgs &args) noexcept
  {

    if (CoreApp::m_IsVisible != args.IsMinimized)
    {
      CoreApp::m_IsVisible = args.IsMinimized;
    };
    Log<Console>::Write(L"OnWindowActivate");
  };

  void OnKeyStroke(_In_ const ::Window::KeyEventArgs &args) noexcept
  {
    switch (args.VirtualKey)
    {
      CASE(VK_ESCAPE, { CoreApp::Close(); });
      CASE(VK_SPACE, {});
    }
    Log<Console>::Write(L"OnKeyStroke");
  };

  void OnCreate(_In_ const ::Window::CreationArgs &args) noexcept
  {
    SIZE RTSize{RECTWIDTH(args.Rect), RECTHEIGHT(args.Rect)};
    Log<Console>::Write(L"OnCreate");
  };

  void OnSizeChanged(_In_ const ::Window::SizeChangedArgs &args) noexcept { Log<Console>::Write(L"OnSizeChanged"); };
  void OnCursorMove(_In_ const ::Window::CursorArgs &args) noexcept
  {

    Log<Console>::Write(L"OnCursorMove", L"val:", static_cast<UINT>(args.Event),
                        L"Misc:", translateCursorArgsMisc(args.Misc), L"Misc val:", static_cast<WPARAM>(args.Misc));
  };
  void OnCursorEvent(_In_ const ::Window::CursorArgs &args) noexcept
  {
    Log<Console>::Write(L"OnCursorEvent Event:", translateCursorArgsEvent(args.Event), L"Event val:", static_cast<UINT>(args.Event),
                        L"cursor pos:", args.pos.x, args.pos.y,
                        L"Misc:", translateCursorArgsMisc(args.Misc), L"Misc val:", static_cast<WPARAM>(args.Misc));
  };

  void Draw() noexcept
  {
    Log<Console>::Write(L"Draw");
  };

  void OnClose() noexcept
  {
    Log<Console>::Write(L"OnClose");
  };

private:
};

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
int wWinMain(_In_ HINSTANCE hinst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) { return Invoke(&Test::AppEntry, hinst); };
int main() { return Invoke(&Test::AppEntry, (HINSTANCE)&__ImageBase); };