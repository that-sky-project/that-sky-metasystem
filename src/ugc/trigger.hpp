#ifndef __UGC_TRIGGER_HPP__
#define __UGC_TRIGGER_HPP__

#include <Base/Meta.hpp>
#include "proto.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Trigger
// ----------------------------------------------------------------------------

class Trigger;
META_DECLARE_CLASS(Trigger);

// Triggers are a special type of event used to perform operations outside the
// game or to make reflective modifications to the game itself.
class Trigger: public Event {
public:
  Trigger() {
    m_metaClassId = MetaClassId(Trigger);
  }
};

// ----------------------------------------------------------------------------
// [SECTION] WinMessageBoxA
// ----------------------------------------------------------------------------

class WinMessageBoxA;
META_DECLARE_CLASS(WinMessageBoxA);

// Show a MessageBox.
class WinMessageBoxA: public Trigger {
public:
  WinMessageBoxA() {
    m_metaClassId = MetaClassId(WinMessageBoxA);
  }

  virtual void OnStart(void *) override {
    MessageBoxA(nullptr, lpText.c_str(), lpCaption.c_str(), uType);
  }

public:
  TgcString lpText = "";
  TgcString lpCaption = "MessageBoxA";
  uint32_t uType = MB_OK;
};

// ----------------------------------------------------------------------------
// [SECTION] FowardTrigger
// ----------------------------------------------------------------------------

class FowardTrigger;
META_DECLARE_CLASS(FowardTrigger);

// FowardTrigger is used to execute a shell command.
class FowardTrigger: public Trigger {
public:
  FowardTrigger() {
    m_metaClassId = MetaClassId(FowardTrigger);
  }

  virtual void OnStart(void *) override {
    system(src.c_str());
  }

public:
  TgcString src = "echo \"NO SRC SPECIFIED\"";
};

#endif
