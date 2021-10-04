#pragma once

#include <Kal/Concepts/Construction.hpp>
#include <Kal/Concepts/Primitive.hpp>

template<typename T> inline const T     defaultValue;
template<Integral T> inline constexpr T defaultValue<T> = 0;

template<typename T>
concept WithDefaultValue = requires (T b) {
  {b = defaultValue<T>};
};
