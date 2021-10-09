#include <Alpackage/Package.hpp>

#include <gtest/gtest.h>

#include <cstdlib>
#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <sstream>
#include <stdexcept>


TEST (Package, Equals) {
  Alpackage::Package a ("pkg", "pm1", true);
  Alpackage::Package b ("pkg", "pm1", false);
  ASSERT_EQ (a, b);
  Alpackage::Package c ("pkg", "pm2", true);
  ASSERT_NE (a, c);
  ASSERT_NE (b, c);
  Alpackage::Package d ("pkg2", "pm", false);
  ASSERT_NE (a, d);
  ASSERT_NE (b, d);
}

TEST (Package, OStream) {
  Alpackage::Package a ("pkg", "pm1", true);
  std::stringstream  s;
  s << a;
  ASSERT_EQ (s.str ( ), "{ \"name\": \"pkg\", \"packageManager\": \"pm1\" }");
}
