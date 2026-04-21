#include <Windows.h>
#include "internal.hpp"

HMODULE hModuleDll = nullptr;

BOOL APIENTRY DllMain(
  HMODULE hModule,
  DWORD dwReason,
  LPVOID lpReserved
) {
  if (dwReason == DLL_PROCESS_ATTACH)
    hModuleDll = hModule;
  return TRUE;
}
