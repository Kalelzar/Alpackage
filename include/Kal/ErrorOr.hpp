#pragma once

#include <Kal/Concepts/Stream.hpp>
#include <Kal/Concepts/detail/mkString.hpp>

#include "Kal/StringBuilder.hpp"
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
  Option<T>     t;
  StringBuilder errors;
  public:
  ErrorOr (Option<T> const& other) : t (other) {
    if (other.isEmpty ( )) { errors.putr ("Constructed from empty option."); }
  };

  ErrorOr (Option<T>&& other) : t (std::forward<Option<T>> (other)) {
    if (t.isEmpty ( )) { errors.putr ("Constructed from empty option."); }
  };

  template<typename U>
    requires (!Same<T, U>)
  ErrorOr (Option<U> const& other) : t (other) {
    if (t.isDefined ( )) {
      errors.putr (
        "Attempt to propagate an ErrorOr with value of different type");
    }
  }

  ErrorOr (ErrorOr const& other)
      : t (other.t)
      , errors (other.errors.get ( )) { }
  ErrorOr (ErrorOr&& other) noexcept
      : t (std::move (other.t))
      , errors (std::move (other.errors)){ };

  template<typename U>
    requires (!Same<T, U>)
  ErrorOr (ErrorOr<U> const& other)
      : errors (other.getErrors ( ), true)
      , t ( ) {
    if (t.isDefined ( )) {
      errors.putr (
        "Attempt to propagate an ErrorOr with value of different type");
    }
  }

  ErrorOr& operator= (ErrorOr const&) = delete;
  ErrorOr& operator= (ErrorOr&&) = delete;

  ErrorOr (std::string const& errorMsg) : t ( ), errors (errorMsg, true) { }


  ErrorOr (std::string&& errorMsg)
      : t ( )
      , errors (std::forward<std::string> (errorMsg), true) { }

  ErrorOr (T const& t) : t (t) { }
  ErrorOr (T&& t) : t (std::move (t)) { }

  inline ErrorOr<T> propagate (std::string const& msg) {
    errors.putrf ("{}\n\tCaused by: ", msg);
    return *this;
  }

  inline ErrorOr<T> propagate (std::string&& msg) {
    errors.putrf ("{}\n\tCaused by: ", std::forward<std::string> (msg));
    return *this;
  }

  [[nodiscard]] inline bool isEmpty ( ) const { return !isDefined ( ); }
  [[nodiscard]] inline bool isDefined ( ) const { return t.isDefined ( ); }

  [[nodiscard]] inline std::string getErrors ( ) const {
    return errors.getr ( );
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
  bool          defined{false};
  StringBuilder errors;
  public:
  ErrorOr (ErrorOr const& other)
      : defined (other.defined)
      , errors (other.errors.get ( )) { }
  ErrorOr (ErrorOr&& other) noexcept
      : defined (other.defined)
      , errors (std::move (other.errors)){ };

  template<typename U>
    requires (!Same<void, U>)
  ErrorOr (ErrorOr<U> const& other) : errors (other.getErrors ( ), true) {
    if (other.isDefined ( )) {
      errors.putr (
        "Attempt to propagate an ErrorOr with value of different type");
    }
  }

  ErrorOr& operator= (ErrorOr const&) = delete;
  ErrorOr& operator= (ErrorOr&&) = delete;

  ErrorOr (std::string const& errorMsg) : errors (errorMsg, true) { }


  ErrorOr (std::string&& errorMsg)
      : errors (std::forward<std::string> (errorMsg), true) { }

  ErrorOr ( ) : defined (true) { }

  inline ErrorOr propagate (std::string const& msg) {
    errors.putrf ("{}\n\tCaused by: ", msg);
    return *this;
  }

  inline ErrorOr propagate (std::string&& msg) {
    errors.putrf ("{}\n\tCaused by: ", std::forward<std::string> (msg));
    return *this;
  }

  [[nodiscard]] inline bool        isEmpty ( ) const { return !isDefined ( ); }
  [[nodiscard]] inline bool        isDefined ( ) const { return defined; }

  [[nodiscard]] inline std::string getErrors ( ) const {
    return errors.getr ( );
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
  Option<std::string> t;
  StringBuilder       errors;
  public:
  ErrorOr (ErrorOr const& other)
      : t (other.t)
      , errors (other.errors.get ( )){ };
  ErrorOr (ErrorOr&& other) noexcept
      : t (std::move (other.t))
      , errors (std::move (other.errors)){ };


  template<typename U>
    requires (!Same<std::string, U>)
  ErrorOr (ErrorOr<U> const& other) : errors (other.getErrors ( ), true) {
    if (other.isDefined ( )) {
      errors.putr (
        "Attempt to propagate an ErrorOr with value of different type");
    }
  }

  ErrorOr& operator= (ErrorOr const&) = delete;
  ErrorOr& operator= (ErrorOr&&) = delete;

  ErrorOr (ErrorMsg const& errorMsg) : errors (errorMsg.data, true) { }


  ErrorOr (ErrorMsg&& errorMsg) : errors (errorMsg.data, true) { }

  ErrorOr (std::string&& t) : t (std::move (t)) { }

  inline ErrorOr<std::string> propagate (std::string const& msg) {
    errors.putrf ("{}\n\tCaused by: ", msg);
    return *this;
  }

  inline ErrorOr<std::string> propagate (std::string&& msg) {
    errors.putrf ("{}\n\tCaused by: ", std::forward<std::string> (msg));
    return *this;
  }

  [[nodiscard]] inline bool isEmpty ( ) const { return !isDefined ( ); }
  [[nodiscard]] inline bool isDefined ( ) const { return t.isDefined ( ); }

  [[nodiscard]] inline std::string getErrors ( ) const {
    return errors.getr ( );
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

  std::ostream& operator<< (std::ostream& out) const {
    if (isDefined ( )) {
      out << "<void>";
    } else {
      // bool front = true;
      // for (auto it : instance.getErrors ( )) {
      //   if (front) {
      //     out << "Error: " << it << std::endl;
      //     front = false;
      //   } else {
      //     out << "\tCaused by: " << it << std::endl;
      //   }
      // }
      out << "Error: " << getErrors ( );
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
    // bool front = true;
    // for (auto it : instance.getErrors ( )) {
    //   if (front) {
    //     out << "Error: " << it << std::endl;
    //     front = false;
    //   } else {
    //     out << "\tCaused by: " << it << std::endl;
    //   }
    // }
    out << "Error: " << instance.getErrors ( );
  }
  return out;
}
