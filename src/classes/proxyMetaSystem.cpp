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
}
