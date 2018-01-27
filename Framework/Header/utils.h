#pragma once

#define LOG(x) "[INFO] " << x
#define LOGERR(x) "[ERRO] " << x
#define LOGWAR(x) "[WARN] " << x
#define WAS_SUCCESS(x) (x == 0)
#define WAS_FAILURE(x) (x != 0)

#ifdef _WIN32
#define IS_INVALID_SOCKET(x) (x == INVALID_SOCKET)
#else
#define IS_INVALID_SOCKET(x) (x < 0)
#endif
