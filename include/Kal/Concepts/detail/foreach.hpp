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
  requires IndexableT<C> &&(!Iterable<C>) &&SizeableT<C> void foreach (
    const C          collection,
    Consumer<T> auto consumer) {
    for (SizeT i = 0; i < collection.size ( ); i++) {
      consumer (collection[i]);
    }
  }
