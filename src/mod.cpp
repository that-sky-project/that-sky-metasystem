#include "includes/htmod.h"
#include "internal.hpp"

RealMetaSystem gRealMetaSystem;
u32 gEngineVersion[3] = {0};
u32 gResourceVersion = 0;

__declspec(dllexport) HTStatus HTMLAPI HTModOnInit(
  void *
) {
  FakeMetaSystem::initialize();

  return HT_SUCCESS;
}
