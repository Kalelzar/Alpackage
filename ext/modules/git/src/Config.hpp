#pragma once

#include <Alpackage/Util/ConfLine.hpp>

#include <Kal/ErrorOr.hpp>

#include <string>
#include <vector>

struct Config {
  std::string            sshKey{ };
  std::string            defaultDir{ };
  static ErrorOr<Config> from (std::vector<ConfLine> const&);
};
