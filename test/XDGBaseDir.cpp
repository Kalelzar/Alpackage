#include <Kal/XDGBaseDir.hpp>

#include <gtest/gtest.h>

#include <cstdlib>
#include <rapidcheck/Assertions.h>
#include <rapidcheck/gtest.h>
#include <stdexcept>

TEST (XDGBaseConfig, BestCase) {
  setenv ("XDG_CONFIG_HOME", "/home/gtest/.config", 69);
  auto config = TRY_ASSERT (XDG_CONFIG_HOME ( ));
  ASSERT_EQ (config, "/home/gtest/.config");
}

TEST (XDGBaseConfig, EmptyCase) {
  setenv ("XDG_CONFIG_HOME", "", 69);
  auto config = TRY_ASSERT (XDG_CONFIG_HOME ( ));
  ASSERT_EQ (config, XDG_CONFIG_HOME_DEFAULT);
}

TEST (XDGBaseConfig, UnsetCase) {
  unsetenv ("XDG_CONFIG_HOME");
  auto config = TRY_ASSERT (XDG_CONFIG_HOME ( ));
  ASSERT_EQ (config, XDG_CONFIG_HOME_DEFAULT);
}

TEST (XDGBaseConfig, SetToRelativePath) {
  setenv ("XDG_CONFIG_HOME", "foo/bar/.config", 69);
  ASSERT_TRUE (XDG_CONFIG_HOME ( ).isEmpty ( ));
}

TEST (XDGBaseData, BestCase) {
  setenv ("XDG_DATA_HOME", "/home/gtest/.data", 69);
  auto data = TRY_ASSERT (XDG_DATA_HOME ( ));
  ASSERT_EQ (data, "/home/gtest/.data");
}

TEST (XDGBaseData, EmptyCase) {
  setenv ("XDG_DATA_HOME", "", 69);
  auto data = TRY_ASSERT (XDG_DATA_HOME ( ));
  ASSERT_EQ (data, XDG_DATA_HOME_DEFAULT);
}

TEST (XDGBaseData, UnsetCase) {
  unsetenv ("XDG_DATA_HOME");
  auto data = TRY_ASSERT (XDG_DATA_HOME ( ));
  ASSERT_EQ (data, XDG_DATA_HOME_DEFAULT);
}

TEST (XDGBaseData, SetToRelativePath) {
  setenv ("XDG_DATA_HOME", "foo/bar/.data", 69);
  ASSERT_TRUE (XDG_DATA_HOME ( ).isEmpty ( ));
}

TEST (XDGBaseCache, BestCase) {
  setenv ("XDG_CACHE_HOME", "/home/gtest/.cache", 69);
  auto cache = TRY_ASSERT (XDG_CACHE_HOME ( ));
  ASSERT_EQ (cache, "/home/gtest/.cache");
}

TEST (XDGBaseCache, EmptyCase) {
  setenv ("XDG_CACHE_HOME", "", 69);
  auto cache = TRY_ASSERT (XDG_CACHE_HOME ( ));
  ASSERT_EQ (cache, XDG_CACHE_HOME_DEFAULT);
}

TEST (XDGBaseCache, UnsetCase) {
  unsetenv ("XDG_CACHE_HOME");
  auto cache = TRY_ASSERT (XDG_CACHE_HOME ( ));
  ASSERT_EQ (cache, XDG_CACHE_HOME_DEFAULT);
}

TEST (XDGBaseCache, SetToRelativePath) {
  setenv ("XDG_CACHE_HOME", "foo/bar/.cache", 69);
  ASSERT_TRUE (XDG_CACHE_HOME ( ).isEmpty ( ));
}

TEST (XDGBaseState, BestCase) {
  setenv ("XDG_STATE_HOME", "/home/gtest/.state", 69);
  auto state = TRY_ASSERT (XDG_STATE_HOME ( ));
  ASSERT_EQ (state, "/home/gtest/.state");
}

TEST (XDGBaseState, EmptyCase) {
  setenv ("XDG_STATE_HOME", "", 69);
  auto state = TRY_ASSERT (XDG_STATE_HOME ( ));
  ASSERT_EQ (state, XDG_STATE_HOME_DEFAULT);
}

TEST (XDGBaseState, UnsetCase) {
  unsetenv ("XDG_STATE_HOME");
  auto state = TRY_ASSERT (XDG_STATE_HOME ( ));
  ASSERT_EQ (state, XDG_STATE_HOME_DEFAULT);
}

TEST (XDGBaseState, SetToRelativePath) {
  setenv ("XDG_STATE_HOME", "foo/bar/.state", 69);
  ASSERT_TRUE (XDG_STATE_HOME ( ).isEmpty ( ));
}

TEST (XDGBaseExec, BestCase) {
  setenv ("XDG_EXEC_HOME", "/home/gtest/.exec", 69);
  auto exec = TRY_ASSERT (XDG_EXEC_HOME ( ));
  ASSERT_EQ (exec, "/home/gtest/.exec");
}

TEST (XDGBaseExec, EmptyCase) {
  setenv ("XDG_EXEC_HOME", "", 69);
  auto exec = TRY_ASSERT (XDG_EXEC_HOME ( ));
  ASSERT_EQ (exec, XDG_EXEC_HOME_DEFAULT);
}

TEST (XDGBaseExec, UnsetCase) {
  unsetenv ("XDG_EXEC_HOME");
  auto exec = TRY_ASSERT (XDG_EXEC_HOME ( ));
  ASSERT_EQ (exec, XDG_EXEC_HOME_DEFAULT);
}

TEST (XDGBaseExec, SetToRelativePath) {
  setenv ("XDG_EXEC_HOME", "foo/bar/.exec", 69);
  ASSERT_TRUE (XDG_EXEC_HOME ( ).isEmpty ( ));
}

TEST (XDGBaseRuntime, BestCase) {
  setenv ("XDG_RUNTIME_DIR", "/dir/gtest/.runtime", 69);
  auto runtime = TRY_ASSERT (XDG_RUNTIME_DIR ( ));
  ASSERT_EQ (runtime, "/dir/gtest/.runtime");
}

TEST (XDGBaseRuntime, EmptyCase) {
  setenv ("XDG_RUNTIME_DIR", "", 69);
  auto runtime = TRY_ASSERT (XDG_RUNTIME_DIR ( ));
  ASSERT_EQ (runtime, XDG_RUNTIME_DIR_DEFAULT);
}

TEST (XDGBaseRuntime, UnsetCase) {
  unsetenv ("XDG_RUNTIME_DIR");
  auto runtime = TRY_ASSERT (XDG_RUNTIME_DIR ( ));
  ASSERT_EQ (runtime, XDG_RUNTIME_DIR_DEFAULT);
}

TEST (XDGBaseRuntime, SetToRelativePath) {
  setenv ("XDG_RUNTIME_DIR", "foo/bar/.runtime", 69);
  ASSERT_TRUE (XDG_RUNTIME_DIR ( ).isEmpty ( ));
}
