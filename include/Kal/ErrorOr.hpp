#pragma once

#include <Kal/Concepts/Stream.hpp>
#include <Kal/Concepts/detail/mkString.hpp>

#include <Kal/Option.hpp>
#include <Kal/default.hpp>

#include <forward_list>
#include <ostream>
#include <stdexcept>
#include <string>

#define TRY(FORM)                                                              \
  ({                                                                           \
    auto result = std::move (FORM);                                            \
    if (result.isEmpty ( )) { return result; }                                 \
    result.get ( );                                                            \
  })

#define TRY_WITH(FORM, MSG)                                                    \
  ({                                                                           \
    auto result = std::move (FORM);                                            \
    if (result.isEmpty ( )) { return result.propagate (MSG); }                 \
    result.get ( );                                                            \
  })

#define TRY_OR_ELSE(FORM, DEFAULT) (FORM).getOrElse (DEFAULT)


#define TRY_ASSERT(FORM)                                                       \
  ({                                                                           \
    auto result = std::move (FORM);                                            \
    ASSERT_TRUE (result.isDefined ( )) << result << std::endl;                 \
    result.get ( );                                                            \
  })

#define TRY_RC_ASSERT(FORM)                                                    \
  ({                                                                           \
    auto result = std::move (FORM);                                            \
    if (result.isEmpty ( )) { RC_FAIL (format ("{}", result)); }               \
    result.get ( );                                                            \
  })

/**
 * @implements{OStreamable}
 */
template<typename Tp, typename T = RemoveRef<Tp>> class ErrorOr {
  private:
  Option<T>                      t;
  std::forward_list<std::string> errors;
  public:
  ErrorOr (Option<T> const& other) : t (other) {
    if (other.isEmpty ( )) {
      errors.push_front ("Constructed from empty option.");
    }
  };

  ErrorOr (Option<T>&& other) : t (std::forward<Option<T>> (other)) {
    if (t.isEmpty ( )) { errors.push_front ("Constructed from empty option."); }
  };

  template<typename U>
    requires (!Same<T, U>)
  ErrorOr (Option<U> const& other) : t (other) {
    if (t.isDefined ( )) {
      errors.push_front (
        "Attempt to propagate an ErrorOr with value of different type");
    }
  }

  ErrorOr (ErrorOr const& other) : t (other.t), errors (other.errors) { }
  ErrorOr (ErrorOr&& other) noexcept
      : t (std::move (other.t))
      , errors (std::move (other.errors)){ };

  template<typename U>
    requires (!Same<T, U>)
  ErrorOr (ErrorOr<U> const& other) : errors (other.getErrors ( )), t ( ) {
    if (t.isDefined ( )) {
      errors.push_front (
        "Attempt to propagate an ErrorOr with value of different type");
    }
  }

  ErrorOr& operator= (ErrorOr const&) = delete;
  ErrorOr& operator= (ErrorOr&&) = delete;

  ErrorOr (std::string const& errorMsg) : t ( ) {
    errors.push_front (errorMsg);
  }


  ErrorOr (std::string&& errorMsg) : t ( ) {
    errors.push_front (std::forward<std::string> (errorMsg));
  }

  ErrorOr (T const& t) : t (t) { }
  ErrorOr (T&& t) : t (std::move (t)) { }

  ErrorOr<T> propagate (std::string const& msg) {
    ErrorOr<T> next (*this);
    next.errors.push_front (msg);
    return next;
  }

  [[nodiscard]] inline bool isEmpty ( ) const { return !isDefined ( ); }
  [[nodiscard]] inline bool isDefined ( ) const { return t.isDefined ( ); }

  [[nodiscard]] std::forward_list<std::string> const& getErrors ( ) const {
    return errors;
  }

  inline T getOrElse (T def) const {
    if (isDefined ( )) { return t.get ( ); }
    return def;
  }

  inline   operator bool ( ) const { return t.isDefined ( ); }

  inline T get ( ) const {
    if (isDefined ( )) { return t.get ( ); }
    exit (1);     // Unrecoverable.
  }


  ~ErrorOr ( ) = default;
};

