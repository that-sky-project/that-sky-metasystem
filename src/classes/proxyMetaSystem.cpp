#include "internal.hpp"

ProxyMetaSystemDataContainer &ProxyMetaSystemDataContainer::operator=(
  const MetaSystemDataContainer &rhs
) {
  m_metaTypes = rhs.m_metaTypes;
  m_metaConstants = rhs.m_metaConstants;
  m_metaVariables = rhs.m_metaVariables;
  m_metaFunctions = rhs.m_metaFunctions;
  m_metaClasses = rhs.m_metaClasses;
  unk_6 = rhs.unk_6;
  unk_7 = rhs.unk_7;
  unk_8 = rhs.unk_8;
  return *this;
}

ProxyMetaSystem *ProxyMetaSystem::create(
  u32 maxClasses
) {
  ProxyMetaSystemDataContainer *pData = new ProxyMetaSystemDataContainer();
  pData->m_maxClasses = maxClasses;
  pData->m_count = 0;

  ProxyMetaSystem *pMetaSystem = (ProxyMetaSystem *)operator new(
    sizeof(ProxyMetaSystem) + sizeof(const MetaClass *) * maxClasses);
  new (pMetaSystem) ProxyMetaSystem();
  pMetaSystem->m_data = pData;

  for (u32 i = 0; i < maxClasses; i++)
    pMetaSystem->m_classes[i] = GetMetaClass();

  return pMetaSystem;
}

void ProxyMetaSystem::set(
  const MetaSystem *p,
  u32 count
) {
  *m_data = *p->m_data;

  // Actually we don't know how many classes we need to copy at compile time.
  memcpy(m_classes, (const void *)p->m_classes, count * sizeof(const MetaClass *));

  m_data->m_count = count;
}

bool ProxyMetaSystem::addClass(
  MetaClass *clazz
) {
  if (!m_data)
    return false;

  // To make it easier for a mod to register metaclasses for its dependencies,
  // we stipulate: if a metaclass with the same name already exists in the registry,
  // only set the input metaclass's m_self. Since almost all metaclass operations
  // must be performed via Must_call_META_REGISTER_CLASS or GetMetaClassByType
  // (i.e., through m_self), all metaclass operations can be forwarded to the
  // existing metaclass.
  // 
  // Furthermore, because a dependent module is always loaded after its dependency,
  // and a child class is always loaded after its parent class, this initialization
  // process is guaranteed to be stable.
  // 
  // Thus, an external module only needs to declare an "empty" metaclass with the
  // same name to use the metaclass of its dependency.
  const auto &itClass = m_data->m_metaTypes.find(clazz->m_name);
  if (itClass != m_data->m_metaTypes.end()) {
    clazz->m_self = itClass->second;
    return true;
  }

  if (!clazz->AsClass())
    return false;

  char *name = new char[strlen(clazz->m_name) + 1];
  strcpy(name, clazz->m_name);

  auto *mc = (MetaClass *)clazz->Copy();
  mc->m_name = name;
  mc->m_self = clazz->m_self = mc;
  mc->m_globalId = m_data->m_count++;
  if (mc->m_metaDataContainer)
    delete mc->m_metaDataContainer;
  mc->m_metaDataContainer = new MetaDataContainer();

  m_classes[mc->m_globalId] = mc;

  m_data->m_metaTypes[clazz->m_name] = mc;
  m_data->m_metaClasses[clazz->m_name] = mc;

  return true;
}
