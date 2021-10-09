#include <Alpackage/Util/EntryReader.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <sstream>


RC_GTEST_PROP (EntryReader, TestReadInt, (int a, int b, int c)) {
  std::stringstream s;
  s << a << " " << b << " " << c;
  auto v = EntryReader<int>::parse (&s);
  RC_ASSERT (v.size ( ) == 3);
  RC_ASSERT (v[0] == a);
  RC_ASSERT (v[1] == b);
  RC_ASSERT (v[2] == c);
  RC_ASSERT (s.eof ( ));
}

class ThreeNumbers {
  public:
  int a, b, c;
  ThreeNumbers ( ) : a (0), b (0), c (0) { }

  friend std::istream& operator>> (std::istream& in, ThreeNumbers& nums) {
    in >> nums.a >> nums.b >> nums.c;
    return in;
  }
};

RC_GTEST_PROP (EntryReader, TestReadStruct, (int a, int b, int c)) {
  std::stringstream s;
  s << a << " " << b << " " << c;
  auto v = EntryReader<ThreeNumbers>::parse (&s);
  RC_ASSERT (v.size ( ) == 1);
  RC_ASSERT (v[0].a == a);
  RC_ASSERT (v[0].b == b);
  RC_ASSERT (v[0].c == c);
  RC_ASSERT (s.eof ( ));
}

RC_GTEST_PROP (EntryReader, TestReadStructFail, (int a, int b)) {
  std::stringstream s;
  s << a << " " << b;
  RC_ASSERT_THROWS (EntryReader<ThreeNumbers>::parse (&s));
}
