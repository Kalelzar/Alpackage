#pragma once
#include <Kal/Concepts/Construction.hpp>
#include <Kal/Concepts/Type.hpp>

template<typename T, typename U> struct Convert { static U to (T t); };

template<typename U, typename T> U to (T t) { return Convert<T, U>::to (t); }

template<typename T, typename U>
concept Convertible = requires (T t) {
  { to<U> (t) } -> Same<U>;
};


template<typename T, typename U>
concept CanConvertTo = requires (T t) {
  { t.template to<U> ( ) } -> Same<U>;
};

// BUG: This doesn't seem to work.
template<typename U, CanConvertTo<U> T> struct Convert<T, U> {
  static U to (T t) { return t.template to<U> ( ); }
};

template<typename U, typename T>
  requires ConstructibleFrom<U, T>
struct Convert<T, U> {
  static U to (T t) { return U (t); }
};

template<typename T> struct Convert<T, T> {
  static T to (T t) { return t; }
};

template<typename U, typename T>
  requires (!Convertible<T, U>)
struct Convert<T, U> {
  static U to (T t) {
    static_assert (Convertible<T, U>, "No availabl conversion from T to U");
  }
};

template<typename T, typename U>
concept Interchangeable = Convertible<T, U> && Convertible<U, T>;
