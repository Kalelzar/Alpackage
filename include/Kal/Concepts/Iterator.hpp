#pragma once

#include <Kal/Concepts/Type.hpp>

template<typename Begin, typename End>
concept IteratorPair = requires (Begin begin, End end) {
  *begin;
  { begin != end } -> Same<bool>;
  ++begin;
};

template<typename T>
concept Iterable = requires (T t) {
  { t.begin ( ) } -> IteratorPair<decltype (t.end ( ))>;
};

template<typename T>
concept ConstIterable = requires (T t) {
  { t.cbegin ( ) } -> IteratorPair<decltype (t.cend ( ))>;
};

template<typename T>
concept AnyIterable = ConstIterable<T> || Iterable<T>;

template<typename T, typename U>
concept IterableWith = Iterable<T> && requires (T t) {
  { *t.begin ( ) } -> Same<U&>;
};

template<typename T, typename U>
concept IterableWithConst = (NotConst<U> && Iterable<T> && requires (T t) {
  { *t.begin ( ) } -> Same<const U&>;
});

template<typename T, typename U>
concept ConstIterableWith = (ConstIterable<T> && requires (T t) {
  { *t.cbegin ( ) } -> Same<AddConst<U>&>;
});

template<typename T, typename U>
concept AnyIterableWith
  = ConstIterableWith<T, U> || IterableWithConst<T, U> || IterableWith<T, U>;

template<typename T> struct IteratesWith;

template<typename T>
  requires Iterable<T> && ConstIterable<T>
struct IteratesWith<T> {
  private:
  T t;
  public:
  using type  = decltype (*t.begin ( ));
  using ctype = decltype (*t.cbegin ( ));
};

template<Iterable T> struct IteratesWith<T> {
  private:
  T t;
  public:
  using type = decltype (*t.begin ( ));
};


template<ConstIterable T> struct IteratesWith<T> {
  private:
  T t;
  public:
  using type = decltype (*t.cbegin ( ));
};
