#include <Windows.h>
#include <unordered_map>
#include "internal.hpp"

class MetaMustCallRegister {
public:
  void initialize(
    const MetaType *clazz);

private:
  struct MovAbs {
    // 48 A1
    u08 mov_rax_qwordptr[2];
    u08 moffs64[8];
    u08 ret[1];
    u08 gap[5];
  };
  struct MovRel {
    // 48 8B 05
    u08 mov_rax_qwordptr[3];
    u08 rel32[4];
    u08 ret[1];
    u32 gap[1];
    u32 fallback[1];
  };

  MovAbs abs;
};

class MetaMustCallRegisterBarn {
public:
  void initialize();
  bool alloc(const MetaType *clazz);
  bool free(const MetaType *clazz);
  PFN_RegisterClass get(cstring name);
  PFN_RegisterClass get(const MetaType *clazz);

private:
  // Size of each memory block (page size of VirtualAlloc).
  static constexpr int kMemoryBlockSize = 0x1000;
  static constexpr int kMemorySlotSize = 0x10;

  union MemorySlot {
    MemorySlot *pNext;
    MetaMustCallRegister regFn;
  };

  struct MemoryBlock {
    MemoryBlock *pNext;
    MemorySlot *pFree;
    u32 refCount;
  };

  MemoryBlock *getBlock();

  MemoryBlock *pMemoryBlocks;
  MetaStrHashMap<void *> fn;
};

void MetaMustCallRegister::initialize(
  const MetaType *clazz
) {
  /*
  rel.mov_rax_qwordptr[0] = 0x48;
  rel.mov_rax_qwordptr[1] = 0x8B;
  rel.mov_rax_qwordptr[2] = 0x05;
  
  // 48:8B05 [rel32]
  i64 offset = (i64)((const char *)&clazz->m_self - (const char *)this + 7);
  i32 relAddr;
  if (offset < 0x7FFFFFFFull && offset > -0x80000000ull) {
    // 32-bit relative address.
    relAddr = (i32)offset;
  } else {
    // Out of range. Fall back to nullptr.
    relAddr = 1 + 4;
  }
  memcpy(rel.rel32, &relAddr, sizeof(i32));

  rel.ret[0] = 0xC3;
  rel.gap[0] = 0xCCCCCCCC;
  rel.fallback[0] = 0x00000000;
  */
  abs.mov_rax_qwordptr[0] = 0x48;
  abs.mov_rax_qwordptr[1] = 0xA1;

  u64 addr = (u64)&clazz->m_self;
  memcpy(abs.moffs64, &addr, sizeof(u64));

  abs.ret[0] = 0xC3;

  memset(abs.gap, 0xCC, sizeof(abs.gap));
}

void MetaMustCallRegisterBarn::initialize() {
  pMemoryBlocks = getBlock();
}

bool MetaMustCallRegisterBarn::alloc(
  const MetaType *clazz
) {
  MemorySlot *pSlot;
  MemoryBlock *pBlock = getBlock();

  if (pBlock == nullptr)
    return false;

  // Remove an unused slot from the list.
  pSlot = pBlock->pFree;
  pBlock->pFree = pSlot->pNext;

  pSlot->regFn.initialize(clazz);
  fn[clazz->m_name] = pSlot;

  return true;
}

PFN_RegisterClass MetaMustCallRegisterBarn::get(
  cstring name
) {
  const auto &it = fn.find(name);
  if (it == fn.end())
    return nullptr;

  return (PFN_RegisterClass)it->second;
}

MetaMustCallRegisterBarn::MemoryBlock *MetaMustCallRegisterBarn::getBlock() {
  MemoryBlock *pBlock;
  for (pBlock = pMemoryBlocks; pBlock; pBlock = pBlock->pNext) {
    if (pBlock->pFree)
      return pBlock;
  }

  pBlock = (MemoryBlock *)VirtualAlloc(
    nullptr,
    kMemoryBlockSize,
    MEM_COMMIT | MEM_RESERVE,
    PAGE_EXECUTE_READWRITE);

  if (pBlock) {
    // Build a chain list of all the slots.
    MemorySlot *pSlot = (MemorySlot *)pBlock + 2;
    pBlock->pFree = nullptr;
    do {
      pSlot->pNext = pBlock->pFree;
      pBlock->pFree = pSlot;
      pSlot++;
    } while ((u64)pSlot - (u64)pBlock <= kMemoryBlockSize - kMemorySlotSize);

    pBlock->pNext = pMemoryBlocks;
    pMemoryBlocks = pBlock;
  }

  return pBlock;
}

MetaMustCallRegisterBarn test;

struct FakeMetaClass: public MetaObject<FakeMetaClass> {
  FakeMetaClass(cstring name): MetaObject<FakeMetaClass>(name), m_self(this) { }
  virtual ~FakeMetaClass() = default;

  void *unk_1;
  FakeMetaClass *m_self;
};

FakeMetaClass test111{"aaabbb"};

int main() {
  test.initialize();
  test.alloc((const MetaType *)&test111);
  __debugbreak();
  PFN_RegisterClass regFn = test.get("aaabbb");
  printf("%p %p %p\n", regFn, regFn(), test111.m_self);
}
