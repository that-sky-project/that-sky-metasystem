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

  if (m_data->m_metaTypes.find(clazz->m_name) != m_data->m_metaTypes.end())
    return false;

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
}
