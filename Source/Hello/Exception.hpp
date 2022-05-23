#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#define Beep (MessageBeep(5))

#if defined(_DEBUG) || defined(PROFILE)

#define H_ERR(hr, Note) (Exception::FormatError(Exception::ToFile(L"" __FILE__), __LINE__, Exception::ToFunc(L"" #hr), hr, Note))
#define H_CHECK(hr, Note) (Exception::Check(Exception::ToFile(L"" __FILE__), __LINE__, Exception::ToFunc(L"" #hr), hr, Note))
#define H_OK(hr) ((H_CHECK(hr, L"")))
#define H_FAIL(hr) (!(H_CHECK(hr, L"")))
#define W32(expr)      \
    ::SetLastError(0); \
    expr;              \
    H_CHECK(HRESULT_FROM_WIN32(::GetLastError()), L"W32")
#define RLS_ONLY(expr)
#define DBG_ONLY(expr) expr
#define W32_ERR(Error, Note) H_CHECK(HRESULT_FROM_WIN32(Error), Note)
#else
#define H_ERR(hr, Note) hr         //
#define H_CHECK(hr, Note) (hr == S_OK)    // true == ok
#define H_OK(hr) (hr == S_OK)      //  formats hr errors (if any) in debug\profile mode
#define H_FAIL(hr) (hr != S_OK)    //
#define W32(expr) expr             // check winapi err
#define RLS_ONLY(expr) expr        //    expr is  part of   release build  only ( )
#define DBG_ONLY(expr) 0           //    expr is not a part of   release build
#define W32_ERR(Error, Note) Error // todo :do something

#endif //  defined(_DEBUG) || defined(PROFILE)

namespace Exception
{
    template <typename PF, typename... Args>
    auto Invoke(PF pfunction, Args... args);

    inline HRESULT FormatError(std::wstring_view File, uint32_t uLine, std::wstring_view Func, HRESULT ErrorCode, std::wstring_view Note);
    inline bool Check(std::wstring_view File, uint32_t uLine, std::wstring_view Func, HRESULT ErrorCode, std::wstring_view Note)
    {
        return (ErrorCode == S_OK) ? true : (S_OK == FormatError(File, uLine, Func, ErrorCode, Note));
    };
    constexpr std::wstring_view ToFunc(std::wstring_view FunctionName);
    constexpr std::wstring_view ToFile(std::wstring_view FileName);

};

#endif
