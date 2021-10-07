#include "Unique.hpp"

rc::Gen<Unq> rc::Arbitrary<Unq>::arbitrary ( ) {
  return rc::gen::build<Unq> (rc::gen::set (&Unq::a), rc::gen::set (&Unq::b));
}
