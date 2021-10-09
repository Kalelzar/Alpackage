#pragma once

#include <Kal/Concepts/Collection.hpp>
#include <Kal/Concepts/Functional.hpp>
#include <Kal/Concepts/Iterator.hpp>

#include <Kal/default.hpp>

template<typename T>
void foreach (const Iterable auto collection, Consumer<T> auto consumer) {
  for (const T& it : collection) { consumer (it); }
}

template<typename T, typename C>
  requires (!Iterable<C>)
&&IndexableT<C> void foreach (SizeT            size,
                              const C          collection,
                              Consumer<T> auto consumer) {
  for (SizeT i = 0; i < size; i++) { consumer (collection[i]); }
}

template<typename T, typename C>
  requires SizeableT<C> &&(!Iterable<C>) &&IndexableT<C> void foreach (
    const C          collect,
    Consumer<T> auto consumer) {
    foreach (collect.size ( ), collect, consumer)
      ;
  }
