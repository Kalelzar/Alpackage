#pragma once

#include <Alpackage/Util/Logging.hpp>

#include <Kal/Concepts/detail/mkString.hpp>

#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>
#include <Kal/Option.hpp>

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * An implementation of the XDG Base Directory Specification
 * @see
 * https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
 */
constexpr const char* XDG_CONFIG_HOME_DEFAULT = "~/.config";
ErrorOr<std::string>  XDG_CONFIG_HOME ( ) {
  char* env = nullptr;
  if ((env = getenv ("XDG_CONFIG_HOME")) != nullptr && strlen (env) > 0) {
    std::filesystem::path path (env);
    if (path.is_absolute ( )) { return mkString (env); }
    return ErrorMsg{
      format ("The path in 'XDG_CONFIG_HOME' needs to be absolute. It "
               "currently holds the relative path '{}'",
              env)};
  }
  Log::warn ("'XDG_CONFIG_HOME' is not set. You should probably remedy that. "
              "Using '%s' as a default.",
             XDG_CONFIG_HOME_DEFAULT);
  return mkString (XDG_CONFIG_HOME_DEFAULT);
}

constexpr const char* XDG_DATA_HOME_DEFAULT = "~/.local/share";
ErrorOr<std::string>  XDG_DATA_HOME ( ) {
  char* env = nullptr;
  if ((env = getenv ("XDG_DATA_HOME")) != nullptr && strlen (env) > 0) {
    std::filesystem::path path (env);
    if (path.is_absolute ( )) { return mkString (env); }
    return ErrorMsg{
      format ("The path in 'XDG_DATA_HOME' needs to be absolute. It "
               "currently holds the relative path '{}'",
              env)};
  }
  Log::warn ("'XDG_DATA_HOME' is not set. You should probably remedy that. "
              "Using '%s' as a default.",
             XDG_DATA_HOME_DEFAULT);
  return mkString (XDG_DATA_HOME_DEFAULT);
}

constexpr const char* XDG_CACHE_HOME_DEFAULT = "~/.cache";
ErrorOr<std::string>  XDG_CACHE_HOME ( ) {
  char* env = nullptr;
  if ((env = getenv ("XDG_CACHE_HOME")) != nullptr && strlen (env) > 0) {
    std::filesystem::path path (env);
    if (path.is_absolute ( )) { return mkString (env); }
    return ErrorMsg{
      format ("The path in 'XDG_CACHE_HOME' needs to be absolute. It "
               "currently holds the relative path '{}'",
              env)};
  }
  Log::warn ("'XDG_CACHE_HOME' is not set. You should probably remedy that. "
              "Using '%s' as a default.",
             XDG_CACHE_HOME_DEFAULT);
  return mkString (XDG_CACHE_HOME_DEFAULT);
}

constexpr const char* XDG_STATE_HOME_DEFAULT = "~/.local/state";
ErrorOr<std::string>  XDG_STATE_HOME ( ) {
  char* env = nullptr;
  if ((env = getenv ("XDG_STATE_HOME")) != nullptr && strlen (env) > 0) {
    std::filesystem::path path (env);
    if (path.is_absolute ( )) { return mkString (env); }
    return ErrorMsg{
      format ("The path in 'XDG_STATE_HOME' needs to be absolute. It "
               "currently holds the relative path '{}'",
              env)};
  }
  Log::warn ("'XDG_STATE_HOME' is not set. You should probably remedy that. "
              "Using '%s' as a default.",
             XDG_STATE_HOME_DEFAULT);
  return mkString (XDG_STATE_HOME_DEFAULT);
}

constexpr const char* XDG_RUNTIME_DIR_DEFAULT = "~/.cache/run/";
ErrorOr<std::string>  XDG_RUNTIME_DIR ( ) {
  char* env = nullptr;
  if ((env = getenv ("XDG_RUNTIME_DIR")) != nullptr && strlen (env) > 0) {
    std::filesystem::path path (env);
    if (path.is_absolute ( )) { return mkString (env); }
    return ErrorMsg{
      format ("The path in 'XDG_RUNTIME_DIR' needs to be absolute. It "
               "currently holds the relative path '{}'",
              env)};
  }
  Log::warn ("'XDG_RUNTIME_DIR' is not set. You REALLY should remedy that. "
              "Using '%s' as a default.",
             XDG_RUNTIME_DIR_DEFAULT);
  return mkString (XDG_RUNTIME_DIR_DEFAULT);
}

