#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <compare>
#include <mutex>

// decl
#include "Wrapper.hpp"
#include "Writer.hpp"
#include "Exception.hpp"
#include "Timer.hpp"

using ::Exception::Invoke;
using ::Writer::Error;
using ::Writer::Log;
using ::Writer::Warning;
using ::Writer::Out::Console;
using ::Writer::Out::File;

// impl
#include "Wrapper.inl"
#include "CoreWindow.hpp"
#include "Writer.inl"
#include "Exception.inl"
#include "Timer.inl"
