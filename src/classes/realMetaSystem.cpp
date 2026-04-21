#include "internal.hpp"

void RealMetaSystem::initialize(
  const FakeMetaSystem *original
) {
  registeredClasses.clear();
  registeredTree.clear();
  container = *original->data;

  for (auto &it: container.m_metaClasses) {
    gRealMetaSystem.addClass(it.second);
    if (it.second->m_parent) {
      const MetaClass *parent = it.second->m_parent();
      gRealMetaSystem.addChild(parent, it.second);
    }
  }
}

void RealMetaSystem::addClass(
  const MetaClass *clazz
) {
  registeredClasses[clazz->m_name] = clazz;
}

void RealMetaSystem::addChild(
  const MetaClass *parent,
  const MetaClass *child
) {
  registeredTree[parent->m_name].push_back(child->m_name);
}

void RealMetaSystem::dumpTree(
  std::ostringstream &s
) const {
  std::unordered_map<TgcString, bool> visitState;

  for (auto &it: registeredTree)
    visitState[it.first] = false;

  for (auto &it: registeredTree) {
    if (visitState[it.first])
      continue;

    recursiveChild(s, it.first, visitState);
  }
}

void RealMetaSystem::recursiveChild(
  std::ostringstream &s,
  TgcString className,
  std::unordered_map<TgcString, bool> &visitState
) const {
  if (visitState[className])
    return;

  // Get parent class.
  const auto &it = registeredClasses.find(className);
  const MetaClass *parent = nullptr;
  if (it->second->m_parent)
    parent = it->second->m_parent();

  if (parent)
    recursiveChild(s, parent->m_name, visitState);

  if (visitState[className])
    return;

  s << "class " << className;

  if (parent)
    s << ": public " << parent->m_name;

  s << " { };\n";

  visitState[className] = true;

  const auto &children = registeredTree.find(className);
  if (children == registeredTree.end())
    return;

  for (auto &it: children->second) {
    if (visitState[it])
      continue;

    recursiveChild(s, it, visitState);
  }
}
