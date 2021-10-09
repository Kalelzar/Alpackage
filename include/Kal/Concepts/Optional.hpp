#pragma once

#include <Kal/Concepts/Construction.hpp>
#include <Kal/Concepts/Type.hpp>

template<template<typename> class T, typename U = int>
concept Optional = NotNull<U> && TriviallyConstructible<
  T<U>> && ConstructibleFrom<T<U>, U> && requires (T<U> t, U o) {
  { t.isEmpty ( ) } -> Same<bool>;

  { t.get ( ) } -> Same<U>;

  { t.getOrElse (o) } -> Same<U>;

  { t.isDefined ( ) } -> Same<bool>;
};
