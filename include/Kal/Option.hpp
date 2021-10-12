#pragma once

#include <Kal/Concepts/Stream.hpp>
#include <Kal/Concepts/TypeConversion.hpp>

#include <Kal/default.hpp>

#include <ostream>
#include <stdexcept>

/**
 * @implements{Optional}
 * @implements{OStreamable}
 */
template<WithDefaultValue T> class Option {

  private:
  T    t;
  bool defined{ };
  public:
  constexpr Option ( ) : t (defaultValue<T>) { }
  constexpr Option (T t) : t (t), defined (true){ };
  constexpr Option (Convertible<T> auto t) : t (to<T> (t)), defined (true){ };

  Option<T>& operator= (T ot) {
    t       = ot;
    defined = true;
    return *this;
  }

  Option<T>& operator= (Option<T> const& ot) {
    if (&ot != this) {
      t       = ot.t;
      defined = ot.defined;
    }
    return *this;
  }

  Option<T>& operator= (Convertible<T> auto ot) {
    t       = to<T> (ot);
    defined = true;
    return *this;
  }

  [[nodiscard]] constexpr inline bool isEmpty ( ) const {
    return !isDefined ( );
  }
  [[nodiscard]] constexpr inline bool isDefined ( ) const { return defined; }

  constexpr inline T                  getOrElse (T def) const {
    if (isDefined ( )) { return t; }
    return def;
  }

  constexpr inline T getOrDefault ( ) const { return t; }

  constexpr inline   operator bool ( ) const { return defined; }

  constexpr inline T get ( ) const {
    if (isDefined ( )) { return t; }
    throw std::runtime_error ("Attempt to access undefined Option.");
  }

  std::ostream& operator<< (std::ostream& out) const requires OStreamable<T> {
    if (isDefined ( )) {
      out << t;
    } else {
      out << "None";
    }
    return out;
  }
};

template<typename T>
inline constexpr Option<T> defaultValue<Option<T>> = Option<T> ( );