#include <Kal/Concepts/detail/aggregate.hpp>
#include <Kal/Concepts/detail/filter.hpp>

#include <gtest/gtest.h>

#include <array>
#include <forward_list>
#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>

RC_GTEST_PROP (FilterConcepts,
               filterAppendableIterable,
               (std::vector<int> list)) {
  auto filtered = filter (list, [] (int a) -> bool { return a == 0; });
  auto res      = aggregate (filtered, [] (int a, int b) { return a + b; });


  RC_ASSERT (res == 0);
}

RC_GTEST_PROP (FilterConcepts, filterInsertableIterable, (std::set<int> list)) {
  auto filtered = filter (list, [] (int a) -> bool { return a == 0; });
  auto res      = aggregate (filtered, [] (int a, int b) { return a + b; });


  RC_ASSERT (res == 0);
}

RC_GTEST_PROP (FilterConcepts,
               filterPrependableReversableIterable,
               (std::forward_list<int> list)) {
  auto filtered = filter (list, [] (int a) -> bool { return a == 0; });
  auto res      = aggregate (filtered, [] (int a, int b) { return a + b; });


  RC_ASSERT (res == 0);
}

RC_GTEST_PROP (FilterConcepts, filterPlainArray, (std::array<int, 256> list)) {
  int* arr = (int*) calloc (256, sizeof (int));

  filter (256, list.data ( ), arr, [] (int a) { return a == 0; });
  auto res = aggregate (arr, 256, [] (int a, int b) { return a + b; });

  free (arr);

  RC_ASSERT (res == 0);
}
