#pragma once

#include <Alpackage/Util/ConfLine.hpp>

#include <string>

struct Config {
  std::string name, pin, description, remote, dir, build, install, version;
  Config (Config&& c)
      : name (std::move (c.name))
      , pin (std::move (c.pin))
      , description (std::move (c.description))
      , remote (std::move (c.remote))
      , dir (std::move (c.dir))
      , build (std::move (c.build))
      , install (std::move (c.install))
      , version (std::move (c.version)) { }
};


bool   validate (Config const&);
Config from (std::vector<ConfLine> const&);
