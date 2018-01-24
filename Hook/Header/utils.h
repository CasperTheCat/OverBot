#pragma once

#define LOG(x) "[INFO] " << x
#define LOGERR(x) "[ERRO] " << x
#define LOGWAR(x) "[WARN] " << x
#define WAS_SUCCESS(x) (x == false)
#define WAS_FAILURE(x) (x == true)

bool IsWow64();