#pragma once

#include <Kal/Concepts/Type.hpp>

#include <type_traits>

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

template<typename T>
concept CopyConstructible = requires (T t) {
  { T (t) } -> Same<T>;
};

template<typename T>
concept CopyAssignable = requires (T t, T u) {
  { u = t } -> Same<T>;
};

template<typename T>
concept MoveConstructible = requires (T t) {
  { T ((T &&) t) } -> Same<T>;
};

template<typename T>
concept MoveAssignable = requires (T t, T u) {
  { u = (T &&) t } -> Same<T>;
};


template<typename T>
concept Destructible = std::is_destructible_v<T>;     // TODO:
