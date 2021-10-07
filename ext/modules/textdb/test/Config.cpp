#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <sstream>

#include "../src/Config.hpp"

TEST (TextDBConfig, Parse) {
  std::stringstream s;
  s << "pkg;xbps;1.0.0;A package";
  auto res = readConfig (&s);
  ASSERT_TRUE (res.size ( ) == 1);
  ASSERT_EQ (res[0].name, "pkg");
  ASSERT_EQ (res[0].pm, "xbps");
  ASSERT_TRUE (res[0].installed);
}
