#include <unordered_set>
#include <map>
#include <set>
#include <cJSON.h>
#include "internal.hpp"

class MetaDataClass: public MetaObject<MetaDataClass> { };

struct DumpedClass {
  LPCMetaClass parent = nullptr;
  std::map<TgcString, const MetaMemberVariable *> members = {};
};

struct DumpedEnum {
  TgcString as = "";
  // Sort by value first, then by alphabetical order.
  std::map<i32, std::set<TgcString>> values = {};
};

static const std::unordered_set<TgcString> sExclude = {
  // Primitive types.
  "void",
  "bool",
  "int8_t", "uint8_t",
  "int16_t", "uint16_t",
  "int32_t", "uint32_t",
  "int64_t", "uint64_t",
  "float", "double",
  "cstring", "TgcString",
  // Pre-defined structs.
  "Vector2", "Vector3", "Vector4", "Quat",
  "Matrix3", "Matrix4",
  "Color",
  "TgcUuid",
  "NetAddress",
  // Internal objects.
  "Object", "Clump"
};

static void predefine(
  cJSON *root
) {
  cJSON *decl;
  // Create Vector2.
  decl = cJSON_CreateObject();
  cJSON_AddNumberToObject(decl, "$align", 8);
  cJSON_AddStringToObject(decl, "x", "float");
  cJSON_AddStringToObject(decl, "y", "float");
  cJSON_AddItemToObject(root, "S$Vector2", decl);

  // Create Vector3.
  decl = cJSON_CreateObject();
  cJSON_AddNumberToObject(decl, "$align", 16);
  cJSON_AddStringToObject(decl, "x", "float");
  cJSON_AddStringToObject(decl, "y", "float");
  cJSON_AddStringToObject(decl, "z", "float");
  cJSON_AddItemToObject(root, "S$Vector3", decl);

  // Create Vector4.
  decl = cJSON_CreateObject();
  cJSON_AddNumberToObject(decl, "$align", 16);
  cJSON_AddStringToObject(decl, "x", "float");
  cJSON_AddStringToObject(decl, "y", "float");
  cJSON_AddStringToObject(decl, "z", "float");
  cJSON_AddStringToObject(decl, "w", "float");
  cJSON_AddItemToObject(root, "S$Vector4", decl);

  // Create Quat.
  decl = cJSON_CreateObject();
  cJSON_AddNumberToObject(decl, "$align", 16);
  cJSON_AddStringToObject(decl, "i", "float");
  cJSON_AddStringToObject(decl, "j", "float");
  cJSON_AddStringToObject(decl, "k", "float");
  cJSON_AddStringToObject(decl, "w", "float");
  cJSON_AddItemToObject(root, "S$Quat", decl);

  // Create Matrix3.
  decl = cJSON_CreateObject();
  cJSON_AddNumberToObject(decl, "$align", 16);
  cJSON_AddStringToObject(decl, "row1", "Vector3");
  cJSON_AddStringToObject(decl, "row2", "Vector3");
  cJSON_AddStringToObject(decl, "row3", "Vector3");
  cJSON_AddItemToObject(root, "S$Matrix3", decl);

  // Create Matrix4.
  decl = cJSON_CreateObject();
  cJSON_AddNumberToObject(decl, "$align", 16);
  cJSON_AddStringToObject(decl, "row1", "Vector4");
  cJSON_AddStringToObject(decl, "row2", "Vector4");
  cJSON_AddStringToObject(decl, "row3", "Vector4");
  cJSON_AddStringToObject(decl, "row4", "Vector4");
  cJSON_AddItemToObject(root, "S$Matrix4", decl);

  // Create Color.
  decl = cJSON_CreateObject();
  cJSON_AddNumberToObject(decl, "$align", 16);
  cJSON_AddStringToObject(decl, "r", "float");
  cJSON_AddStringToObject(decl, "g", "float");
  cJSON_AddStringToObject(decl, "b", "float");
  cJSON_AddStringToObject(decl, "a", "float");
  cJSON_AddItemToObject(root, "S$Color", decl);

  // Create TgcUuid.
  decl = cJSON_CreateObject();
  cJSON_AddNumberToObject(decl, "$align", 1);
  cJSON_AddStringToObject(decl, "data", "uint8_t[16]");
  cJSON_AddItemToObject(root, "S$TgcUuid", decl);

  // Create NetAddress.
  decl = cJSON_CreateObject();
  cJSON_AddNumberToObject(decl, "$align", 4);
  cJSON_AddStringToObject(decl, "data", "uint32_t[4]");
  cJSON_AddStringToObject(decl, "port", "uint16_t");
  cJSON_AddStringToObject(decl, "type", "uint16_t");
  cJSON_AddItemToObject(root, "S$NetAddress", decl);
}

