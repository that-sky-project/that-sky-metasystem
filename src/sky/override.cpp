#include <errno.h>
#include "internal.hpp"
#include "proto.hpp"

class HTFakeEvent;
META_DECLARE_CLASS(HTFakeEvent);

class HTFakeEvent: public Event {
public:
  HTFakeEvent() {
    m_metaClassId = MetaClassImpl<HTFakeEvent>::Must_call_META_REGISTER_CLASS()->m_globalId;
  }

  virtual void OnStart(void *) override {
    HTTellText("aaa");
  }

  virtual void Update(void *) override {
    HTTellText("update...");
  }

  void TestFunc() {
    HTTellText("TestFunc lol");
  }
};

META_REGISTER_CLASS(HTFakeEvent)
META_REGISTER_FUNCTION_MEMBER(HTFakeEvent, TestFunc)


typedef void (__fastcall *PFN_Game_Alloc)(
  Game *);
typedef const MetaClass *(__fastcall *PFN_GetMetaClassById)(
  u32);
typedef const MetaClass *(__fastcall *PFN_GetMetaClassByName)(
  cstring, bool);

FakeMetaSystem *gMetaSystem = nullptr;
ProxyMetaSystem *gProxyMetaSystem = nullptr;
Game *gGame = nullptr;

static const HTAsmSig sigE8_Game_Alloc{
  "E8 ?  ?  ?  ?  C7 87 ?  ?  ?  ?  ?  ?  ?  ?  48 "
  "89 F9 E8 ?  ?  ?  ?  48 8B 06 ",
  HT_SCAN_E8,
  0x12
};

static const HTAsmSig sigE8_GetMetaClassById{
  "4C 89 8C 24 ?  ?  ?  ?  41 8B 08 E8 ?  ?  ?  ?  "
  "48 8B 10 48 89 C1 ",
  HT_SCAN_E8,
  0x0B
};

static const HTAsmSig sigE8_GetMetaClassByName{
  "48 89 D9 31 D2 E8 ?  ?  ?  ?  48 89 C3 E8 ",
  HT_SCAN_E8,
  0x05
};

static HTAsmFunction sfn_Game_Alloc{
  "Game::Alloc()",
  nullptr,
  nullptr,
  nullptr
};

static HTAsmFunction sfn_GetMetaClassById{
  "GetMetaClassById()",
  nullptr,
  nullptr,
  nullptr
};

static HTAsmFunction sfn_GetMetaClassByName{
  "GetMetaClassByName()",
  nullptr,
  nullptr,
  nullptr
};

class HTMonkeyG_Class {
  int aaa;
};

META_REGISTER_CLASS(HTMonkeyG_Class);

MetaClass *gEventClass = nullptr;

