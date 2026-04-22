#ifndef __INTERNAL_HPP__
#define __INTERNAL_HPP__

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <sstream>
#include "includes/htmodloader.h"
#include "Meta.hpp"

// ----------------------------------------------------------------------------
// [SECTION] utils/typeAliases
// ----------------------------------------------------------------------------

#pragma warning(disable: 4200)

#define i08 char
#define u08 unsigned char
#define i16 short
#define u16 unsigned short
#define i32 int
#define u32 unsigned int
#define i64 long long
#define u64 unsigned long long
#define f32 float
#define f64 double
#define nil void

using TgcString = std::string;
using TgcWString = std::wstring;
using cstring = const char *;

typedef void *Game;

// ----------------------------------------------------------------------------
// [SECTION] utils/realMetaSystem
// ----------------------------------------------------------------------------

class FakeMetaSystem;
class RealMetaSystem;

class RealMetaSystem {
public:
  RealMetaSystem() = default;
  ~RealMetaSystem() = default;

  void initialize(
    const FakeMetaSystem *original);

  void addClass(
    const MetaClass *clazz);

  void addChild(
    const MetaClass *parent,
    const MetaClass *child);

  void dumpTree(
    std::ostringstream &s
  ) const;
  
private:
  void recursiveChild(
    std::ostringstream &s,
    TgcString className,
    std::unordered_map<TgcString, bool> &visitState
  ) const;

  MetaSystemDataContainer container;

  std::map<TgcString, const MetaClass *> registeredClasses;
  std::map<TgcString, std::vector<TgcString>> registeredTree;
};

// ----------------------------------------------------------------------------
// [SECTION] utils/fakeMetaSystem
// ----------------------------------------------------------------------------

class FakeMetaSystem {
public:
  static void initialize();

  FakeMetaSystem() = default;
  FakeMetaSystem(
    const MetaSystem *original);

  ~FakeMetaSystem() = default;

  u32 metaClassId;
  MetaSystemDataContainer *data;
};

// ----------------------------------------------------------------------------
// [SECTION] utils/proxyMetaSystem
// ----------------------------------------------------------------------------

class ProxyMetaSystemDataContainer: public MetaSystemDataContainer {
public:
  ProxyMetaSystemDataContainer() = default;
  ~ProxyMetaSystemDataContainer() = default;

  ProxyMetaSystemDataContainer &operator=(const MetaSystemDataContainer &);

  u32 m_count;
  u32 m_maxClasses;
};

class ProxyMetaSystem {
public:
  static ProxyMetaSystem *create(
    u32 maxClasses = 0x100000);

  ProxyMetaSystem() = default;
  ~ProxyMetaSystem() = default;

  bool addClass(
    MetaClass *clazz);
  
  void set(
    const MetaSystem *p,
    u32 count);

  u32 m_metaClassId;
  ProxyMetaSystemDataContainer *m_data;
  const MetaClass *m_classes[];
};

namespace MetaSystemOverride {
void initialize();
}

// ----------------------------------------------------------------------------
// [SECTION] utils/globals
// ----------------------------------------------------------------------------

extern HMODULE hModuleDll;
extern RealMetaSystem gRealMetaSystem;
extern FakeMetaSystem *gMetaSystem;
extern ProxyMetaSystem *gProxyMetaSystem;
extern Game *gGame;

namespace HTModLoader {
HTStatus createHookAndEnable(
  const HTAsmSig *sigXX,
  HTAsmFunction *sfn);
}

void *operator new(std::size_t _Size);
void *operator new[](std::size_t _Size);
void operator delete(void *_Block) noexcept;
void operator delete[](void *_Block) noexcept;

#endif
