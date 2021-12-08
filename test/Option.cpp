#include <Kal/Option.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>

#include "Option.hpp"

template<typename T>
rc::Gen<Option<T>> rc::Arbitrary<Option<T>>::arbitrary ( ) {
  return rc::gen::oneOf (
    rc::gen::construct<Option<T>> (rc::gen::arbitrary<T> ( )),
    rc::gen::just (Option<T>{ }));
}

RC_GTEST_PROP (Option, CanHoldAValue, (int a)) {
  Option<int> b (a);

  RC_ASSERT (b.isDefined ( ));
  RC_ASSERT_FALSE (b.isEmpty ( ));
  RC_ASSERT (b.get ( ) == a);
  RC_ASSERT (b.getOrElse (a - 1) == a);
  RC_ASSERT (b.getOrDefault ( ) == a);
}

RC_GTEST_PROP (Option, CanHoldNoValue, (int val)) {
  Option<int> b;

  RC_ASSERT_FALSE (b.isDefined ( ));
  RC_ASSERT (b.isEmpty ( ));
  RC_ASSERT (b.getOrElse (val) == val);
  RC_ASSERT (b.getOrDefault ( ) == defaultValue<int>);
}


RC_GTEST_PROP (Option, Copy, (Option<int> opt, int val)) {
  Option<int> opt2 = opt;

  RC_ASSERT (opt.isDefined ( ) == opt2.isDefined ( ));
  RC_ASSERT (opt.getOrElse (val) == opt2.getOrElse (val));
  RC_ASSERT (opt.getOrDefault ( ) == opt2.getOrDefault ( ));
}