static void hook_Game_Alloc(
  Game *self
) {
  const auto &classes = gMetaSystem->data->m_metaClasses;

  // Resolve sizeof(MetaSystem).
  const auto &itMetaSystem = classes.find("MetaSystem");
  if (itMetaSystem == classes.end())
    return ((PFN_Game_Alloc)sfn_Game_Alloc.origin)(self);

  const MetaClass *pmcMetaSystem = itMetaSystem->second;
  size_t sizeMetaSystem = pmcMetaSystem->SizeOfObject();
  u32 classCount = (sizeMetaSystem - sizeof(FakeMetaSystem)) / sizeof(const MetaClass *);

  // Resolve Game.metaSystem.
  const auto &itGame = classes.find("Game");
  if (itGame == classes.end())
    return ((PFN_Game_Alloc)sfn_Game_Alloc.origin)(self);

  const MetaClass *pmcGame = itGame->second;
  const auto &variables = pmcGame->m_metaDataContainer->m_variables;
  const auto &itVars = variables.find("metaSystem");
  if (itVars == variables.end())
    return ((PFN_Game_Alloc)sfn_Game_Alloc.origin)(self);

  // Override Game.metaSystem.
  const MetaMemberVariable *pmmvMetaSystem = itVars->second;
  ProxyMetaSystem **ppGameMetaSystem = (ProxyMetaSystem **)((char *)self + pmmvMetaSystem->GetContext().offset);

  // Create ProxyMetaSystem from MetaSystem.
  gProxyMetaSystem = ProxyMetaSystem::create();
  gProxyMetaSystem->set(
    reinterpret_cast<const MetaSystem *>(gMetaSystem),
    classCount);
  HTTellText(
    "§a[ThatSkyMetaSystem] MetaSystem overriden: %p -> %p",
    *ppGameMetaSystem,
    gProxyMetaSystem);
  HTTellText(
    "§e[ThatSkyMetaSystem] Copied %u classes of %llu classes",
    gProxyMetaSystem->m_data->m_count,
    gProxyMetaSystem->m_data->m_metaClasses.size());

  const MetaSystem *old = reinterpret_cast<const MetaSystem *>(*ppGameMetaSystem);

  HTTellText("§a[ThatSkyMetaSystem] Destroying previous.....");
  // Call destructor of MetaStrMap.
  delete old->m_data;
  // Directly free the memory.
  operator delete((void *)old);

  *ppGameMetaSystem = gProxyMetaSystem;

  {
    // - Try to register a new event.
    const auto &itEvent = gProxyMetaSystem->m_data->m_metaClasses.find("Event");
    static auto eventClass = itEvent->second;
    auto pmcEvent = MetaClassImpl<HTFakeEvent>::Must_call_META_REGISTER_CLASS();
    pmcEvent->m_parent = []() -> MetaClass * {
      return eventClass;
    };
    pmcEvent->m_topoOrder = 9000;
    pmcEvent->m_baseTopoIdList = eventClass->m_baseTopoIdList;
    pmcEvent->m_baseTopoIdList.push_back(eventClass->m_topoOrder);
    if (!gProxyMetaSystem->addClass(pmcEvent))
      __debugbreak();

    // - Add member function.
    char *name = new char[strlen(g_metaMemberFunction_HTFakeEvent_TestFunc.GetName()) + 1];
    strcpy(name, g_metaMemberFunction_HTFakeEvent_TestFunc.GetName());
    HTTellText("%s", name);
    g_metaMemberFunction_HTFakeEvent_TestFunc.Initialize();
    pmcEvent->GetActive()->AsClass()->m_metaDataContainer->m_functions[name]
      = &g_metaMemberFunction_HTFakeEvent_TestFunc;
  }

  ((PFN_Game_Alloc)sfn_Game_Alloc.origin)(self);
}

static const MetaClass *hook_GetMetaClassById(
  u32 id
) {
  if (!gProxyMetaSystem)
    return ((PFN_GetMetaClassById)sfn_GetMetaClassById.origin)(id);

  if (id > gProxyMetaSystem->m_data->m_maxClasses)
    return nullptr;

  return gProxyMetaSystem->m_classes[id];
}

static const MetaClass *hook_GetMetaClassByName(
  cstring name,
  bool isConstString
) {
  if (!gProxyMetaSystem)
    return ((PFN_GetMetaClassByName)sfn_GetMetaClassByName.origin)(name, isConstString);

  if (!name)
    return nullptr;

  const auto &classes = gProxyMetaSystem->m_data->m_metaClasses;
  const auto &it = classes.find(name);

  if (it == classes.end())
    return nullptr;

  return it->second;
}

void MetaSystemOverride::initialize() {
  sfn_Game_Alloc.detour = (void *)hook_Game_Alloc;
  HTModLoader::createHookAndEnable(
    &sigE8_Game_Alloc,
    &sfn_Game_Alloc);

  sfn_GetMetaClassById.detour = (void *)hook_GetMetaClassById;
  HTModLoader::createHookAndEnable(
    &sigE8_GetMetaClassById,
    &sfn_GetMetaClassById);

  sfn_GetMetaClassByName.detour = (void *)hook_GetMetaClassByName;
  HTModLoader::createHookAndEnable(
    &sigE8_GetMetaClassByName,
    &sfn_GetMetaClassByName);
}
