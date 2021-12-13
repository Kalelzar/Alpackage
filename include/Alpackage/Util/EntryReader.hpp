#pragma once

#include <Alpackage/Util/Logging.hpp>

#include <Kal/Concepts/Stream.hpp>

#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>
#include <Kal/default.hpp>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef __GNUC__
#  include <cxxabi.h>
#endif

template<typename T>
concept Entry = requires (std::istream* u) {
  { T::read (u) } -> Same<ErrorOr<T>>;
};

template<typename T, typename Container = std::vector<T>> class EntryReader {
  private:
  static inline std::string error ( ) {
#ifdef __GNUC__
    const std::type_info& ti     = typeid (T);
    int                   status = 0;
    auto*                 realname
      = abi::__cxa_demangle (ti.name ( ), nullptr, nullptr, &status);
    auto res = format ("Failed to read entry of type: {}", realname);
    free (realname);
    return res;
#else
    return format ("Failed to read entry of type: {}", typeid (Entry).name ( ));
#endif
  }
  public:
  [[nodiscard]] static ErrorOr<Container> parse (std::istream* in) {
    return error ( );
  }

  static ErrorOr<Container> parse (const std::string& path) {
    return error ( );
  }
};

template<Entry T, WithDefaultValue Container> class EntryReader<T, Container> {
  private:
  static inline std::string error ( ) {
#ifdef __GNUC__
    const std::type_info& ti     = typeid (T);
    int                   status = 0;
    auto*                 realname
      = abi::__cxa_demangle (ti.name ( ), nullptr, nullptr, &status);
    auto res = format ("Failed to read entry of type: {}", realname);
    free (realname);
    return res;
#else
    return format ("Failed to read entry of type: {}", typeid (Entry).name ( ));
#endif
  }
  public:
  [[nodiscard]] static ErrorOr<Container> parse (std::istream* in) {
    auto res = defaultValue<Container>;

    while (!in->eof ( ) && in->peek ( ) != std::char_traits<char>::eof ( )) {
      T e = TRY_WITH (T::read (in), error ( ));
      if (in->fail ( )) { return error ( ); }
      res.push_back (e);
    }

    return std::move (res);
  }

  static ErrorOr<Container> parse (const std::string& path) {
    std::ifstream file (path);
    auto          result = parse (&file);
    file.close ( );
    if (result.isEmpty ( )) {
      return result.propagate (format ("Failed to parse file: '{}'", path));
    }
    return result.get ( );
  }
};


template<IStreamable T, WithDefaultValue Container>
  requires WithDefaultValue<T> &&(!Entry<T>) class EntryReader<T, Container> {
    private:
    static ErrorOr<int> error ( ) {
#ifdef __GNUC__
      const std::type_info& ti     = typeid (T);
      int                   status = 0;
      auto*                 realname
        = abi::__cxa_demangle (ti.name ( ), nullptr, nullptr, &status);
      auto res = format ("Failed to read entry of type: {}", realname);
      free (realname);
      return res;
#else
      return format ("Failed to read entry of type: {}", typeid (T).name ( ));
#endif
    }
    public:
    [[nodiscard]] static ErrorOr<Container> parse (std::istream* in) {
      auto res = defaultValue<Container>;

      while (!in->eof ( ) && in->peek ( ) != std::char_traits<char>::eof ( )) {
        T e = defaultValue<T>;
        try {
          *in >> e;
        } catch (std::runtime_error&) { return error ( ); }
        if (in->fail ( )) { return error ( ); }
        res.push_back (e);
      }

      return std::move (res);
    }

    static ErrorOr<Container> parse (const std::string& path) {
      std::ifstream file (path);
      auto          result = parse (&file);
      file.close ( );
      if (result.isEmpty ( )) {
        return result.propagate (format ("Failed to parse file: '{}'", path));
      }
      return result.get ( );
    }
  };
