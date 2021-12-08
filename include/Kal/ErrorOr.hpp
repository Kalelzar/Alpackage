#pragma once

#include <Kal/Concepts/Stream.hpp>

#include <Kal/default.hpp>

#include <forward_list>
#include <ostream>
#include <stdexcept>
#include <string>

#define TRY(FORM)                                                              \
  ({                                                                           \
    auto result = (FORM);                                                      \
    if (result.isEmpty ( )) { return result; }                                 \
    result.get ( );                                                            \
  })

#define TRY_ASSERT(FORM)                                                       \
  ({                                                                           \
    auto result = (FORM);                                                      \
    ASSERT_TRUE (result.isDefined ( ));                                        \
    result.get ( );                                                            \
  })

#define TRY_RC_ASSERT(FORM)                                                    \
  ({                                                                           \
    auto result = (FORM);                                                      \
    if (result.isEmpty ( )) { RC_FAIL (format ("{}", result)); }               \
    result.get ( );                                                            \
  })

#define TRY_WITH(FORM, MSG)                                                    \
  ({                                                                           \
    auto result = (FORM);                                                      \
    if (result.isEmpty ( )) { return result.propagate (MSG); }                 \
    result.get ( );                                                            \
  })

/**
 * @implements{OStreamable}
 */
template<WithDefaultValue T> class ErrorOr {

  private:
  T                              t;
  std::forward_list<std::string> errors;
  bool                           defined;
  public:
  ErrorOr (ErrorOr<T> const& other)
      : t (other.t)
      , errors (other.errors)
      , defined (other.defined) { }

  template<typename U>
    requires (!Same<T, U>)
  ErrorOr (ErrorOr<U> const& other)
      : errors (other.getErrors ( ))
      , defined (other.isDefined ( )) {
    if (defined) {
      errors.push_front (
        "Attempt to propagate an ErrorOr with value of different type");
    }
  }

  constexpr ErrorOr (std::string const& errorMsg) requires (!Ref<T>)
    || ConstRef<T> : t (defaultValue<RemoveRef<T>>)
      , defined (false) {
    errors.push_front (errorMsg);
  }

  constexpr ErrorOr (std::string const& errorMsg) requires Ref<T> &&(
    !ConstRef<T>)
      : t (const_cast<T> (defaultValue<RemoveRef<T>>))
      , defined (false) {
    errors.push_front (errorMsg);
  }


  constexpr ErrorOr (std::string&& errorMsg) requires (!Ref<T>)
    || ConstRef<T> : t (defaultValue<RemoveRef<T>>)
      , defined (false) {
    errors.push_front (std::forward<std::string> (errorMsg));
  }

  constexpr ErrorOr (std::string&& errorMsg) requires Ref<T> &&(!ConstRef<T>)
      : t (const_cast<T> (defaultValue<RemoveRef<T>>))
      , defined (false) {
    errors.push_front (std::forward<std::string> (errorMsg));
  }

  constexpr ErrorOr (T t) : t (t), defined (true) { }

  ErrorOr<T> propagate (std::string const& msg) {
    ErrorOr<T> next (*this);
    next.errors.push_front (msg);
    return next;
  }

  ErrorOr<T>& operator= (T ot) {
    t       = ot;
    defined = true;
    return *this;
  }

  ErrorOr<T>& operator= (ErrorOr<T> const& ot) {
    if (&ot != this) {
      t       = ot.t;
      defined = ot.defined;
      errors  = ot.errors;
    }
    return *this;
  }

  [[nodiscard]] constexpr inline bool isEmpty ( ) const {
    return !isDefined ( );
  }
  [[nodiscard]] constexpr inline bool isDefined ( ) const { return defined; }

  [[nodiscard]] constexpr std::forward_list<std::string> const&
    getErrors ( ) const {
    return errors;
  }

  constexpr inline T getOrElse (T def) const {
    if (isDefined ( )) { return t; }
    return def;
  }

  constexpr inline T getOrDefault ( ) const { return t; }

  constexpr inline   operator bool ( ) const { return defined; }

  constexpr inline T get ( ) const {
    if (isDefined ( )) { return t; }
    exit (1);     // Unrecoverable.
  }


  friend std::ostream&
    operator<< (std::ostream&     out,
                ErrorOr<T> const& instance) requires OStreamable<T> {
    if (instance.isDefined ( )) {
      out << instance.t;
    } else {
      bool front = true;
      for (auto it : instance.errors) {
        if (front) {
          out << "Error: " << it << std::endl;
          front = false;
        } else {
          out << "\tCaused by: " << it << std::endl;
        }
      }
    }
    return out;
  }


  ~ErrorOr ( ) = default;
};

template<typename T>
inline constexpr ErrorOr<T> defaultValue<ErrorOr<T>> = ErrorOr<T> ( );
