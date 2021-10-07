#pragma once

#include <gtest/gtest.h>

#include <ostream>
#include <rapidcheck/gen/Arbitrary.h>
#include <rapidcheck/gtest.h>

struct Unq {
  bool a;
  int  b;
};

static std::ostream& operator<< (std::ostream& o, Unq const& inst) {
  o << (inst.a ? "true " : "false ") << inst.b;
  return o;
}

namespace rc {
template<> struct Arbitrary<Unq> { static Gen<Unq> arbitrary ( ); };
}     // namespace rc
