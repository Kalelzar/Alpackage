#include <Kal/Concepts/detail/foreach.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>

RC_GTEST_PROP (ForeachConcepts, foreachIterable, (std::vector<int> list)) {
  int b = 0;
  foreach
    <int> (list, [&b] (int a) { b += a; });

  int acc = 0;
  for (int i : list) { acc += i; }

  RC_ASSERT (b == acc);
}

RC_GTEST_PROP (ForeachConcepts, foreachIndexable, (std::array<int, 256> list)) {
  int b = 0;
  foreach
    <int> (256, list.data ( ), [&b] (int a) { b += a; });

  int acc = 0;
  for (int i = 0; i < 256; i++) { acc += list[i]; }

  RC_ASSERT (b == acc);
}
