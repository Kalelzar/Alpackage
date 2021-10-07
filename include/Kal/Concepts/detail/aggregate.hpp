#pragma once


#include <Kal/Concepts/Collection.hpp>
#include <Kal/Concepts/Functional.hpp>
#include <Kal/Concepts/Iterator.hpp>

#include <Kal/default.hpp>


/**
 * Collects the values of the collection into a single value.
 *
 * @tparam C the type of collection to aggregate
 * @tparam T the value inside the collection
 * @tparam T2 the value inside the collection with const and references stripped
 *
 * @requires{C, Iterable}
 * @requires{T2, WithDefaultValue}
 * @requires{aggregator, Aggregator}
 *
 * @arg @c collection collection to aggregate
 * @arg @c aggregator the combiner doing the aggregation
 */
template<Iterable C,
         typename T  = typename IteratesWith<C>::type,
         typename T2 = RemoveConst<RemoveRef<T>>>
  requires WithDefaultValue<T2> T2 aggregate (const C             collection,
                                              Aggregator<T2> auto aggregator) {
    T2 total = defaultValue<T2>;
    for (const T2& it : collection) { total = aggregator (total, it); }
    return total;
  }
template<typename U,
         Iterable C,
         typename T  = typename IteratesWith<C>::ctype,
         typename T2 = RemoveConst<RemoveRef<T>>>
U foldLeft (U total, const C collection, Bimap<U, T2> auto aggregator) {
  for (const T2& it : collection) { total = aggregator (total, it); }
  return total;
}

template<IndexableT C,
         typename T  = typename IndexReturnType<C, SizeT>::type,
         typename T2 = RemoveRef<T>>
  requires (!Iterable<C>)
&&WithDefaultValue<T2> T2 aggregate (const C             collection,
                                     SizeT               size,
                                     Aggregator<T2> auto aggregator) {
  T2 total = defaultValue<T2>;
  for (SizeT i = 0; i < size; i++) {
    total = aggregator (total, collection[i]);
  }
  return total;
}

template<IndexableT C,
         typename T  = typename IndexReturnType<C, SizeT>::type,
         typename T2 = RemoveRef<T>>
  requires WithDefaultValue<T2> &&(!Iterable<C>) &&SizeableT<C> RemoveRef<
    T2> aggregate (const C collection, Aggregator<T2> auto aggregator) {
    return aggregate (collection, collection.size ( ), aggregator);
  }
