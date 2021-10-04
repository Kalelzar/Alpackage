#pragma once

#include <Alpackage/Util/Logging.hpp>
#include <Kal/Concepts/Applicators.hpp>
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
std::string           XDG_CONFIG_HOME ( ) {
  char* env;
  if ((env = getenv ("XDG_CONFIG_HOME")) != nullptr && strlen (env) > 0) {
    std::filesystem::path path (env);
    if (path.is_absolute ( ))
      return env;
    else {
      Log::error ("The path in 'XDG_CONFIG_HOME' needs to be absolute. It "
                  "currently holds the relative path '%s'",
                  env);
      throw std::runtime_error ("XDG Basedir violation.");
    }
  } else {
    Log::warn ("'XDG_CONFIG_HOME' is not set. You should probably remedy that. "
               "Using '%s' as a default.",
               XDG_CONFIG_HOME_DEFAULT);
    return XDG_CONFIG_HOME_DEFAULT;
  }
}

constexpr const char* XDG_DATA_HOME_DEFAULT = "~/.local/share";
std::string           XDG_DATA_HOME ( ) {
  char* env;
  if ((env = getenv ("XDG_DATA_HOME")) != nullptr && strlen (env) > 0) {
    std::filesystem::path path (env);
    if (path.is_absolute ( ))
      return env;
    else {
      Log::error ("The path in 'XDG_DATA_HOME' needs to be absolute. It "
                  "currently holds the relative path '%s'",
                  env);
      throw std::runtime_error ("XDG Basedir violation.");
    }
  } else {
    Log::warn ("'XDG_DATA_HOME' is not set. You should probably remedy that. "
               "Using '%s' as a default.",
               XDG_DATA_HOME_DEFAULT);
    return XDG_DATA_HOME_DEFAULT;
  }
}

constexpr const char* XDG_CACHE_HOME_DEFAULT = "~/.cache";
std::string           XDG_CACHE_HOME ( ) {
  char* env;
  if ((env = getenv ("XDG_CACHE_HOME")) != nullptr && strlen (env) > 0) {
    std::filesystem::path path (env);
    if (path.is_absolute ( ))
      return env;
    else {
      Log::error ("The path in 'XDG_CACHE_HOME' needs to be absolute. It "
                  "currently holds the relative path '%s'",
                  env);
      throw std::runtime_error ("XDG Basedir violation.");
    }
  } else {
    Log::warn ("'XDG_CACHE_HOME' is not set. You should probably remedy that. "
               "Using '%s' as a default.",
               XDG_CACHE_HOME_DEFAULT);
    return XDG_CACHE_HOME_DEFAULT;
  }
}

constexpr const char* XDG_STATE_HOME_DEFAULT = "~/.local/state";
std::string           XDG_STATE_HOME ( ) {
  char* env;
  if ((env = getenv ("XDG_STATE_HOME")) != nullptr && strlen (env) > 0) {
    std::filesystem::path path (env);
    if (path.is_absolute ( ))
      return env;
    else {
      Log::error ("The path in 'XDG_STATE_HOME' needs to be absolute. It "
                  "currently holds the relative path '%s'",
                  env);
      throw std::runtime_error ("XDG Basedir violation.");
    }
  } else {
    Log::warn ("'XDG_STATE_HOME' is not set. You should probably remedy that. "
               "Using '%s' as a default.",
               XDG_STATE_HOME_DEFAULT);
    return XDG_STATE_HOME_DEFAULT;
  }
}

constexpr const char* XDG_RUNTIME_DIR_DEFAULT = "~/.cache/run/";
std::string           XDG_RUNTIME_DIR ( ) {
  char* env;
  if ((env = getenv ("XDG_RUNTIME_DIR")) != nullptr && strlen (env) > 0) {
    std::filesystem::path path (env);
    if (path.is_absolute ( ))
      return env;
    else {
      Log::error ("The path in 'XDG_RUNTIME_DIR' needs to be absolute. It "
                  "currently holds the relative path '%s'",
                  env);
      throw std::runtime_error ("XDG Basedir violation.");
    }
  } else {
    Log::warn ("'XDG_RUNTIME_DIR' is not set. You REALLY should remedy that. "
               "Using '%s' as a default.",
               XDG_RUNTIME_DIR_DEFAULT);
    return XDG_RUNTIME_DIR_DEFAULT;
  }
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
std::string           XDG_EXEC_HOME ( ) {
  char* env;
  if ((env = getenv ("XDG_EXEC_HOME")) != nullptr && strlen (env) > 0) {
    std::filesystem::path path (env);
    if (path.is_absolute ( ))
      return env;
    else {
      Log::error ("The path in 'XDG_EXEC_HOME' needs to be absolute. It "
                  "currently holds the relative path '%s'",
                  env);
      throw std::runtime_error ("XDG Basedir violation.");
    }
  } else {
    Log::info ("'XDG_EXEC_HOME' is not set. You don't really need to set it. "
               "Using '%s' as a default.",
               XDG_EXEC_HOME_DEFAULT);
    return XDG_EXEC_HOME_DEFAULT;
  }
}


constexpr const char*    XDG_DATA_DIRS_DEFAULT = "/usr/local/share:/usr/share";
std::vector<std::string> XDG_DATA_DIRS ( ) {
  char*       env;
  const char* paths;
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

  result.push_back (XDG_DATA_HOME ( ));

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
constexpr const char*    XDG_CONFIG_DIRS_DEFAULT = "/etc/xdg";
std::vector<std::string> XDG_CONFIG_DIRS ( ) {
  char*       env;
  const char* paths;
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

  result.push_back (XDG_CONFIG_HOME ( ));

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

// TODO: A good target for an optional return type.
std::string findFirstFile (std::vector<std::string> paths,
                           std::string              relativeFile) {
  for (auto p : paths) {
    std::filesystem::path path (p);
    std::filesystem::path file = path / relativeFile;
    if (std::filesystem::exists (file)) { return file.string ( ); }
  }
  Log::error (
    "Could not find the file '%s' in any of the following directories: %s",
    relativeFile,
    mkString (paths, " : "));
  throw std::runtime_error ("Could not find the required file.");
}

std::string findFileAt (std::string root, std::string relativeFile) {
  std::filesystem::path path (root);
  std::filesystem::path file = path / relativeFile;
  if (std::filesystem::exists (file)) { return file.string ( ); }
  Log::error ("Could not find the file '%s' in '%s'", relativeFile, root);
  throw std::runtime_error ("Could not find the required file.");
}