/**
 * As per the spec: "There is a single base directory relative to which
 * user-specific executable files may be written."
 *
 * The spec never defines an environment variable for this specific base
 * directory, as such there is no official name for it.
 * XDG_EXEC_HOME was chosen for it in this implementation.
 *
 * The specification does recommend that "$HOME/.local/bin" is used for this
 * purpose.
 */
constexpr const char* XDG_EXEC_HOME_DEFAULT = "~/.local/bin";
ErrorOr<std::string>  XDG_EXEC_HOME ( ) {
  char* env = nullptr;
  if ((env = getenv ("XDG_EXEC_HOME")) != nullptr && strlen (env) > 0) {
    std::filesystem::path path (env);
    if (path.is_absolute ( )) { return mkString (env); }
    return ErrorMsg{
      format ("The path in 'XDG_EXEC_HOME' needs to be absolute. It "
               "currently holds the relative path '{}'",
              env),
    };
  }
  Log::info ("'XDG_EXEC_HOME' is not set. You don't really need to set it. "
              "Using '%s' as a default.",
             XDG_EXEC_HOME_DEFAULT);
  return mkString (XDG_EXEC_HOME_DEFAULT);
}


constexpr const char* XDG_DATA_DIRS_DEFAULT = "/usr/local/share:/usr/share";
ErrorOr<std::vector<std::string>> XDG_DATA_DIRS ( ) {
  char*       env   = nullptr;
  const char* paths = nullptr;
  if ((env = getenv ("XDG_DATA_DIRS")) != nullptr && strlen (env) > 0) {
    paths = env;
  } else {
    Log::info ("'XDG_DATA_DIRS' is not set. You should probably remedy that. "
               "Using '%s' as a default.",
               XDG_DATA_DIRS_DEFAULT);
    paths = XDG_DATA_DIRS_DEFAULT;
  }

  std::vector<std::string> result;
  std::stringstream        ss (paths);
  std::string              item;

  auto                     dataDir = TRY (XDG_DATA_HOME ( ));

  result.push_back (dataDir);

  while (getline (ss, item, ':')) {
    std::filesystem::path path (item);
    if (path.is_absolute ( )) {
      result.push_back (item);
    } else {
      Log::warn ("Path '%s' in XDG_DATA_DIRS is not absolute. Ignoring.", item);
    }
  }

  return result;
}
constexpr const char*             XDG_CONFIG_DIRS_DEFAULT = "/etc/xdg";
ErrorOr<std::vector<std::string>> XDG_CONFIG_DIRS ( ) {
  char*       env   = nullptr;
  const char* paths = nullptr;
  if ((env = getenv ("XDG_CONFIG_DIRS")) != nullptr && strlen (env) > 0) {
    paths = env;
  } else {
    Log::info ("'XDG_CONFIG_DIRS' is not set. You should probably remedy that. "
               "Using '%s' as a default.",
               XDG_CONFIG_DIRS_DEFAULT);
    paths = XDG_CONFIG_DIRS_DEFAULT;
  }

  std::vector<std::string> result;
  std::stringstream        ss (paths);
  std::string              item;

  auto                     configHome = TRY (XDG_CONFIG_HOME ( ));

  result.push_back (configHome);

  while (getline (ss, item, ':')) {
    std::filesystem::path path (item);
    if (path.is_absolute ( )) {
      result.push_back (item);
    } else {
      Log::warn ("Path '%s' in XDG_CONFIG_DIRS is not absolute. Ignoring.",
                 item);
    }
  }

  return result;
}


inline ErrorOr<std::string> findFileAt (std::string root,
                                        std::string relativeFile) {
  std::filesystem::path path (root);
  std::filesystem::path file = path / relativeFile;
  if (std::filesystem::exists (file)) { return file.string ( ); }
  return ErrorMsg{
    format ("Could not find the file '{}' in '{}'", relativeFile, root)};
}

inline ErrorOr<std::string>
  findFirstFile (std::vector<std::string> const& paths,
                 std::string                     relativeFile) {
  for (auto const& p : paths) {
    auto path = std::move (findFileAt (p, relativeFile));
    if (path) { return path; }
  }
  return ErrorMsg{format (
    "Could not find the file '{}' in any of the following directories: {}",
    relativeFile,
    mkString (paths, " : "))};
}
