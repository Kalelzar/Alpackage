#include <Kal/Concepts/detail/aggregate.hpp>
#include <Kal/Concepts/detail/unique.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>

#include "Unique.hpp"

RC_GTEST_PROP (UniqueConcepts, unique, (std::vector<int> list)) {
  auto          prepped = unique (list);
  std::set<int> t (list.begin ( ), list.end ( ));
  RC_ASSERT (t.size ( ) == prepped.size ( ));
  t.clear ( );
  for (auto& p : prepped) { t.insert (p); }
  RC_ASSERT (t.size ( ) == prepped.size ( ));
}

RC_GTEST_PROP (UniqueConcepts, uniqueBy, (std::vector<Unq> list)) {
  auto prepped1 = uniqueBy (list, [] (Unq const& t1, Unq const& t2) -> bool {
    return t1.a == t2.a;
  });

  RC_ASSERT (prepped1.size ( ) <= 2);
  if (prepped1.size ( ) == 2) { RC_ASSERT (prepped1[0].a != prepped1[1].a); }


  auto prepped2 = uniqueBy (list, [] (Unq const& t1, Unq const& t2) -> bool {
    return t1.b == t2.b;
  });


  std::set<int> a;
  for (auto& t : prepped2) { a.insert (t.b); }
  RC_ASSERT (a.size ( ) == prepped2.size ( ));
  a.clear ( );
  for (auto& t : list) { a.insert (t.b); }
  RC_ASSERT (a.size ( ) == prepped2.size ( ));
}

TEST (UniqueConcepts, uniqueBy2) {
  std::vector<Unq> list = {{true, 1}, {true, 2}, {false, 1}};


  auto prepped1 = uniqueBy (list, [] (Unq const& t1, Unq const& t2) -> bool {
    return t1.a == t2.a;
  });


  bool res1     = foldLeft (true, prepped1, [] (bool acc, Unq const& t) {
    return acc && t.a;
  });

  ASSERT_FALSE (res1);

  auto prepped2 = uniqueBy (list, [] (Unq const& t1, Unq const& t2) -> bool {
    return t1.b == t2.b;
  });


  bool res2     = foldLeft (true, prepped2, [] (bool acc, Unq const& t) {
    return acc && t.a;
  });


  ASSERT_TRUE (res2);
}
