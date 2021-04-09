#pragma once

#include <atlstr.h>
#include <stdexcept>

#define THROW_WSTRING_RUNTIME_ERROR(x)				\
do {												\
		throw std::runtime_error(CW2A((x).c_str()));\
} while(0)