#include "includes/htmod.h"
#include "internal.hpp"

__declspec(dllexport) HTStatus HTMLAPI HTModOnInit(
  void *
) {
  FakeMetaSystem::initialize();
  MetaSystemOverride::initialize();

  return HT_SUCCESS;
}

namespace HTModLoader {

HTStatus createHookAndEnable(
  const HTAsmSig *sigXX,
  HTAsmFunction *sfn
) {
  if (sigXX && !HTSigScanFunc(sigXX, sfn))
    return HT_FAIL;

  if (!HTAsmHookCreate(hModuleDll, sfn))
    return HT_FAIL;

  return HTAsmHookEnable(hModuleDll, sfn->fn);
}

}
