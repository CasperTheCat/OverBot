#pragma once

#define LOG(x) "[INFO] " << x
#define LOGERR(x) "[ERRO] " << x
#define LOGWAR(x) "[WARN] " << x
#define WAS_SUCCESS(x) (x == false)
#define WAS_FAILURE(x) (x == true)

#ifdef _WIN32
#define IS_INVALID_SOCKET(x) (x == INVALID_SOCKET)
#else
#define IS_INVALID_SOCKET(x) (x < 0)
#endif
