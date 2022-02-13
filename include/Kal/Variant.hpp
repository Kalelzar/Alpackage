#pragma once

#include <Kal/Concepts/Construction.hpp>
#include <Kal/Concepts/Type.hpp>

#include <Kal/Format.hpp>
#include <Kal/Option.hpp>
#include <Kal/Symbol.hpp>
#include <Kal/UnrecoverableError.hpp>

#include <functional>
#include <string>


template<typename First, typename... Rest> class Variant {
  private:
  bool defined{ };
  alignas (First) uint8_t value[sizeof (First)]{ };
  Variant<Rest...> base{ };
  public:
  template<typename T>
    requires (!Same<T, First>)
  constexpr Variant (T t) : base (t) { }

  constexpr Variant (First f) : defined (true) { new (&value) First (f); }

  constexpr Variant ( ) = default;


  constexpr Variant (Variant const& other) : defined (other.defined) {
    if (&other != this) {
      base = other.base;
      if (defined) new (&value) First (other.get<First> ( ));
    }
  }

  constexpr Variant (Variant&& other) noexcept : defined (other.defined) {
    if (&other != this) {
      base = std::move (other.base);
      if (defined) new (&value) First (other.give<First> ( ));
    }
  }

  Variant& operator= (Variant const& other) {
    if (&other != this) {
      if (defined) { get<First> ( ).~First ( ); }
      if (other.defined) { new (&value) First (other.get<First> ( )); }
      base    = other.base;
      defined = other.defined;
    }
    return *this;
  }

  Variant& operator= (Variant&& other) noexcept {
    if (&other != this) {
      if (defined) { get<First> ( ).~First ( ); }
      if (other.defined) { new (&value) First (other.give<First> ( )); }
      base    = other.base;
      defined = other.defined;
    }
    return *this;
  }

  template<typename Ret, typename Match>
  Option<Ret> operator( ) (std::function<Ret (Match const&)> f) {
    if (is<Match> ( )) { return f (get<Match> ( )); }
    return { };
  }

  template<typename Ret, typename Match>
  Option<Ret> operator( ) (Ret f (Match const&)) {
    if (is<Match> ( )) { return f (get<Match> ( )); }
    return { };
  }

  template<typename Ret, typename Match>
  Option<Ret> visit (std::function<Ret (Match const&)> f) {
    if (is<Match> ( )) { return f (get<Match> ( )); }
    return { };
  }

  template<typename Ret, typename Match>
  Option<Ret> visit (Ret f (Match const&)) {
    if (is<Match> ( )) { return f (get<Match> ( )); }
    return { };
  }

  template<typename T> T const& get ( ) const& requires (Same<T, First>) {
    if (!defined) {
      throw UnrecoverableError (
        "({}:{}:)Trying to get value for '{}' but it is not defined.",
        __FILE__,
        __LINE__,
        symbolicate<T> ( ));
    }
    return *__builtin_launder (reinterpret_cast<T const*> (&value));
  }

  template<typename T> T& get ( ) & requires (Same<T, First>) {
    if (!defined) {
      throw UnrecoverableError (
        "({}:{}:)Trying to get value for '{}' but it is not defined.",
        __FILE__,
        __LINE__,
        symbolicate<T> ( ));
    }
    return *__builtin_launder (reinterpret_cast<T*> (&value));
  }

  template<typename T>
  T give ( ) requires (Same<T, First>&& MoveConstructible<T>) {
    if (!defined) {
      throw UnrecoverableError (
        "({}:{}:)Trying to get value for '{}' but it is not defined.",
        __FILE__,
        __LINE__,
        symbolicate<T> ( ));
    }     // Unrecoverable
    T val = std::move (get<T> ( ));
    get<T> ( ).~T ( );
    defined = false;
    return val;
  }


  template<typename T> T const& get ( ) const& requires (!Same<T, First>) {
    return base.template get<T> ( );
  }

  template<typename T> T& get ( ) & requires (!Same<T, First>) {
    return base.template get<T> ( );
  }

  template<typename T>
  T give ( ) requires (!Same<T, First> && MoveConstructible<T>) {
    return std::move (base.template give<T> ( ));
  }

  template<typename T> [[nodiscard]] bool is ( ) const requires Same<T, First> {
    return defined;
  }

  template<typename T>
  [[nodiscard]] bool is ( ) const requires (!Same<T, First>) {
    return base.template is<T> ( );
  }

  [[nodiscard]] bool isEmpty ( ) const {
    if (defined) { return false; }
    return base.isEmpty ( );
  }

  [[nodiscard]] bool isDefined ( ) const { return !base.isEmpty ( ); }


  ~Variant ( ) = default;
  ~Variant ( ) requires Destructible<First> {
    if (defined) {
      get<First> ( ).~First ( );
      defined = false;
    }
  }
};

template<typename First> class Variant<First> {
  private:
  bool defined{ };
  alignas (First) uint8_t value[sizeof (First)]{ };
  public:
  constexpr Variant (First f) : defined (true) { new (&value) First (f); }

  constexpr Variant (Variant const& other) : defined (other.defined) {
    if (defined) new (&value) First (other.get<First> ( ));
  }

  constexpr Variant (Variant&& other) noexcept : defined (other.defined) {
    if (defined) new (&value) First (other.give<First> ( ));
  }

  Variant& operator= (Variant const& other) {
    if (&other != this) {
      if (defined) { get<First> ( ).~First ( ); }
      defined = other.defined;
      if (other.defined) { new (&value) First (other.get<First> ( )); }
    }
    return *this;
  }

  Variant& operator= (Variant&& other) noexcept {
    if (&other != this) {
      if (defined) { get<First> ( ).~First ( ); }
      defined = other.defined;
      if (other.defined) {
        new (&value) First (std::move (other.give<First> ( )));
      }
    }
    return *this;
  }

  constexpr Variant ( ) = default;

  template<typename T> T const& get ( ) const& {
    static_assert (Same<T, First>, "T is not a part of this variant.");
    if (!defined) {
      throw UnrecoverableError (
        "({}:{}:) Trying to get value for '{}' but it is not defined.",
        __FILE__,
        __LINE__,
        symbolicate<T> ( ));
    }     // Unrecoverable
    return *__builtin_launder (reinterpret_cast<T const*> (&value));
  }

  template<typename T> T& get ( ) & {
    static_assert (Same<T, First>, "T is not a part of this variant.");
    if (!defined) {
      throw UnrecoverableError (
        "({}:{}:)Trying to get value for '{}' but it is not defined.",
        __FILE__,
        __LINE__,
        symbolicate<T> ( ));
    }     // Unrecoverable
    return *__builtin_launder (reinterpret_cast<T*> (&value));
  }

  template<typename T> T give ( ) requires MoveConstructible<T> {
    static_assert (Same<T, First>, "T is not a part of this variant.");
    if (!defined) {
      throw UnrecoverableError (
        "({}:{}:)Trying to get value for '{}' but it is not defined.",
        __FILE__,
        __LINE__,
        symbolicate<T> ( ));
    }     // Unrecoverable
    T val = std::move (get<T> ( ));
    get<T> ( ).~T ( );
    defined = false;
    return val;
  }

  template<typename T> [[nodiscard]] bool is ( ) const {
    static_assert (Same<T, First>, "T is not a part of this variant.");
    return defined;
  }

  [[nodiscard]] bool isEmpty ( ) const { return !defined; }

  ~Variant ( ) = default;
  ~Variant ( ) requires Destructible<First> {
    if (defined) {
      get<First> ( ).~First ( );
      defined = false;
    }
  }

  template<typename Ret, typename Match>
  Option<Ret> operator( ) (std::function<Ret (Match const&)> f) {
    if (is<Match> ( )) { return f (get<Match> ( )); }
    return { };
  }

  template<typename Ret, typename Match>
  Option<Ret> operator( ) (Ret f (Match const&)) {
    if (is<Match> ( )) { return f (get<Match> ( )); }
    return { };
  }

  template<typename Ret, typename Match>
  Option<Ret> visit (std::function<Ret (Match const&)> f) {
    if (is<Match> ( )) { return f (get<Match> ( )); }
    return { };
  }

  template<typename Ret, typename Match>
  Option<Ret> visit (Ret f (Match const&)) {
    if (is<Match> ( )) { return f (get<Match> ( )); }
    return { };
  }
};
