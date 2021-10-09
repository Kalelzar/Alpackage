#pragma once

#include <Kal/Option.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/gen/Arbitrary.h>
#include <rapidcheck/gtest.h>

namespace rc {
template<typename T> struct Arbitrary<Option<T>> {
  static Gen<Option<T>> arbitrary ( );
};
}     // namespace rc
