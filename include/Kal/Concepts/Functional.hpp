#pragma once

#include <Kal/Concepts/Type.hpp>

#include <Kal/default.hpp>

template<typename T, typename U = T>
concept Comparable = requires (T t, U u) {
  { t == u } -> Same<bool>;
  { t != u } -> Same<bool>;
  { t >= u } -> Same<bool>;
  { t <= u } -> Same<bool>;
  { t < u } -> Same<bool>;
  { t > u } -> Same<bool>;
};

template<typename T, typename... Args>
concept Callable = requires (T t, Args... args) {
  t (args...);
};

template<template<typename> class T, typename U>
concept Liftable = requires (U u) {
  { T<U>::lift (u) } -> Same<T<U>>;
};

template<typename T, typename From, typename To>
concept Transformer = requires (T transformer, From t) {
  { transformer (t) } -> Same<To>;
};


template<typename Source, typename Result> static Result __map (const Source&) {
  return defaultValue<Result>;
}

template<template<typename> class Kind, typename From, typename To>
concept Mappable = requires (Kind<From> kind) {
  { kind.template map<To> (__map<From, To>) } -> Same<Kind<To>>;
};

template<typename From>
static From __reduce (const From& accumulator, const From& next) {
  return accumulator;
}

template<template<typename> class Kind, typename From>
concept Reducible = requires (Kind<From> kind) {
  { kind.reduce (__reduce<From>) } -> Same<From>;
};

template<typename Source, typename Result>
static Result __fold (const Result& accumulator, const Source& next) {
  return accumulator;
}

template<template<typename> class Kind, typename Source, typename Result>
concept Foldable = requires (Kind<Source> kind, Result result) {
  {
    kind.template fold<Result> (result, __fold<Source, Result>)
    } -> Same<Result>;
};

template<typename T, typename TestType>
concept Predicate = requires (T predicate, TestType t) {
  { predicate (t) } -> Same<bool>;
};

template<typename T, typename TestType1, typename TestType2>
concept Bipredicate = requires (T predicate, TestType1 t1, TestType2 t2) {
  { predicate (t1, t2) } -> Same<bool>;
};

template<typename T, typename TestType>
concept Consumer = requires (T consumer, TestType t) {
  { consumer (t) } -> Same<void>;
};

template<typename T, typename Aggregate>
concept Aggregator = requires (T aggregator, Aggregate a, Aggregate b) {
  { aggregator (a, b) } -> Same<Aggregate>;
};

template<typename T, typename U, typename Aggregate>
concept Bimap = requires (T aggregator, U a, Aggregate b) {
  { aggregator (a, b) } -> Same<U>;
};
