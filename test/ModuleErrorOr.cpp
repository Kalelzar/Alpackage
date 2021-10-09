#include <Alpackage/Module/Module.hpp>

#include <gtest/gtest.h>

#include <cstdlib>
#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <stdexcept>

RC_GTEST_PROP (ModuleErrorOr, HasValue, (int a)) {
  Alpackage::Module::ModuleErrorOr<int> b (a);
  RC_ASSERT (!b.isError ( ));
  RC_ASSERT (b.getValue ( ) == a);
  RC_ASSERT (b.get ( ).get ( ) == a);
  RC_ASSERT (b.getError ( ) == Alpackage::Module::ModuleError::NONE);
}

TEST (ModuleErrorOr, HasError) {
  Alpackage::Module::ModuleErrorOr<int> b (
    Alpackage::Module::ModuleError::UNSUPPORTED);
  RC_ASSERT (b.isError ( ));
  RC_ASSERT_THROWS (b.getValue ( ));
  RC_ASSERT (b.get ( ).isEmpty ( ));
  RC_ASSERT (b.getError ( ) == Alpackage::Module::ModuleError::UNSUPPORTED);
}
