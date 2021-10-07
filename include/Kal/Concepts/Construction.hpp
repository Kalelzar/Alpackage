#pragma once

#include <Kal/Concepts/Type.hpp>

template<typename T>
concept TriviallyConstructible = requires ( ) {
  {
    T { }
    } -> Same<T>;
};

template<typename T, typename... U>
concept ConstructibleFrom = requires (U... u) {
  { T (u...) } -> Same<T>;
};
