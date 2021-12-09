#pragma once

#include <Alpackage/Util/ConfLine.hpp>

#include <Kal/ErrorOr.hpp>

#include <string>

class Config {
  public:
  std::string name, pin, description, remote, dir, build, install, version;
  Config ( )      = default;
  Config& operator= (Config const&) = default;
  Config (Config const&)            = default;
  Config (Config&& c) noexcept
      : name (std::move (c.name))
      , pin (std::move (c.pin))
      , description (std::move (c.description))
      , remote (std::move (c.remote))
      , dir (std::move (c.dir))
      , build (std::move (c.build))
      , install (std::move (c.install))
      , version (std::move (c.version)) { }

  ~Config ( ) = default;
};

bool            validate (Config const&);
ErrorOr<Config> from (std::vector<ConfLine> const&);
