#include <Alpackage/Util/EntryReader.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <sstream>


RC_GTEST_PROP (EntryReader, TestReadInt, (int a, int b, int c)) {
  std::stringstream s;
  s << a << " " << b << " " << c;
  auto v = TRY_RC_ASSERT (EntryReader<int>::parse (&s));
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

  ~ThreeNumbers ( ) = default;
};

class ThreeNumbersRead {
  public:
  int a, b, c;
  constexpr ThreeNumbersRead ( ) : a (0), b (0), c (0) { }
  constexpr ThreeNumbersRead& operator= (ThreeNumbersRead const&) = default;

  static ErrorOr<ThreeNumbersRead> read (std::istream* in) {
    ThreeNumbersRead res;
    *in >> res.a;
    if (in->fail ( )) {
      return ErrorOr<ThreeNumbersRead> (
        std::move (format ("Couldn't read number 1")));
    }
    *in >> res.b;
    if (in->fail ( )) {
      return ErrorOr<ThreeNumbersRead> (
        std::move (format ("Couldn't read number 2")));
    }
    *in >> res.c;
    if (in->fail ( )) {
      return ErrorOr<ThreeNumbersRead> (
        std::move (format ("Couldn't read number 3")));
    }

    return res;
  }
};


RC_GTEST_PROP (EntryReader, TestReadStructWithOperator, (int a, int b, int c)) {
  std::stringstream s;
  s << a << " " << b << " " << c;
  auto v = TRY_RC_ASSERT (EntryReader<ThreeNumbers>::parse (&s));
  RC_ASSERT (v.size ( ) == 1);
  RC_ASSERT (v[0].a == a);
  RC_ASSERT (v[0].b == b);
  RC_ASSERT (v[0].c == c);
  RC_ASSERT (s.eof ( ));
}

RC_GTEST_PROP (EntryReader, TestReadStructWithRead, (int a, int b, int c)) {
  std::stringstream s;
  s << a << " " << b << " " << c;
  auto v = TRY_RC_ASSERT (EntryReader<ThreeNumbersRead>::parse (&s));
  RC_ASSERT (v.size ( ) == 1);
  RC_ASSERT (v[0].a == a);
  RC_ASSERT (v[0].b == b);
  RC_ASSERT (v[0].c == c);
  RC_ASSERT (s.eof ( ));
}

RC_GTEST_PROP (EntryReader, TestReadStructFail, (int a, int b)) {
  std::stringstream s;
  s << a << " " << b;
  RC_ASSERT (EntryReader<ThreeNumbers>::parse (&s).isEmpty ( ));
}

RC_GTEST_PROP (EntryReader, TestReadStructWithReadFail, (int a, int b)) {
  std::stringstream s;
  s << a << " " << b << " b";
  auto v = EntryReader<ThreeNumbersRead>::parse (&s);
  // RC_ASSERT (v.isEmpty ( ));
  // RC_ASSERT (v.getErrors ( ).front ( ) == "Couldn't read number 3");
}
