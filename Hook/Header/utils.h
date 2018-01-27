#pragma once

#define LOG(x) "[INFO] " << x
#define LOGERR(x) "[ERRO] " << x
#define LOGWAR(x) "[WARN] " << x
#define WAS_SUCCESS(x) (x == 0)
#define WAS_FAILURE(x) (x != 0)

bool IsWow64();