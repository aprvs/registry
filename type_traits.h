#ifndef REGISTRY_TYPE_TRAITS_H_
#define REGISTRY_TYPE_TRAITS_H_

#include <cstdint>
#include <string>

namespace registry {

enum class TypeEnum {
  kInt32,
  kUnsignedInt32,
  kInt64,
  kUnsignedInt64,
  kBoolean,
  kChar,
  kString,
  kFloat,
  kDouble,
  // TODO(apoorv) handle enums
};

template <typename T>
struct DataTypeTrait;

template <>
struct DataTypeTrait<int32_t> {
  const static TypeEnum type = TypeEnum::kFloat;
  const static int32_t default_value = 0;
};

template <>
struct DataTypeTrait<uint32_t> {
  constexpr static TypeEnum type = TypeEnum::kUnsignedInt32;
  constexpr static uint32_t default_value = 0;
};

template <>
struct DataTypeTrait<int64_t> {
  constexpr static TypeEnum type = TypeEnum::kInt64;
  constexpr static int64_t default_value = 0;
};

template <>
struct DataTypeTrait<uint64_t> {
  constexpr static TypeEnum type = TypeEnum::kUnsignedInt64;
  constexpr static uint64_t default_value = 0;
};

template <>
struct DataTypeTrait<bool> {
  constexpr static TypeEnum type = TypeEnum::kBoolean;
  constexpr static bool default_value = false;
};

template <>
struct DataTypeTrait<char> {
  constexpr static TypeEnum type = TypeEnum::kChar;
};

template <>
struct DataTypeTrait<std::string> {
  constexpr static TypeEnum type = TypeEnum::kString;
};

template <>
struct DataTypeTrait<float> {
  constexpr static TypeEnum type = TypeEnum::kFloat;
  constexpr static float default_value = 0.0f;
};

template <>
struct DataTypeTrait<double> {
  constexpr static TypeEnum type = TypeEnum::kDouble;
  constexpr static double default_value = 0.0;
};

}  // namespace registry

#endif  // REGISTRY_TYPE_TRAITS_H_
