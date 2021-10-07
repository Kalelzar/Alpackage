#pragma once

#include <Kal/Concepts/Collection.hpp>
#include <Kal/Concepts/Functional.hpp>
#include <Kal/Concepts/Iterator.hpp>
#include <Kal/Concepts/Value.hpp>

#include <Kal/default.hpp>

#include <utility>

// @test
template<typename T, template<typename> class C>
  requires Iterable<C<T>> && Appendable<C, T> && Equal<T> C<T>
    unique (const C<T> iterable) {
    C<T> newCollection = defaultValue<C<T>>;
    for (const T& it : iterable) {
      bool skip = false;
      for (const T& other : newCollection) {
        if (other == it) {
          skip = true;
          break;
        }
      }
      if (!skip) newCollection.push_back (it);
      continue;
    }
    return std::move (newCollection);
  }


// @test
template<typename T, template<typename> class C>
  requires Iterable<C<T>> && Appendable<C, T> C<T>
    uniqueBy (const C<T> iterable, Bipredicate<T, T> auto bipredicate) {
    C<T> newCollection = defaultValue<C<T>>;
    for (const T& it : iterable) {
      for (const T& other : newCollection) {
        if (bipredicate (it, other)) goto next;
      }
      newCollection.push_back (it);
next:
      continue;
    }
    return std::move (newCollection);
  }
