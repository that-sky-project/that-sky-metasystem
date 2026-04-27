#ifndef __PROTO_HPP__
#define __PROTO_HPP__

#include "internal.hpp"

class Event: public Object {
public:
  Event()
    : unk_1(0)
    , unk_2()
    , unk_3(-1)
    , unk_4(-1)
    , unk_5(0)
  { }

  virtual ~Event() = default;

  virtual void Unk1() { }
  virtual void Unk2() { }
  virtual void Unk3() { }
  virtual void Unk4() { }
  virtual void OnStart(void *pGame) { }
  virtual void OnRestart(void *pGame) { }
  virtual void OnStop(void *pGame) { }
  virtual void Update(void *pGame) { }
  virtual void Unk9() { }
  virtual void Unk10() { }
  virtual void Unk11() { }
  virtual void Unk12() { }
  virtual u32 GetMaxTypeIdx() { return 0xFFFFFFFE; }
  virtual void Unk13() { }
  virtual void Unk14() { }

  u08 unk_1;
  u64 unk_2[2];
  i08 unk_3;
  i32 unk_4;
  u16 unk_5;
};

META_DECLARE_CLASS(Event);

#endif
