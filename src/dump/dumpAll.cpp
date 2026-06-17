#include <fstream>
#include "internal.hpp"

void MetaDumper::readVersions() {
  
}

void MetaDumper::dumpAll(
  const MetaSystemDataContainer &container
) {
  // Dump raw json.
  cJSON *rawJson = MetaDumper::dumpRawJson(container);
  if (rawJson) {
    cstring s = cJSON_PrintUnformatted(rawJson);
    if (s) {
      std::ofstream ofs("meta_dump_raw@0.33.7+394009.json");
      ofs << s;
      ofs.close();
    }
  }

  // Dump declaration group.
  cJSON *declJson = MetaDumper::dumpDeclGroup(container);
  if (declJson) {
    cstring s = cJSON_PrintUnformatted(declJson);
    if (s) {
      std::ofstream ofs("meta_dump_decl@0.33.7+394009.json");
      ofs << s;
      ofs.close();
    }
  }

  // Crash the game manually when the dump is finished.
  MessageBoxA(nullptr, "Dump finished!", "", MB_OK);
  __debugbreak();
}
