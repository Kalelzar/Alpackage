#pragma once

#include <Alpackage/Util/Logging.hpp>
#include <Kal/Concepts/Stream.hpp>
#include <Kal/default.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef __GNUC__
#  include <cxxabi.h>
#endif

template<IStreamable Entry, WithDefaultValue Container = std::vector<Entry>>
  requires WithDefaultValue<Entry>
class EntryReader {
  public:
  static Container parse (std::istream* in) {
    auto res = defaultValue<Container>;

    while (!in->eof ( )) {
      Entry e = defaultValue<Entry>;
      *in >> e;
      if (in->fail ( )) {
#ifdef __GNUC__
        const std::type_info& ti = typeid (Entry);
        int                   status;
        char* realname = abi::__cxa_demangle (ti.name ( ), 0, 0, &status);
        Log::error ("Failed to read entry of type: %s", realname);
        Log::flush ( );
        free (realname);
#else
        Log::error ("Failed to read entry of type: %s",
                    typeid (Entry).name ( ));
#endif

        throw std::runtime_error ("Failed to read entry.");
      }
      res.push_back (e);
    }

    return std::move (res);
  }

  static Container parse (std::string path) {
    std::ifstream file (path);
    auto          result = parse (&file);
    file.close ( );
    return std::move (result);
  }
};
