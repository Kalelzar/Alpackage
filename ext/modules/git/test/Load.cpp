#include <Alpackage/Module/ModuleLoader.hpp>

#include <gtest/gtest.h>

#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <sstream>

TEST (Git, Load) {
  auto mod
    = TRY_ASSERT (Alpackage::Module::ModuleLoader::getModule ("AlpackageGit"));
  ASSERT_STREQ (mod->name ( ), "Git");
  ASSERT_TRUE (mod->canList ( ));
  ASSERT_TRUE (mod->canFind ( ));
  ASSERT_TRUE (mod->canInstall ( ));
  ASSERT_TRUE (mod->canSearch ( ));
}
