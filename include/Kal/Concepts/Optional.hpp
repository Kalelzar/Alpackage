#pragma once

#include <Kal/Concepts/Type.hpp>

template<template<typename> class T, typename U = int>
concept Optional = NotNull<U> && requires (T<U> t, U o) {
  { T<U>::None ( ) } -> Same<T<U>>;

  { T<U>::Some (o) } -> Same<T<U>>;

  { t.isEmpty ( ) } -> Same<bool>;

  { t.get ( ) } -> Same<U>;

  { t.getOrElse (o) } -> Same<U>;

  { t.isDefined ( ) } -> Same<bool>;
};
