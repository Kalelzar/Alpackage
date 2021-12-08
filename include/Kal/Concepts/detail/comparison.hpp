#pragma once

#include <Kal/Concepts/Comparators.hpp>
#include <Kal/Concepts/Iterator.hpp>

template<typename A, Comparator<A> C> class ComparatorOps {
  private:
  static const C instance{ };

  public:
  static A& max (A& left, A& right) {
    return instance.greater (left, right) ? left : right;
  };

  static A& min (A& left, A& right) {
    return instance.lesser (left, right) ? left : right;
  };


  template<template<typename> class Collection>
  requires AnyIterable<Collection<A>> && LowerBound<A> A&
    max (Collection<A> const& c) {
    A& r = minBound<A>;
    for (A const& a : c) {
      if (instance.greater (a, r)) { r = a; }
    }
    return r;
  }

  template<template<typename> class Collection>
  requires AnyIterable<Collection<A>> && UpperBound<A> A&
    min (Collection<A> const& c) {
    A& r = maxBound<A>;
    for (A const& a : c) {
      if (instance.lesser (a, r)) { r = a; }
    }
    return r;
  }
};
