#include <Kal/Concepts/detail/aggregate.hpp>

#include <gtest/gtest.h>

#include <array>
#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>

RC_GTEST_PROP (AggregateConcepts, aggregateIterable, (std::vector<int> list)) {
  auto res = aggregate (list, [] (int a, int b) { return a + b; });

  int  acc = 0;
  for (int i : list) { acc += i; }

  RC_ASSERT (res == acc);
}

RC_GTEST_PROP (AggregateConcepts,
               aggregateIndexable,
               (std::array<int, 256> list)) {
  auto res
    = aggregate (list.data ( ), 256, [] (int a, int b) { return a + b; });

  int acc = 0;
  for (int i = 0; i < 256; i++) { acc += list[i]; }

  RC_ASSERT (res == acc);
}

RC_GTEST_PROP (AggregateConcepts,
               foldLeftIterable,
               (int def, std::vector<int> list)) {
  auto res = foldLeft (def, list, [] (int a, int b) { return a + b; });

  int  acc = def;
  for (int i : list) { acc += i; }

  RC_ASSERT (res == acc);
}
