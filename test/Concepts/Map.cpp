#include <Kal/Concepts/detail/aggregate.hpp>
#include <Kal/Concepts/detail/map.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>

RC_GTEST_PROP (MapConcepts, mapAppendableIterable, (std::vector<int> list)) {
  auto mapped = map<int> (list, [] (int a) -> int { return 0; });
  auto res    = aggregate (mapped, [] (int a, int b) { return a + b; });


  RC_ASSERT (res == 0);
}

RC_GTEST_PROP (MapConcepts, mapInsertableIterable, (std::set<int> list)) {
  auto mapped = map<int> (list, [] (int a) -> int { return 0; });
  auto res    = aggregate (mapped, [] (int a, int b) { return a + b; });


  RC_ASSERT (res == 0);
}

RC_GTEST_PROP (MapConcepts,
               mapPrependableReversableIterable,
               (std::forward_list<int> list)) {
  auto mapped = map<int> (list, [] (int a) -> int { return 0; });
  auto res    = aggregate (mapped, [] (int a, int b) { return a + b; });


  RC_ASSERT (res == 0);
}

RC_GTEST_PROP (MapConcepts, mapPlainArray, (std::array<int, 256> list)) {
  int* arr = (int*) calloc (256, sizeof (int));

  map<int> (256, list.data ( ), arr, [] (int a) { return 0; });
  auto res = aggregate (arr, 256, [] (int a, int b) { return a + b; });

  free (arr);

  RC_ASSERT (res == 0);
}
