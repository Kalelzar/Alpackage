#pragma once

#include <Kal/Concepts/Stream.hpp>

#include <Kal/Symbol.hpp>
#include <Kal/UnrecoverableError.hpp>
#include <Kal/default.hpp>

#include <ostream>
#include <stdexcept>

/**
 * @implements{Optional}
 * @implements{OStreamable}
 * @implements{MoveConstructible} if \b T does.
 * @implements{CopyConstructible} if \b T does.
 */
template<typename Tp, typename T = RemoveRef<Tp>> class Option {
  private:
  alignas (T) uint8_t t[sizeof (T)]{ };
  bool defined{false};

  public:
  inline Option ( ) = default;
  Option (T&& value) : defined (true) { new (&t) T (std::forward<T> (value)); }
  Option (T const& value) : defined (true) { new (&t) T (value); }

  Option (Option const&) = default;
  inline Option (Option const& other) requires (!TriviallyConstructible<T>)
      : defined (other.defined) {
    if (other.isDefined ( )) { new (&t) T (other.get ( )); }
  }

  inline Option (Option&& other) noexcept : defined (other.defined) {
    if (other.isDefined ( )) { new (&t) T (other.give ( )); }
  }


  Option& operator= (T const& value) {
    if (defined) { get ( ).~T ( ); }
    defined = true;
    new (&t) T (value);
    return *this;
  }

  Option& operator= (T&& value) {
    if (defined) { get ( ).~T ( ); }
    defined = true;
    new (&t) T (std::forward<T> (value));
    return *this;
  }

  Option& operator= (Option const&) = delete;
  Option& operator= (Option&& t) = delete;

  ~Option ( )                    = default;
  ~Option ( ) requires Destructible<T> {
    if (isDefined ( )) {
      get ( ).~T ( );
      defined = false;
    }
  }


  [[nodiscard]] inline bool isEmpty ( ) const { return !isDefined ( ); }

  [[nodiscard]] inline bool isDefined ( ) const { return defined; }

  [[nodiscard]] inline T    getOrElse (T const& def) const& {
    if (isDefined ( )) { return get ( ); }
    return def;
  }

  [[nodiscard]] inline T getOrElse (T&& def) && {
    if (isDefined ( )) { return get ( ); }
    return def;
  }

  [[nodiscard]] inline          operator bool ( ) const { return defined; }

  [[nodiscard]] inline T const& get ( ) const& {
    if (isDefined ( )) {
      return *__builtin_launder (reinterpret_cast<T const*> (&t));
    }
    exit (2);     // Unrecoverable
  }

  [[nodiscard]] inline T& get ( ) & {
    if (isDefined ( )) {
      return *__builtin_launder (reinterpret_cast<T*> (&t));
    }
    exit (3);     // Unrecoverable
  }

  [[nodiscard]] inline T get ( ) && {
    if (isDefined ( )) { return give ( ); }
    throw UnrecoverableError (
      "({}:{}:)Trying to move 'Option<{}>' but it is not defined.",
      __FILE__,
      __LINE__,
      symbolicate<T> ( ));
  }

  [[nodiscard]] inline T give ( ) requires (MoveConstructible<T>) {
    if (isDefined ( )) {
      T value = std::move (get ( ));
      get ( ).~T ( );
      defined = false;
      return value;
    }
    exit (5);     // Unrecoverable
  }

  template<typename U> inline bool operator== (Option<U> const& other) const {
    return isDefined ( ) == other.isDefined ( )
        && (isEmpty ( ) || get ( ) == other.get ( ));
  }

  template<typename U> inline bool operator== (U const& other) const {
    return isDefined ( ) && get ( ) == other;
  }

  friend std::ostream&
    operator<< (std::ostream&    out,
                Option<T> const& instance) requires OStreamable<T> {
    if (instance.isDefined ( )) {
      out << instance.get ( );
    } else {
      out << "None";
    }
    return out;
  }
};

template<typename T>
inline constexpr Option<T> defaultValue<Option<T>> = Option<T> ( );
