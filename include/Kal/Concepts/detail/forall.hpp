#pragma once

#include <Kal/Concepts/Collection.hpp>
#include <Kal/Concepts/Functional.hpp>
#include <Kal/Concepts/Iterator.hpp>

#include <Kal/default.hpp>

#include <utility>


template<typename T, template<typename> class C>
  requires Iterable<C<T>> && Appendable<C, T>
bool forall (const C<T>& collection, Predicate<T> auto predicate) {
  for (const T& it : collection) {
    if (!predicate (it)) { return false; }
  }

  return true;
}

template<typename T, template<typename> class C>
  requires Iterable<C<T>> && Insertable<C, T>
bool forall (const C<T> collection, Predicate<T> auto predicate) {
  for (const T& it : collection) {
    if (!predicate (it)) { return false; }
  }

  return true;
}

template<typename T, template<typename> class C>
  requires (!Iterable<C<T>>)
&&IndexableT<C<T>>&& SizeableT<C<T>>&& Appendable<C, T> bool
  forall (const C<T> collection, Predicate<T> auto predicate) {
  for (SizeT i = 0; i < collection.size ( ); i++) {
    if (!predicate (collection[i])) { return false; }
  }

  return true;
}

template<typename T>
  requires IndexableT<T*>
bool forall (SizeT size, const T* collection, Predicate<T> auto predicate) {
  for (SizeT i = 0; i < size; i++) {
    if (!predicate (collection[i])) { return false; }
  }
  return true;
}

template<typename T, template<typename> class C>
  requires Reversible<C<T>> &&(!Appendable<C, T>) &&Iterable<
    C<T>>&& Prependable<C, T> bool forall (const C<T>        collection,
                                           Predicate<T> auto predicate) {
    for (const T& it : collection) {
      if (!predicate (it)) { return false; }
    }

    return true;
  }
