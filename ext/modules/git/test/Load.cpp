#include <Alpackage/Module/ModuleLoader.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <sstream>

TEST (Git, Load) {
  auto mod = Alpackage::Module::ModuleLoader::getModule ("AlpackageGit",
                                                         "/lib/alpackage");
  ASSERT_STREQ (mod->name ( ), "Git");
  ASSERT_TRUE (mod->canList ( ));
  ASSERT_FALSE (mod->canFind ( ));
  ASSERT_FALSE (mod->canInstall ( ));
  ASSERT_FALSE (mod->canSearch ( ));
}
