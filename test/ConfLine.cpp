#include <Alpackage/Util/ConfLine.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <sstream>

TEST (ConfLine, ValidConfLine) {
  std::stringstream ss ("name = value");

  ConfLine          c = TRY_ASSERT (ConfLine::read (&ss));

  ASSERT_EQ ("name", c.name);
  ASSERT_EQ ("value", c.value);
}
