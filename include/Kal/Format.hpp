#pragma once
#include <Kal/Concepts/Applicators.hpp>

#include <cstdlib>
#include <string>
#include <vector>


template<typename... T> struct FormatArglist;

template<> struct FormatArglist<> {
  static std::vector<std::string> format ( ) {
    std::vector<std::string> v = defaultValue<std::vector<std::string>>;
    return std::move (v);
  }

  static std::vector<std::string> formatOnto (std::vector<std::string>& v) {
    return v;
  }
};

template<typename H, typename... T> struct FormatArglist<H, T...> {
  static std::vector<std::string> format (H arg, T... t) {
    std::vector<std::string> v = defaultValue<std::vector<std::string>>;
    return formatOnto (v, arg, t...);
  }

  static std::vector<std::string>
    formatOnto (std::vector<std::string>& v, H arg, T... t) {
    v.push_back (mkString (arg));
    return FormatArglist<T...>::formatOnto (v, t...);
  }
};

template<typename H> struct FormatArglist<H> {
  static std::vector<std::string> format (H arg) {
    std::vector<std::string> v = defaultValue<std::vector<std::string>>;
    return formatOnto (v, arg);
  }

  static std::vector<std::string> formatOnto (std::vector<std::string>& v,
                                              H                         arg) {
    v.push_back (mkString (arg));
    return v;
  }
};

template<typename... T>
std::string format (std::string const& formatString, T... t) {
  auto              args   = FormatArglist<T...>::format (t...);

  bool              escape = false;
  bool              expect = false;

  std::stringstream s;
  std::stringstream digits;
  int               i = 0;
  for (char c : formatString) {
    if (escape) {
      s << c;
      escape = false;
      continue;
    }
    if (c == '\\') {
      escape = true;
      continue;
    }
    if (c == '{') {
      expect = true;
      continue;
    }
    if (expect) {
      if (c == '}') {
        auto sv = digits.str ( );
        if (sv.empty ( )) {
          s << args[i++];
        } else {
          digits = { };
          auto j = -1;
          j = std::atoi (sv.c_str ( ));     // NOLINT(cert-err34-c): It will
                                            // always be a number.
          s << args[j];
        }
      } else if (isdigit (c)) {
        digits << c;
        continue;
      } else {
        digits = { };
        s << '{' << c;
      }
      expect = false;
      continue;
    }
    s << c;
  }

  return s.str ( );
}
