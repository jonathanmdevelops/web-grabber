#pragma once
#pragma comment(lib, "winhttp.lib")
#include <iostream>
#include <string>
#include <Windows.h>
#include <winhttp.h>

#include "tchar.h"

#if defined(UNICODE) || defined(_UNICODE)
#define cout wcout
#define cerr wcerr
#define string wstring
#endif