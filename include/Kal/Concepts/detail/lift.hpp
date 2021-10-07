#pragma once

#include <Kal/Concepts/Collection.hpp>
#include <Kal/Concepts/Functional.hpp>
#include <Kal/Concepts/Iterator.hpp>

#include <Kal/default.hpp>

#include "map.hpp"

template<template<typename A> class Kind, typename T> Kind<T> lift (T);
template<template<typename A> class Kind, typename T>
  requires ConstructibleFrom<Kind<T>, T> Kind<T> lift (T t) { return {t}; }

template<template<typename A> class Kind, typename T>
  requires Liftable<Kind, T> Kind<T> lift (T t) { return Kind<T>::lift (t); }

template<template<typename> class Kind, template<typename> class C, typename T>
C<Kind<T>> liftAll (const C<T> collection) {
  return map<Kind<T>, T, C> (collection,
                             [&] (const T& it) { return lift<Kind> (it); });
}
