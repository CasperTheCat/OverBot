#pragma once

#define LOG(x) "[INFO] " << x
#define LOGERR(x) "[ERRO] " << x
#define LOGWAR(x) "[WARN] " << x
#define WAS_SUCCESS(x) (x == 0)
#define WAS_FAILURE(x) (x != 0)

// Windows Utils
#ifdef _WIN32
#define IS_INVALID_SOCKET(x) (x == INVALID_SOCKET)

// Linux Utils
#else
#define IS_INVALID_SOCKET(x) (x < 0)
#include <cstring>
#define SecureZeroMemory(ptr,size) memset(ptr, 0, size);
#endif
