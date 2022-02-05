#include "Kal/Concepts/detail/forall.hpp"
#include "Kal/Concepts/detail/map.hpp"
#include "Kal/Concepts/detail/mkString.hpp"
#include "Kal/Concepts/detail/zip.hpp"

#include <Kal/ErrorOr.hpp>
#include <Kal/Flag.hpp>
#include <Kal/Format.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <sstream>
#include <string>


RC_GTEST_PROP (Flag, IntFlagRead, (int val, int def)) {
  RC_PRE (def != val);

  Kal::Flag         flag ("Test");
  Kal::FlagRef<int> flagval
    = TRY_RC_ASSERT (flag.flag<int> ("test", def, "test"));

  auto res = TRY_RC_ASSERT (*flagval);

  RC_ASSERT (res == def);

  std::stringstream ss;
  ss << val;
  std::string s_value  = ss.str ( );
  char*       cs_value = const_cast<char*> (s_value.c_str ( ));
  char*       argv[]   = {"-test", cs_value};
  auto        read     = TRY_RC_ASSERT (flag.parse (2, argv));

  RC_ASSERT (read == 2);
  res = TRY_RC_ASSERT (*flagval);
  RC_ASSERT (res == val);
}

RC_GTEST_PROP (Flag, BoolFlagRead, (bool val, bool def)) {
  RC_PRE (def != val);

  Kal::Flag          flag ("Test");
  Kal::FlagRef<bool> flagval
    = TRY_RC_ASSERT (flag.flag<bool> ("test", def, "test"));
  auto res = TRY_RC_ASSERT (*flagval);
  RC_ASSERT (res == def);

  std::stringstream ss;
  ss << val;
  std::string s_value  = ss.str ( );
  char*       cs_value = const_cast<char*> (s_value.c_str ( ));
  char*       argv[]   = {"-test", cs_value};
  auto        read     = TRY_RC_ASSERT (flag.parse (2, argv));

  RC_ASSERT (read == 2);
  res = TRY_RC_ASSERT (*flagval);
  RC_ASSERT (res == val);
}

RC_GTEST_PROP (Flag, StringFlagRead, (std::string sval, std::string sdef)) {
  RC_PRE (sdef != sval);
  char*               val = const_cast<char*> (sval.c_str ( ));
  char*               def = const_cast<char*> (sdef.c_str ( ));
  Kal::Flag           flag ("Test");
  Kal::FlagRef<char*> flagval
    = TRY_RC_ASSERT (flag.flag<char*> ("test", def, "test"));


  auto* res = TRY_RC_ASSERT (*flagval);
  RC_ASSERT (strcmp (res, def) == 0);

  char* argv[] = {"-test", val};
  auto  read   = TRY_RC_ASSERT (flag.parse (2, argv));

  RC_ASSERT (read == 2);
  res = TRY_RC_ASSERT (*flagval);
  RC_ASSERT (strcmp (res, val) == 0);
}

TEST (Flag, EmptyFlagRead) {
  Kal::Flag          flag ("Test");
  Kal::FlagRef<bool> flagval = TRY_ASSERT (flag.flag ("test", "test"));

  auto               res     = TRY_ASSERT (*flagval);

  ASSERT_FALSE (res);

  char* argv[] = {"-test"};
  auto  read   = TRY_ASSERT (flag.parse (1, argv));

  ASSERT_EQ (read, 1);
  res = TRY_ASSERT (*flagval);
  ASSERT_TRUE (res);
}


template<typename T>
bool deepEq (std::vector<T> const& l, std::vector<T> const& r) {
  if (l.size ( ) != r.size ( )) return false;
  if (l.size ( ) == 0) return true;

  auto zipped   = zip<T, T, std::vector, std::vector, std::vector> (l, r);

  auto equality = forall (zipped, [] (std::pair<T, T> const& pair) {
    return pair.first == pair.second;
  });

  return equality;
}

bool deepStreq (std::vector<char*> const& l, std::vector<char*> const& r) {
  if (l.size ( ) != r.size ( )) return false;
  if (l.empty ( )) return true;

  auto zipped = zip<char*, char*, std::vector, std::vector, std::vector> (l, r);

  auto equality = forall (zipped, [] (std::pair<char*, char*> const& pair) {
    return strcmp (pair.first, pair.second) == 0;
  });

  return equality;
}

RC_GTEST_PROP (Flag, IntListFlagRead, (std::vector<int> val)) {
  RC_PRE (val.size ( ) != 0);

  Kal::Flag                      flag ("Test");
  Kal::FlagRef<std::vector<int>> flagval
    = TRY_RC_ASSERT (flag.flag<std::vector<int>> ("test", { }, "test"));


  std::string s_value  = mkString (val, " ", "", "");
  char*       cs_value = const_cast<char*> (s_value.c_str ( ));
  char*       argv[]   = {"-test", cs_value};
  auto        read     = TRY_RC_ASSERT (flag.parse (2, argv));

  RC_ASSERT (read == 2);
  auto res = TRY_RC_ASSERT (*flagval);
  RC_ASSERT (deepEq (val, res));
}

RC_GTEST_PROP (Flag, BoolListFlagRead, (std::vector<bool> val)) {
  RC_PRE (val.size ( ) != 0);

  Kal::Flag                       flag ("Test");
  Kal::FlagRef<std::vector<bool>> flagval
    = TRY_RC_ASSERT (flag.flag<std::vector<bool>> ("test", { }, "test"));


  std::string s_value  = mkString (val, " ", "", "");
  char*       cs_value = const_cast<char*> (s_value.c_str ( ));
  char*       argv[]   = {"-test", cs_value};
  auto        read     = TRY_RC_ASSERT (flag.parse (2, argv));

  RC_ASSERT (read == 2);
  auto res = TRY_RC_ASSERT (*flagval);
  RC_ASSERT (deepEq (val, res));
}

// RC_GTEST_PROP (Flag, StringListFlagRead, (std::vector<std::string> sval)) {
//   RC_PRE (sval.size ( ) != 0);

//   std::vector<char*> val
//     = map<char*> (sval, [] (std::string const& c) -> char* {
//         return const_cast<char*> (c.c_str ( ));
//       });


//   Kal::Flag                        flag ("Test");
//   Kal::FlagRef<std::vector<char*>> flagval
//     = TRY_RC_ASSERT (flag.flag<std::vector<char*>> ("test", { }, "test"));


//   std::string s_value  = mkString (val, " ", "", "");
//   char*       cs_value = const_cast<char*> (s_value.c_str ( ));
//   char*       argv[]   = {"-test", cs_value};
//   auto        read     = TRY_RC_ASSERT (flag.parse (2, argv));

//   RC_ASSERT (read == 2);
//   auto res = TRY_RC_ASSERT (*flagval);
//   RC_ASSERT (deepStreq (val, res));
// }
