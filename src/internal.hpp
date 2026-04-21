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
// [SECTION] utils/globals
// ----------------------------------------------------------------------------

extern HMODULE hModuleDll;
extern RealMetaSystem gRealMetaSystem;

#endif