cJSON *MetaDumper::dumpDeclGroup(
  const MetaSystemDataContainer &container
) {
  cJSON *root = cJSON_CreateObject();
  if (!root)
    return nullptr;
  
  // Build class table.
  std::map<TgcString, DumpedClass> classMap;
  for (const auto &itmc: container.m_metaClasses) {
    auto *mc = itmc.second;
    auto &decl = classMap[itmc.first];

    decl.parent = mc->GetParent();
    for (const auto &itmv: mc->GetData()->m_variables)
      decl.members[itmv.first] = itmv.second;
  }

  // Build enum and struct tables.
  std::map<TgcString, DumpedEnum> enumMap;
  for (const auto &itmc: container.m_metaConstants) {
    LPCMetaType type = nullptr;

    // Look for Enum_Type metadata.
    cstring enumType = itmc.second->GetMetaData("Enum_Type");
    if (enumType) {
      const auto &it = container.m_metaTypes.find(enumType);
      if (it != container.m_metaTypes.end())
        type = it->second;
    }

    if (!type)
      type = itmc.second->GetType();

    if (!type->IsNumber())
      continue;

    i32 value;
    if (type->SizeOfType() == 1)
      value = ((MetaConstantImpl<i08> *)itmc.second)->GetValue();
    else if (type->SizeOfType() == 2)
      value = ((MetaConstantImpl<i16> *)itmc.second)->GetValue();
    else if (type->SizeOfType() == 4)
      value = ((MetaConstantImpl<i32> *)itmc.second)->GetValue();
    else
      continue;

    auto &decl = enumMap[type->GetName()];
    decl.as = type->SizeOfType() == 1
      ? "int8_t"
      : type->SizeOfType() == 2
        ? "int16_t"
        : type->SizeOfType() == 4
          ? "int32_t"
          : "";
    decl.values[value].insert(itmc.first);
  }

  // Dump enums to json.
  for (const auto &ite: enumMap) {
    cJSON *enumDecl = cJSON_CreateObject();

    cJSON_AddStringToObject(enumDecl, "$as", ite.second.as.c_str());

    for (const auto &itv: ite.second.values)
      for (const auto &itn: itv.second)
        cJSON_AddNumberToObject(enumDecl, itn.c_str(), itv.first);

    char enumName[64] = {0};
    snprintf(enumName, 63, "E$%s", ite.first.c_str());
    cJSON_AddItemToObject(root, enumName, enumDecl);
  }

  // Dump structs to json.
  predefine(root);
  for (const auto &itmt: container.m_metaTypes) {
    if (enumMap.find(itmt.first) != enumMap.end())
      // Skip enum types.
      continue;
    if (itmt.second->AsClass())
      // Skip classes.
      continue;
    if (sExclude.find(itmt.first) != sExclude.end())
      // Exclude internal types.
      continue;

    char buffer[64];
    snprintf(buffer, 63, "A$%s", itmt.second->GetName());
    // Consider the last types as aliases.
    if (itmt.second->SizeOfType() == 1)
      cJSON_AddStringToObject(root, buffer, "int8_t");
    if (itmt.second->SizeOfType() == 2)
      cJSON_AddStringToObject(root, buffer, "int16_t");
    if (itmt.second->SizeOfType() == 4)
      cJSON_AddStringToObject(root, buffer, "int32_t");
    if (itmt.second->SizeOfType() == 8)
      cJSON_AddStringToObject(root, buffer, "int64_t");
  }

  // Dump classes to json.
  for (const auto &itmc: classMap) {
    if (sExclude.find(itmc.first) != sExclude.end())
      continue;

    const auto &mc = itmc.second;
    cJSON *classDecl = cJSON_CreateObject();

    if (mc.parent)
      cJSON_AddStringToObject(classDecl, "$parent", mc.parent->GetName());

    for (const auto &itmv: mc.members) {
      auto *mv = itmv.second;
      char typeExpr[64] = {0};

      if (!mv->IsDynamicArray()) {
        // Static (inlined) array or plain member.
        if (!mv->GetStaticSize()) {
          // "int8_t" or "Object *".
          snprintf(
            typeExpr, 63, "%s%s",
            mv->GetType()->GetName(),
            mv->GetType()->AsClass() ? " *": "");
        } else {
          // No string array in declaration groups.
          if (mv->GetType()->IsString())
            continue;

          // "int8_t[16]" or "Object[16]".
          snprintf(
            typeExpr, 63, "%s[%llu]",
            mv->GetType()->GetName(),
            mv->GetStaticSize());
        }
      } else {
        // No string array in declaration groups.
          if (mv->GetType()->IsString())
            continue;

        // "int8_t[]" or "Object *[]".
        snprintf(
          typeExpr, 63, "%s%s[]",
          mv->GetType()->GetName(),
          mv->GetType()->AsClass() ? " *": "");
      }

      cJSON_AddStringToObject(classDecl, itmv.first.c_str(), typeExpr);
    }

    char className[64] = {0};
    snprintf(className, 63, "C$%s", itmc.first.c_str());
    cJSON_AddItemToObject(root, className, classDecl);
  }

  return root;
}