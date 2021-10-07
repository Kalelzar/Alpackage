#pragma once

#include <Kal/Concepts/Collection.hpp>
#include <Kal/Concepts/Functional.hpp>
#include <Kal/Concepts/Iterator.hpp>

#include <Kal/default.hpp>

template<typename U, typename T, template<typename> class C>
  requires Mappable<C, T, U> C<U> map (const C<T>             collection,
                                       Transformer<T, U> auto transformer) {
    return collection.map (transformer);
  }

template<typename U, typename T, template<typename> class C>
  requires Iterable<C<T>> && Appendable<C, U> C<U>
    map (const C<T> collection, Transformer<T, U> auto transformer) {
    C<U> newCollection = defaultValue<C<U>>;
    for (const T& it : collection) {
      newCollection.push_back (transformer (it));
    }

    return newCollection;
  }

template<typename U, typename T, template<typename> class C>
  requires Iterable<C<T>> && Insertable<C, U> C<U>
    map (const C<T> collection, Transformer<T, U> auto transformer) {
    C<U> newCollection = defaultValue<C<U>>;
    for (const T& it : collection) { newCollection.insert (transformer (it)); }

    return newCollection;
  }

template<typename U, typename T, template<typename> class C>
  requires IndexableT<C<T>> &&(!Iterable<C<T>>) &&SizeableT<
    C<T>>&& Appendable<C, T>&& Appendable<C, U> C<U> map (const C<T> collection,
                                                          Transformer<T, U> auto
                                                            transformer) {
    C<U> newCollection = defaultValue<C<U>>;
    for (SizeT i = 0; i < collection.size ( ); i++) {
      newCollection.push_back (transformer (collection[i]));
    }

    return newCollection;
  }

  template<typename U, typename T, template<typename> class C>
    requires Iterable<C<T>> && Reversible<C<U>> && Prependable<C, U> &&(
      !Appendable<C, U>) C<U> map (const C<T>             collection,
                                   Transformer<T, U> auto transformer) {
      C<U> newCollection = defaultValue<C<U>>;
      for (const T& it : collection) {
        newCollection.push_front (transform (it));
      }

      newCollection.reverse ( );

      return newCollection;
    }
