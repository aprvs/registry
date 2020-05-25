#ifndef REGISTRY_REGISTRY_H_
#define REGISTRY_REGISTRY_H_

#include <array>
#include <memory>
#include <set>
#include <unordered_map>

#include "common/error_or.h"
#include "common/type_traits.h"

namespace registry {

namespace internal {

// The Registry allows pretty much only alphanumeric characters for element
// names. The following characters may be used by the Regsitry for internal
// house-keeping functions
constexpr char kRegistryReservedChars[] = "<>(){}[]#$!@%^&|~`;:.,/*-+= ";

}  // namespace internal

/// @class Registry
class Registry {
 public:
  /// @class Element
  /// Interface for types held by the registry. Defines API that the Registry
  /// needs for its housekeeping, data logging and parameter server functions
  class Element {
   public:
    Element(const std::string& name, TypeEnum type);
    virtual ~Element();

    Element(const Element&) = delete;
    Element& operator=(const Element&) = delete;

    TypeEnum type() const { return type_; }

    const std::string& name() const { return name_; }
    std::string FullName() const;

    template <typename T>
    bool Assign(const T& other) {
      if (TypeTrait<T>::type != type_) {
        return false;
      }
      Assign(static_cast<void const*>(&other));
      return true;
    }

    template <typename T>
    bool Extract(T* other) const {
      if (TypeTrait<T>::type != type_) {
        return false;
      }
      *other = *(reinterpret_cast<T const*>(Extract()));
      return true;
    }

   protected:
    virtual void Assign(void const* other) = 0;
    virtual void const* Extract() const = 0;

   private:
    friend class Registry;

    const std::string name_;
    const TypeEnum type_;
    Registry const* registry_;
  };

  template <typename T>
  class ElementTemplate : public Element {
    static_assert(std::is_copy_assignable<T>::value &&
                      std::is_copy_constructible<T>::value,
                  "Registry elements must be copy constructible / assignable");

   public:
    using ValueType = T;

    ElementTemplate(const std::string& name, const T& initial_value)
        : Element(name, TypeTrait<T>::type), value_(initial_value) {}

    ElementTemplate(const std::string& name)
        : Element(name, TypeTrait<T>::type),
          value_(TypeTrait<T>::default_value) {}

    ~ElementTemplate() override {}

    // At the specific template level we allow direct assignment to and from the
    // underlying type, this allows for all the methods / operators defined for
    // the underlying type to be available to the Registry::ElementTemplate
    inline const T& value() const { return value_; }

    inline const T& operator=(const T& other) {
      value_ = other;
      return other;
    }

    operator T() const { return value(); }

   protected:
    /// An unsafe assignment function that updates the internal value of the
    /// element using the data at the specified memory location
    /// Note: This is extremely unsafe to use directly and is only
    /// designed to be used via the Element class which ensures type safety
    /// @param other pointer to the memory location that the Element should take
    /// on the value of
    void Assign(void const* other) override {
      value_ = *(reinterpret_cast<T const*>(other));
    }

    /// An unsafe getter function
    /// Note: This is extremely unsafe to use directly and is only
    /// designed to be used via the Element class which ensures type safety
    /// @return  pointer to the memory location that the Element is using to
    /// store its curernt value
    void const* Extract() const override {
      return reinterpret_cast<void const*>(&value_);
    }

   private:
    T value_;
  };

  using Int32 = ElementTemplate<int32_t>;
  using UnsignedInt32 = ElementTemplate<uint32_t>;
  using Int64 = ElementTemplate<int64_t>;
  using UnsignedInt64 = ElementTemplate<uint64_t>;
  using Bool = ElementTemplate<bool>;
  using Char = ElementTemplate<char>;
  using String = ElementTemplate<std::string>;
  using Float = ElementTemplate<float>;
  using Double = ElementTemplate<double>;

  Registry(const std::string& name);

  Registry(const Registry&) = delete;
  Registry& operator=(const Registry&) = delete;

  const std::string& name() const { return name_; }
  std::string FullName() const;

  /// Search for a child registry by its name
  /// @param[in] name unique string identifier for the child registry
  /// @return pointer to the registry if found, else an error code
  common::ErrorOr<Registry*> FindChildRegistry(const std::string& name);

  /// Adds a child registry
  /// @param[in] name unique child identifier
  /// @return pointer to child registry if unique name is not already used, else
  /// an error code
  common::ErrorOr<Registry*> AddChildRegistry(const std::string& name);

  /// Finds and returns an existing child registry. Creates a registry if one is
  /// not already present
  /// @param[in] name unique name for the registry
  /// @return pointer to the child registry
  Registry* FindOrAddChildRegistry(const std::string& name);

  common::ErrorOr<Element*> FindElement(const std::string& name);
  common::ErrorOr<Element*> FindElementByExtendedName(const std::string& name);

  common::ErrorOr<Int32*> FindInt32(const std::string& name);
  common::ErrorOr<Int32*> AddInt32(const std::string& name);

  common::ErrorOr<UnsignedInt32*> FindUnsignedInt32(const std::string& name);
  common::ErrorOr<UnsignedInt32*> AddUnsignedInt32(const std::string& name);

  common::ErrorOr<Int64*> FindInt64(const std::string& name);
  common::ErrorOr<Int64*> AddInt64(const std::string& name);

  common::ErrorOr<UnsignedInt64*> FindUnsignedInt64(const std::string& name);
  common::ErrorOr<UnsignedInt64*> AddUnsignedInt64(const std::string& name);

  common::ErrorOr<Bool*> FindBoolean(const std::string& name);
  common::ErrorOr<Bool*> AddBoolean(const std::string& name);

  common::ErrorOr<Char*> FindChar(const std::string& name);
  common::ErrorOr<Char*> AddChar(const std::string& name, char value);

  common::ErrorOr<String*> FindString(const std::string& name);
  common::ErrorOr<String*> AddString(const std::string& name,
                                     const std::string& value);

  common::ErrorOr<Float*> FindFloat(const std::string& name);
  common::ErrorOr<Float*> AddFloat(const std::string& name);

  common::ErrorOr<Double*> FindDouble(const std::string& name);
  common::ErrorOr<Double*> AddDouble(const std::string& name);

  std::set<std::string> GetChildRegistryNames() const;

 private:
  using ChildMap = std::unordered_map<std::string, std::unique_ptr<Registry>>;
  using ElementMap = std::unordered_map<std::string, std::unique_ptr<Element>>;

  template <typename ElementType>
  common::ErrorOr<ElementType*> FindElementType(const std::string& name);

  template <typename ElementType, typename... Args>
  common::ErrorOr<ElementType*> AddElementType(Args... args);

  const std::string name_;
  Registry const* parent_;

  ChildMap child_registries_;
  ElementMap elements_;
};

}  // namespace registry

#endif  // REGISTRY_REGISTRY_H_
