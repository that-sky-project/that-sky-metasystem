#include <fstream>
#include "internal.hpp"

typedef void (__fastcall *PFN_MetaSystem_Initialize)(
  FakeMetaSystem *);

static const HTAsmSig sigE8_MetaSystem_Initialize{
  "31 D2 E8 ?  ?  ?  ?  48 89 D9 E8 ?  ?  ?  ?  B9 "
  "?  ?  ?  ?  E8",
  HT_SCAN_E8,
  0x0A
};

static HTAsmFunction sfn_MetaSystem_Initialize{
  "MetaSystem::Initialize()",
  nullptr,
  nullptr,
  nullptr
};

static void hook_MetaSystem_Initialize(
  FakeMetaSystem *self
) {
  ((PFN_MetaSystem_Initialize)sfn_MetaSystem_Initialize.origin)(self);

  gMetaSystem = self;
  gRealMetaSystem.initialize(self);

  return;
  std::ostringstream oss;
  gRealMetaSystem.dumpTree(oss);

  std::ofstream ofs("exports.txt");
  ofs << oss.str();
  ofs.close();
}

void FakeMetaSystem::initialize() {
  HTSigScanFunc(&sigE8_MetaSystem_Initialize, &sfn_MetaSystem_Initialize);
  sfn_MetaSystem_Initialize.detour = (void *)hook_MetaSystem_Initialize;
  HTAsmHookCreate(hModuleDll, &sfn_MetaSystem_Initialize);
  HTAsmHookEnable(hModuleDll, sfn_MetaSystem_Initialize.fn);
}

FakeMetaSystem::FakeMetaSystem(
  const MetaSystem *original
)
  : metaClassId(original->m_metaClassId)
  , data(original->m_data)
{ }
