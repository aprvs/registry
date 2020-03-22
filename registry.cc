#include "registry/registry.h"

#include <algorithm>
#include <iostream>
#include <memory>

namespace registry {

namespace internal {

constexpr char kNamespaceCharacter = '.';
std::string RemoveReservedCharacters(const std::string& name,
                                     const char* reserved_chars) {
  std::string corrected_name(name);
  corrected_name.erase(
      std::remove_if(
          corrected_name.begin(), corrected_name.end(),
          [&reserved_chars](char string_char) {
            return std::find(reserved_chars,
                             reserved_chars +
                                 std::char_traits<char>::length(reserved_chars),
                             string_char) !=
                   (reserved_chars +
                    std::char_traits<char>::length(reserved_chars));
          }),
      corrected_name.end());
  // TODO handle cases where the name is empty
  return corrected_name;
}

}  // namespace internal

Registry::Element::Element(const std::string& name, TypeEnum type)
    : name_(internal::RemoveReservedCharacters(
          name, internal::kRegistryReservedChars)),
      type_(type),
      registry_(nullptr) {}

Registry::Element::~Element() {}

std::string Registry::Element::FullName() const {
  if (registry_ == nullptr) {
    return name();
  }
  return registry_->FullName() + "." + name();
}

Registry::Registry(const std::string& name)
    : name_(internal::RemoveReservedCharacters(
          name, internal::kRegistryReservedChars)),
      parent_(nullptr) {}

template <typename ElementType>
common::ErrorOr<ElementType*> Registry::FindElementType(
    const std::string& name) {
  common::ErrorOr<Element*> maybe_element = FindElement(name);
  if (!maybe_element.HasValue()) {
    return maybe_element.ErrorOrDie();
  }
  Element* element = maybe_element.ValueOrDie();
  if (element->type() != DataTypeTrait<typename ElementType::ValueType>::type) {
    return common::Error::kNotFound;
  }
  return static_cast<ElementType*>(element);
}

template <typename ElementType, typename... Args>
common::ErrorOr<ElementType*> Registry::AddElementType(Args... args) {
  auto element = std::make_unique<ElementType>(std::forward<Args>(args)...);
  std::pair<ElementMap::iterator, bool> ref =
      elements_.emplace(element->name(), std::move(element));
  if (ref.second) {
    ref.first->second->registry_ = this;
    return static_cast<ElementType*>(ref.first->second.get());
  }
  return common::Error::kUnavailable;
}

std::string Registry::FullName() const {
  if (parent_ == nullptr) {
    return name();
  }
  return parent_->FullName() + "." + name();
}

common::ErrorOr<Registry*> Registry::FindChildRegistry(
    const std::string& name) {
  ChildMap::iterator it = child_registries_.find(name);
  if (it != child_registries_.end()) {
    return it->second.get();
  }
  return common::Error::kNotFound;
}

common::ErrorOr<Registry*> Registry::AddChildRegistry(const std::string& name) {
  auto child = std::make_unique<Registry>(name);
  std::pair<ChildMap::iterator, bool> ref =
      child_registries_.emplace(child->name(), std::move(child));
  if (ref.second) {
    ref.first->second->parent_ = this;
    return ref.first->second.get();
  }
  return common::Error::kUnavailable;
}

common::ErrorOr<Registry::Element*> Registry::FindElement(
    const std::string& name) {
  ElementMap::iterator it = elements_.find(name);
  if (it != elements_.end()) {
    return it->second.get();
  }
  return common::Error::kNotFound;
}

common::ErrorOr<Registry::Element*> Registry::FindElementByExtendedName(
    const std::string& search_name) {
  // Break up the string using the namespace separator (periods)
  auto it = std::find(search_name.cbegin(), search_name.cend(),
                      internal::kNamespaceCharacter);
  if (it == search_name.cend()) {
    return FindElement(search_name);
  }
  std::string registry_name(search_name.cbegin(), it);
  if (registry_name == name()) {
    return FindElementByExtendedName(std::string(++it, search_name.cend()));
  }
  common::ErrorOr<Registry*> child_registry = FindChildRegistry(registry_name);
  if (!child_registry.HasValue()) {
    return common::Error::kNotFound;
  }
  return child_registry.ValueOrDie()->FindElementByExtendedName(
      std::string(++it, search_name.cend()));
}

Registry* Registry::FindOrAddChildRegistry(const std::string& name) {
  ChildMap::iterator it = child_registries_.find(name);
  if (it != child_registries_.end()) {
    return it->second.get();
  }
  std::pair<ChildMap::iterator, bool> ref =
      child_registries_.emplace(name, std::make_unique<Registry>(name));
  return ref.first->second.get();
}

common::ErrorOr<Registry::Int32*> Registry::FindInt32(const std::string& name) {
  return FindElementType<Int32>(name);
}

common::ErrorOr<Registry::Int32*> Registry::AddInt32(const std::string& name) {
  return AddElementType<Int32>(name);
}

common::ErrorOr<Registry::UnsignedInt32*> Registry::FindUnsignedInt32(
    const std::string& name) {
  return FindElementType<UnsignedInt32>(name);
}

common::ErrorOr<Registry::UnsignedInt32*> Registry::AddUnsignedInt32(
    const std::string& name) {
  return AddElementType<UnsignedInt32>(name);
}

common::ErrorOr<Registry::Int64*> Registry::FindInt64(const std::string& name) {
  return FindElementType<Int64>(name);
}

common::ErrorOr<Registry::Int64*> Registry::AddInt64(const std::string& name) {
  return AddElementType<Int64>(name);
}

common::ErrorOr<Registry::UnsignedInt64*> Registry::FindUnsignedInt64(
    const std::string& name) {
  return FindElementType<UnsignedInt64>(name);
}

common::ErrorOr<Registry::UnsignedInt64*> Registry::AddUnsignedInt64(
    const std::string& name) {
  return AddElementType<UnsignedInt64>(name);
}

common::ErrorOr<Registry::Bool*> Registry::FindBoolean(
    const std::string& name) {
  return FindElementType<Bool>(name);
}

common::ErrorOr<Registry::Bool*> Registry::AddBoolean(const std::string& name) {
  return AddElementType<Bool>(name);
}

common::ErrorOr<Registry::Char*> Registry::FindChar(const std::string& name) {
  return FindElementType<Char>(name);
}

common::ErrorOr<Registry::Char*> Registry::AddChar(const std::string& name,
                                                   char value) {
  return AddElementType<Char>(name, value);
}

common::ErrorOr<Registry::String*> Registry::FindString(
    const std::string& name) {
  return FindElementType<String>(name);
}

common::ErrorOr<Registry::String*> Registry::AddString(
    const std::string& name, const std::string& value) {
  return AddElementType<String>(name, value);
}

common::ErrorOr<Registry::Float*> Registry::FindFloat(const std::string& name) {
  return FindElementType<Float>(name);
}

common::ErrorOr<Registry::Float*> Registry::AddFloat(const std::string& name) {
  return AddElementType<Float>(name);
}

common::ErrorOr<Registry::Double*> Registry::FindDouble(
    const std::string& name) {
  return FindElementType<Double>(name);
}

common::ErrorOr<Registry::Double*> Registry::AddDouble(
    const std::string& name) {
  return AddElementType<Double>(name);
}

std::set<std::string> Registry::GetChildRegistryNames() const {
  std::set<std::string> child_registry_names;
  for (const auto& child_entry : child_registries_) {
    child_registry_names.insert(child_entry.first);
  }
  return child_registry_names;
}

}  // namespace registry
