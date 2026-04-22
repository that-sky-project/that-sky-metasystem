#include "internal.hpp"

void *operator new(std::size_t _Size) {
  if (!_Size)
    _Size = 1;
  return _aligned_malloc(_Size, 0x10);
}

void *operator new[](std::size_t _Size) {
  return operator new(_Size);
}

void operator delete(void *_Block) noexcept {
  return _aligned_free(_Block);
}

void operator delete[](void *_Block) noexcept {
  return operator delete(_Block);
}
