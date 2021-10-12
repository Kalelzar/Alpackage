#pragma once

#include <Kal/Concepts/Collection.hpp>
#include <Kal/Concepts/Functional.hpp>
#include <Kal/Concepts/Iterator.hpp>

#include <Kal/default.hpp>

#include <utility>


template<typename T, template<typename> class C>
  requires Iterable<C<T>> && Appendable<C, T> C<T>
    filter (const C<T>& collection, Predicate<T> auto predicate) {
    C<T> newCollection = defaultValue<C<T>>;
    for (const T& it : collection) {
      if (predicate (it)) { newCollection.push_back (it); }
    }

    return std::move (newCollection);
  }

template<typename T, template<typename> class C>
  requires Iterable<C<T>> && Insertable<C, T> C<T>
    filter (const C<T> collection, Predicate<T> auto predicate) {
    C<T> newCollection = defaultValue<C<T>>;
    for (const T& it : collection) {
      if (predicate (it)) { newCollection.insert (it); }
    }

    return std::move (newCollection);
  }

template<typename T, template<typename> class C>
  requires (!Iterable<C<T>>)
&&IndexableT<C<T>>&& SizeableT<C<T>>&& Appendable<C, T> C<T>
  filter (const C<T> collection, Predicate<T> auto predicate) {
  C<T> newCollection = defaultValue<C<T>>;
  for (SizeT i = 0; i < collection.size ( ); i++) {
    if (predicate (collection[i])) { newCollection.push_back (collection[i]); }
  }

  return std::move (newCollection);
}

template<typename T>
requires IndexableT<T*> T* filter (SizeT             size,
                                   const T*          collection,
                                   T*                newCollection,
                                   Predicate<T> auto predicate) {
  for (SizeT i = 0; i < size; i++) {
    if (predicate (collection[i])) { newCollection[i] = collection[i]; }
  }
  return newCollection;
}

template<typename T, template<typename> class C>
  requires Reversible<C<T>> &&(!Appendable<C, T>) &&Iterable<
    C<T>>&& Prependable<C, T> C<T> filter (const C<T>        collection,
                                           Predicate<T> auto predicate) {
    C<T> newCollection = defaultValue<C<T>>;
    for (const T& it : collection) {
      if (predicate (it)) { newCollection.push_front (it); }
    }

    newCollection.reverse ( );

    return std::move (newCollection);
  }
