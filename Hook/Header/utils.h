#pragma once

#define LOG(x) "[INFO] " << x
#define LOGERR(x) "[ERRO] " << x
#define LOGWAR(x) "[WARN] " << x

#define FAILED(x) (x == false)
#define SUCCEEDED(x) (x == true)

#define WAS_SUCCESS(x) SUCCEEDED(x)
#define WAS_FAILURE(x) FAILED(x)

bool IsWow64();