#pragma once

#include <Kal/Concepts/Collection.hpp>
#include <Kal/Concepts/Functional.hpp>
#include <Kal/Concepts/Iterator.hpp>

#include <Kal/default.hpp>


template<typename T, template<typename> class C>
  requires Iterable<C<T>> && Appendable<C, T> C<T>
    filter (const C<T> collection, Predicate<T> auto predicate) {
    C<T> newCollection = defaultValue<C<T>>;
    for (const T& it : collection) {
      if (predicate (it)) newCollection.push_back (it);
    }

    return newCollection;
  }

template<typename T, template<typename> class C>
  requires Iterable<C<T>> && Insertable<C, T> C<T>
    filter (const C<T> collection, Predicate<T> auto predicate) {
    C<T> newCollection = defaultValue<C<T>>;
    for (const T& it : collection) {
      if (predicate (it)) newCollection.insert (it);
    }

    return newCollection;
  }

template<typename T, template<typename> class C>
  requires (!Iterable<C<T>>)
&&IndexableT<C<T>>&& SizeableT<C<T>>&& Appendable<C, T> C<T>
  filter (const C<T> collection, Predicate<T> auto predicate) {
  C<T> newCollection = defaultValue<C<T>>;
  for (SizeT i = 0; i < collection.size ( ); i++) {
    if (predicate (collection[i])) newCollection.push_back (collection[i]);
  }

  return newCollection;
}

template<typename T, template<typename> class C>
  requires Iterable<C<T>> && Reversible<C<T>> && Prependable<C, T> &&(
    !Appendable<C, T>) C<T> filter (const C<T>        collection,
                                    Predicate<T> auto predicate) {
    C<T> newCollection = defaultValue<C<T>>;
    for (const T& it : collection) {
      if (predicate (it)) newCollection.push_front (it);
    }

    newCollection.reverse ( );

    return newCollection;
  }