/**
 * @implements{OStreamable}
 */
template<> class ErrorOr<void> {
  private:
  bool                           defined{false};
  std::forward_list<std::string> errors;
  public:
  ErrorOr (ErrorOr const& other)
      : defined (other.defined)
      , errors (other.errors) { }
  ErrorOr (ErrorOr&& other) noexcept
      : defined (other.defined)
      , errors (std::move (other.errors)){ };

  template<typename U>
    requires (!Same<void, U>)
  ErrorOr (ErrorOr<U> const& other) : errors (other.getErrors ( )) {
    if (other.isDefined ( )) {
      errors.push_front (
        "Attempt to propagate an ErrorOr with value of different type");
    }
  }

  ErrorOr& operator= (ErrorOr const&) = delete;
  ErrorOr& operator= (ErrorOr&&) = delete;

  ErrorOr (std::string const& errorMsg) { errors.push_front (errorMsg); }


  ErrorOr (std::string&& errorMsg) {
    errors.push_front (std::forward<std::string> (errorMsg));
  }

  ErrorOr ( ) : defined (true) { }

  ErrorOr propagate (std::string const& msg) {
    ErrorOr next (*this);
    next.errors.push_front (msg);
    return next;
  }

  [[nodiscard]] inline bool isEmpty ( ) const { return !isDefined ( ); }
  [[nodiscard]] inline bool isDefined ( ) const { return defined; }

  [[nodiscard]] inline std::forward_list<std::string> const&
    getErrors ( ) const {
    return errors;
  }

  inline void get ( ) { }


  inline      operator bool ( ) const { return defined; }


  ~ErrorOr ( ) = default;
};

struct ErrorMsg {
  std::string data;
};

/**
 * @implements{OStreamable}
 */
template<> class ErrorOr<std::string> {
  private:
  Option<std::string>            t;
  std::forward_list<std::string> errors;
  public:
  ErrorOr (ErrorOr const& other) = default;
  ErrorOr (ErrorOr&& other) noexcept
      : t (std::move (other.t))
      , errors (std::move (other.errors)){ };


  template<typename U>
    requires (!Same<std::string, U>)
  ErrorOr (ErrorOr<U> const& other) : errors (other.getErrors ( )) {
    if (t.isDefined ( )) {
      errors.push_front (
        "Attempt to propagate an ErrorOr with value of different type");
    }
  }

  ErrorOr& operator= (ErrorOr const&) = delete;
  ErrorOr& operator= (ErrorOr&&) = delete;

  ErrorOr (ErrorMsg const& errorMsg) { errors.push_front (errorMsg.data); }


  ErrorOr (ErrorMsg&& errorMsg) {
    errors.push_front (std::move (errorMsg.data));
  }

  ErrorOr (std::string&& t) : t (std::move (t)) { }

  ErrorOr<std::string> propagate (std::string const& msg) {
    ErrorOr<std::string> next (*this);
    next.errors.push_front (msg);
    return next;
  }

  [[nodiscard]] inline bool isEmpty ( ) const { return !isDefined ( ); }
  [[nodiscard]] inline bool isDefined ( ) const { return t.isDefined ( ); }

  [[nodiscard]] std::forward_list<std::string> const& getErrors ( ) const {
    return errors;
  }

  [[nodiscard]] inline std::string getOrElse (std::string def) const {
    if (isDefined ( )) { return t.get ( ); }
    return def;
  }

  inline operator bool ( ) const { return t.isDefined ( ); }

  [[nodiscard]] inline std::string get ( ) const {
    if (isDefined ( )) { return t.get ( ); }
    exit (1);     // Unrecoverable.
  }

  std::ostream& operator<< (std::ostream& out) {
    if (isDefined ( )) {
      out << "<void>";
    } else {
      bool front = true;
      for (auto it : getErrors ( )) {
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

template<OStreamable T>
std::ostream& operator<< (std::ostream& out, ErrorOr<T> const& instance) {
  if (instance.isDefined ( )) {
    out << mkString (instance.get ( ));
  } else {
    bool front = true;
    for (auto it : instance.getErrors ( )) {
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
