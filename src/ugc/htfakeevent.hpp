#ifndef __UGC_HTFAKEEVENT_HPP__
#define __UGC_HTFAKEEVENT_HPP__

#include <Base/Meta.hpp>
#include "proto.hpp"

class HTFakeEvent;
META_DECLARE_CLASS(HTFakeEvent);

class HTFakeEvent: public Event {
public:
  HTFakeEvent() {
    m_metaClassId = MetaClassId(HTFakeEvent);
  }

  virtual void OnStart(void *) override {
    HTTellText("aaa");
  }

  virtual void Update(void *) override {
    HTTellText("update...");
  }

  void TestFunc() {
    HTTellText("TestFunc lol");
    HTTellText("  context1 = %d", context1);
    HTTellText("  context2 = %d", context2);
  }

  void TestFunc2(int a, cstring b) {
    HTTellText("TestFunc2 with params: a = %d, b = \"%s\"", a, b);
  }

public:
  bool context1 = false;
  int context2 = 0;
  int array1[10] = {520};
  int array1_count = 1;
};

#endif

