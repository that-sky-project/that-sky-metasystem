#include "includes/htmod.h"
#include "internal.hpp"

__declspec(dllexport) HTStatus HTMLAPI HTModOnInit(
  void *
) {
  FakeMetaSystem::initialize();

  return HT_SUCCESS;
}
