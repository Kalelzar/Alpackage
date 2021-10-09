#include <Kal/Concepts/detail/mkString.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <set>
#include <sstream>
#include <vector>

TEST (StreamableConcepts, MkStringIterable) {
  std::vector<int> list = {1, 2, 3, 4};
  auto             res  = mkString (list);
  ASSERT_EQ (res, "[ 1 2 3 4 ]");
}

TEST (StreamableConcepts, MkStringIterableEmpty) {
  std::vector<int> list = { };
  auto             res  = mkString (list);
  ASSERT_EQ (res, "[  ]");
}

TEST (StreamableConcepts, MkStringIterableCustomDelimiter) {
  std::vector<int> list = {1, 2, 3, 4};
  auto             res  = mkString (list, "::");
  ASSERT_EQ (res, "[ 1::2::3::4 ]");
}

TEST (StreamableConcepts, MkStringConstIterable) {
  std::set<int> list = {1, 2, 3, 4};
  auto          res  = mkString (list);
  ASSERT_EQ (res, "[ 1 2 3 4 ]");
}

TEST (StreamableConcepts, MkStringConstIterableEmpty) {
  std::set<int> list = { };
  auto          res  = mkString (list);
  ASSERT_EQ (res, "[  ]");
}

TEST (StreamableConcepts, MkStringConstIterableCustomDelimiter) {
  std::set<int> list = {1, 2, 3, 4};
  auto          res  = mkString (list, "::");
  ASSERT_EQ (res, "[ 1::2::3::4 ]");
}

RC_GTEST_PROP (StreamableConcepts, MkString, (int inst)) {
  auto              res = mkString (inst);
  std::stringstream s;
  s << inst;
  ASSERT_EQ (res, s.str ( ));
}
