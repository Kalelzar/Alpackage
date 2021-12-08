#include <Alpackage/Module/ModuleLoader.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <stdexcept>

TEST (ModuleLoader, LoadNonexistent) {
  ASSERT_TRUE (Alpackage::Module::ModuleLoader ::getModule ("NonexistentModule")
                 .isEmpty ( ));
}
