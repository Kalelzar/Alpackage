#include <Kal/Data/RadixTree.hpp>
#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <sstream>


RC_GTEST_PROP (RadixTree, Add, (std::string s, int data)) {
  RC_PRE (s.size ( ) > 1);
  Kal::Data::RadixTree<int> tree;
  TRY_RC_ASSERT (tree.add (s, data));
  auto res = tree.lookup (s);
  RC_ASSERT (res.isDefined ( ));
  RC_ASSERT (res == data);
}

RC_GTEST_PROP (RadixTree,
               AddOverwrites,
               (std::string s, int data1, int data2)) {
  RC_PRE (s.size ( ) > 1);
  Kal::Data::RadixTree<int> tree;
  RC_PRE (data1 != data2);

  TRY_RC_ASSERT (tree.add (s, data1));
  auto res = tree.lookup (s);
  RC_ASSERT (res.isDefined ( ));
  RC_ASSERT (res == data1);

  TRY_RC_ASSERT (tree.add (s, data2));

  auto res2 = tree.lookup (s);
  RC_ASSERT (res2.isDefined ( ));
  RC_ASSERT (res2 == data2);
}

RC_GTEST_PROP (RadixTree, Contains, (std::string _s, int data)) {
  RC_PRE (_s.size ( ) > 1);
  Kal::Data::RadixTree<int> tree;
  const char*               s = _s.c_str ( );
  TRY_RC_ASSERT (tree.add (s, data));

  auto res = tree.contains (s);
  RC_ASSERT (res);

  char s2[2048];
  strncpy (s2, s, strlen (s) - 1);
  s2[strlen (s) - 1] = '\0';

  size_t len         = strlen (s2);
  if (len > 0) {
    auto missing = !tree.contains (s2);
    RC_ASSERT (missing);
  }
}

RC_GTEST_PROP (RadixTree, Remove, (std::string s, int data)) {
  RC_PRE (s.size ( ) > 1);
  Kal::Data::RadixTree<int> tree;
  TRY_RC_ASSERT (tree.add (s, data));

  auto res = tree.contains (s);
  RC_ASSERT (res);

  auto rem = TRY_RC_ASSERT (tree.remove (s));

  RC_ASSERT (rem == data);
}

RC_GTEST_PROP (RadixTree, CopyConstructor, (std::string s, int data)) {
  RC_PRE (s.size ( ) > 1);
  Kal::Data::RadixTree<int> tree;
  TRY_RC_ASSERT (tree.add (s, data));
  auto res = tree.lookup (s);
  RC_ASSERT (res.isDefined ( ));
  RC_ASSERT (res == data);

  Kal::Data::RadixTree<int> tree2 (tree);
  auto                      res2 = tree2.lookup (s);
  RC_ASSERT (res2.isDefined ( ));
  RC_ASSERT (res2 == data);
}

RC_GTEST_PROP (RadixTree, MoveConstructor, (std::string s, int data)) {
  RC_PRE (s.size ( ) > 1);
  Kal::Data::RadixTree<int> tree;
  TRY_RC_ASSERT (tree.add (s, data));
  auto res = tree.lookup (s);
  RC_ASSERT (res.isDefined ( ));
  RC_ASSERT (res == data);

  Kal::Data::RadixTree<int> tree2 (std::move (tree));
  auto                      res2 = tree2.lookup (s);
  RC_ASSERT (res2.isDefined ( ));
  RC_ASSERT (res2 == data);
}

RC_GTEST_PROP (RadixTree, CopyOperator, (std::string s, int data)) {
  RC_PRE (s.size ( ) > 1);
  Kal::Data::RadixTree<int> tree1;
  Kal::Data::RadixTree<int> tree;
  TRY_RC_ASSERT (tree.add (s, data));
  auto res = tree.lookup (s);
  RC_ASSERT (res.isDefined ( ));
  RC_ASSERT (res == data);

  TRY_RC_ASSERT (tree1.add (s, -data));
  auto res1 = tree1.lookup (s);
  RC_ASSERT (res1.isDefined ( ));
  RC_ASSERT (res1 == -data);

  tree      = tree1;
  auto res2 = tree.lookup (s);
  RC_ASSERT (res2.isDefined ( ));
  RC_ASSERT (res2 == -data);
}

RC_GTEST_PROP (RadixTree, MoveOperator, (std::string s, int data)) {
  RC_PRE (s.size ( ) > 1);
  Kal::Data::RadixTree<int> tree1;
  Kal::Data::RadixTree<int> tree;
  TRY_RC_ASSERT (tree.add (s, data));
  auto res = tree.lookup (s);
  RC_ASSERT (res.isDefined ( ));
  RC_ASSERT (res == data);

  TRY_RC_ASSERT (tree1.add (s, -data));
  auto res1 = tree1.lookup (s);
  RC_ASSERT (res1.isDefined ( ));
  RC_ASSERT (res1 == -data);

  tree      = std::move (tree1);
  auto res2 = tree.lookup (s);
  RC_ASSERT (res2.isDefined ( ));
  RC_ASSERT (res2 == -data);
}
