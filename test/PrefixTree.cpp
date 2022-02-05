#include <Kal/Data/PrefixTree.hpp>
#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <sstream>


RC_GTEST_PROP (PrefixTree, add, (std::string _s, int data)) {
  RC_PRE (_s.size ( ) > 1);
  Kal::Data::PrefixTree<int> tree;
  const char*                s   = _s.c_str ( );
  auto                       res = TRY_RC_ASSERT (tree.add (data, s));
  RC_ASSERT (res == data);
}

RC_GTEST_PROP (PrefixTree,
               addOverwrites,
               (std::string _s, int data1, int data2)) {
  RC_PRE (_s.size ( ) > 1);
  Kal::Data::PrefixTree<int> tree;
  RC_PRE (data1 != data2);
  const char* s   = _s.c_str ( );
  auto        res = TRY_RC_ASSERT (tree.add (data1, s));
  RC_ASSERT (res == data1);
  res = TRY_RC_ASSERT (tree.add (data2, s));
  RC_ASSERT (res == data2);
}

RC_GTEST_PROP (PrefixTree, has, (std::string _s, int data)) {
  RC_PRE (_s.size ( ) > 1);
  Kal::Data::PrefixTree<int> tree;
  const char*                s = _s.c_str ( );
  TRY_RC_ASSERT (tree.add (data, s));

  auto res = tree.has (s);
  RC_ASSERT (res.isDefined ( ));
  RC_ASSERT (res.get ( ) == data);

  char s2[2048];
  strncpy (s2, s, strlen (s) - 1);
  s2[strlen (s) - 1] = '\0';

  size_t len         = strlen (s2);
  if (len > 0) {
    auto missing = tree.has (s2);
    RC_ASSERT (missing.isEmpty ( ));
  }
}
