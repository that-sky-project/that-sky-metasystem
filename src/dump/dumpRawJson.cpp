#include <cJSON.h>
#include "internal.hpp"

class MetaDataClass: public MetaObject<MetaDataClass> { };

cJSON *MetaDumper::dumpRawJson(
  const MetaSystemDataContainer &container
) {
  cJSON *root = cJSON_CreateObject()
    , *types = cJSON_CreateArray()
    , *classes = cJSON_CreateArray()
    , *consts = cJSON_CreateArray();

  // Dump MetaType.
  for (const auto &it: container.m_metaTypes) {
    const auto &mt = it.second;
    if (mt->AsClass())
      // Only add pure MetaType.
      continue;

    cJSON *typeDecl = cJSON_CreateObject();

    cJSON_AddStringToObject(typeDecl, "name", mt->GetName());
    cJSON_AddNumberToObject(typeDecl, "sizeof", mt->SizeOfType());
    cJSON_AddNumberToObject(typeDecl, "alignof", mt->AlignOfType());
    cJSON_AddBoolToObject(typeDecl, "isNumber", !!mt->IsNumber());
    cJSON_AddBoolToObject(typeDecl, "isString", !!mt->IsString());

    cJSON_AddItemToArray(types, typeDecl);
  }

  // Dump MetaClass.
  for (const auto &it: container.m_metaClasses) {
    const auto &mc = it.second;
    cJSON *classDecl = cJSON_CreateObject();

    cJSON_AddStringToObject(classDecl, "name", mc->GetName());
    cJSON_AddStringToObject(classDecl, "parent", mc->m_parent ? mc->m_parent()->GetName() : "");
    cJSON_AddNumberToObject(classDecl, "sizeof", mc->SizeOfObject());
    cJSON_AddNumberToObject(classDecl, "alignof", mc->AlignOfObject());

    // Dump member variables.
    cJSON *classMembers = cJSON_CreateArray();
    for (const auto &itmv: mc->m_metaDataContainer->m_variables) {
      const auto &mv = itmv.second;
      cJSON *memberDecl = cJSON_CreateObject()
        , *auxValues = cJSON_CreateArray();

      cJSON_AddStringToObject(memberDecl, "name", mv->GetName());
      cJSON_AddStringToObject(memberDecl, "type", mv->GetType()->GetName());
      cJSON_AddStringToObject(memberDecl, "countType", mv->GetCountType()->GetName());
      cJSON_AddNumberToObject(memberDecl, "offset", mv->GetContext().offset);
      cJSON_AddNumberToObject(memberDecl, "countOffset", mv->GetCountContext().offset);
      cJSON_AddNumberToObject(memberDecl, "staticArraySize", mv->GetStaticSize());

      for (MetaDataClass *p = (MetaDataClass *)mv->GetFields(); p; p = p->GetPrev()) {
        cJSON *aux = cJSON_CreateObject();
        cJSON_AddStringToObject(aux, "name", p->GetName());
        cJSON_AddStringToObject(aux, "value", (cstring)p->GetFields());
        cJSON_AddItemToArray(auxValues, aux);
      }
      cJSON_AddItemToObject(memberDecl, "auxValues", auxValues);

      cJSON_AddItemToArray(classMembers, memberDecl);
    }
    cJSON_AddItemToObject(classDecl, "members", classMembers);
    cJSON_AddItemToArray(classes, classDecl);
  }

  // Dump MetaConstant.
  for (const auto &it: container.m_metaConstants) {
    const auto &mc = it.second;
  }

  cJSON_AddItemToObject(root, "types", types);
  cJSON_AddItemToObject(root, "classes", classes);
  cJSON_AddItemToObject(root, "constants", consts);

  return root;
}
