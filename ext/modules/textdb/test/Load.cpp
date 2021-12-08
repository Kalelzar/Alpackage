#include <Alpackage/Module/ModuleLoader.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <sstream>

TEST (TextDB, Load) {
  auto mod_ = Alpackage::Module::ModuleLoader::getModule ("AlpackageTextDB");
  ASSERT_TRUE (mod_.isDefined ( ));
  auto mod = mod_.get ( );
  ASSERT_STREQ (mod->name ( ), "TextDB");
  ASSERT_TRUE (mod->canList ( ));
  ASSERT_FALSE (mod->canFind ( ));
  ASSERT_FALSE (mod->canInstall ( ));
  ASSERT_FALSE (mod->canSearch ( ));
}
