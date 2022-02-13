#pragma once

#include <Kal/Concepts/Stream.hpp>
#include <Kal/Concepts/detail/mkString.hpp>

#include <Kal/Option.hpp>
#include <Kal/StringBuilder.hpp>
#include <Kal/Variant.hpp>
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
  Variant<T, StringBuilder> state;

  public:
  ErrorOr (Option<T> const& other) {
    if (other.isEmpty ( )) {
      state = StringBuilder ("Constructed from empty option.", true);
    } else {
      state = other.get ( );
    }
  };

  ErrorOr (Option<T>&& other) {
    if (other.isEmpty ( )) {
      state = StringBuilder ("Constructed from empty option.", true);
    } else {
      state = other.give ( );
    }
  };

  template<typename U>
    requires (!Same<T, U>)
  ErrorOr (Option<U> const& other) {
    if (other.isDefined ( )) {
      state = StringBuilder (
        "Attempt to propagate an ErrorOr with value of different type",
        true);
    } else {
      state = StringBuilder ( );
    }
  }

  ErrorOr (ErrorOr const& other) : state (other.state) { }
  ErrorOr (ErrorOr&& other) noexcept : state (std::move (other.state)){ };

  template<typename U>
    requires (!Same<T, U>)
  ErrorOr (ErrorOr<U> const& other) {
    if (other.isDefined ( )) {
      state = StringBuilder (
        "Attempt to propagate an ErrorOr with value of different type",
        true);
    } else {
      state = StringBuilder (other.getErrors ( ), true);
    }
  }

  ErrorOr& operator= (ErrorOr const&) = delete;
  ErrorOr& operator= (ErrorOr&&) = delete;

  ErrorOr (std::string const& errorMsg)
      : state (StringBuilder (errorMsg, true)) { }


  ErrorOr (std::string&& errorMsg) : state (StringBuilder (errorMsg, true)) { }

  ErrorOr (T const& t) : state (t) { }
  ErrorOr (T&& t) : state (std::move (t)) { }

  inline ErrorOr<T> propagate (std::string const& msg) {
    if (state.template is<StringBuilder> ( )) {
      state.template get<StringBuilder> ( ).putrf ("{}\n\tCaused by: ", msg);
    } else if (state.template is<T> ( )) {
      return format ("Attempt to propagate '{}' a non-error instance", msg);
    } else {
      state = StringBuilder (msg, true);
    }
    return *this;
  }

  inline ErrorOr<T> propagate (std::string&& msg) {
    if (state.template is<StringBuilder> ( )) {
      state.template get<StringBuilder> ( ).putrf (
        "{}\n\tCaused by: ",
        std::forward<std::string> (msg));
    } else if (state.template is<T> ( )) {
      return format ("Attempt to propagate '{}' a non-error instance",
                     std::forward<std::string> (msg));
    } else {
      state = StringBuilder (std::forward<std::string> (msg), true);
    }
    return *this;
  }

  [[nodiscard]] inline bool isEmpty ( ) const { return !isDefined ( ); }
  [[nodiscard]] inline bool isDefined ( ) const {
    return state.template is<T> ( );
  }

  [[nodiscard]] inline std::string getErrors ( ) const {
    if (state.template is<StringBuilder> ( )) {
      return state.template get<StringBuilder> ( ).getr ( );
    } else if (state.template is<T> ( )) {
      throw UnrecoverableError ("Attempt to output errors for an instance of "
                                "ErrorOr<{}> that is defined.",
                                symbolicate<T> ( ));
    } else {
      return "Accessed a completely undefined ErrorOr instance. This should "
             "never happen.";
    }
  }


  inline T getOrElse (T def) const {
    if (state.template is<T> ( )) { return state.template get<T> ( ); }
    return def;
  }

  inline   operator bool ( ) const { return isDefined ( ); }

  inline T get ( ) const {
    if (state.template is<T> ( )) { return state.template get<T> ( ); }
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
