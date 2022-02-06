#pragma once

#include <Kal/FreeLater.hpp>

#include <cxxabi.h>
#include <sstream>
#include <string>

template<typename T> std::string symbolicate ( ) {
#ifdef __GNUC__
  const std::type_info& ti     = typeid (T);
  int                   status = 0;
  auto* realname = abi::__cxa_demangle (ti.name ( ), nullptr, nullptr, &status);
  FreeLater guard ((void**) &realname);
#else
  auto* realname = typeid (T).name ( );
#endif

  return {realname};
}
