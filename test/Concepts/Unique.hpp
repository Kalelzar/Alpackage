#pragma once

#include <gtest/gtest.h>

#include <rapidcheck/gen/Arbitrary.h>
#include <rapidcheck/gtest.h>

struct Unq {
  bool a;
  int  b;
};

namespace rc {
template<> struct Arbitrary<Unq> { static Gen<Unq> arbitrary ( ); };
}     // namespace rc
