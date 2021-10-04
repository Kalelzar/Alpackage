#pragma once

#include <Kal/Concepts/Primitive.hpp>
#include <Kal/Concepts/Type.hpp>
#include <cstddef>

template<typename T, typename Arg>
concept Indexable = requires (T t, Arg arg) {
  t[arg];
};

template<typename T, typename SizeType>
concept Sizeable = Integral<SizeType> && requires (T t) {
  { t.size ( ) } -> Same<SizeType>;
};

using SizeT = size_t;

template<typename T>
concept IndexableT = Indexable<T, SizeT>;

template<typename T, typename Arg> struct IndexReturnType;
template<typename Arg, Indexable<Arg> T> struct IndexReturnType<T, Arg> {
  private:
  T   t;
  Arg arg;
  public:
  using type = decltype (t[arg]);
};

template<IndexableT T, Same<SizeT> I> struct IndexReturnType<T, I> {
  private:
  T t;
  I arg;
  public:
  using type = decltype (t[arg]);
};

template<IndexableT T> struct IndexReturnType<T, SizeT> {
  private:
  T     t;
  SizeT arg;
  public:
  using type = decltype (t[arg]);
};

template<typename I, typename Arg>
concept CanDeduceIndexReturnType = requires ( ) {
  IndexReturnType<I, Arg>::type;
};


template<typename T>
concept Reversible = requires (T t) {
  { t.reverse ( ) } -> Same<void>;
};

template<typename T>
concept SizeableT = Sizeable<T, SizeT>;

template<class T, typename SizeType>
concept FixedSize = Integral<SizeType> && requires (T t) {
  { t.max_size ( ) } -> Same<SizeType>;
};

template<class IndexableType, typename Type, typename Size>
concept FixedSizeIndexable
  = IndexableT<IndexableType> && Sizeable<IndexableType,
                                          Size> && FixedSize<IndexableType,
                                                             Size>;

template<class IndexableType, typename Type>
concept FixedSizeTIndexable = FixedSizeIndexable<IndexableType, Type, SizeT>;

template<class T>
concept FixedSizeT = FixedSize<T, SizeT>;

template<template<typename> class T, typename O>
concept Appendable = requires (T<O> collection, O element) {
  collection.push_back (element);
};

template<template<typename> class T, typename O>
concept Insertable = requires (T<O> collection, O element) {
  collection.insert (element);
};

template<template<typename> class T, typename O>
concept Prependable = requires (T<O> collection, O element) {
  collection.push_front (element);
};
