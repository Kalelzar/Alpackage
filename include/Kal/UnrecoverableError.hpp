#pragma once

#include <Kal/Format.hpp>

#include <exception>

class UnrecoverableError : public std::exception {
  private:
  std::string msg;
  public:
  template<typename... Ts>
  explicit UnrecoverableError (std::string const& fmt, Ts... ts)
      : msg (format (fmt, ts...)) { }
  [[nodiscard]] const char* what ( ) const noexcept override {
    return msg.c_str ( );
  }
};
