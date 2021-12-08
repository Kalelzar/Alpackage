#include <Kal/View/detail/mkString.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <string>
#include <vector>


RC_GTEST_PROP (Ranges, mkStringDefaultDelimeter, (std::vector<int> i)) {
  auto res      = std::views::all (i) | views::mkString ( );
  auto expected = mkString (i);
  RC_ASSERT (res == expected);
}

RC_GTEST_PROP (Ranges,
               mkStringAnyDelimeter,
               (std::vector<int> i, std::string s)) {
  std::string_view delim (s);
  auto             res      = std::views::all (i) | views::mkString (delim);
  auto             expected = mkString (i, delim);
  RC_ASSERT (res == expected);
}
