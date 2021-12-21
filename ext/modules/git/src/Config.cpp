
#include <Kal/Format.hpp>

#include "Config.hpp"

ErrorOr<Config> Config::from (std::vector<ConfLine> const& lines) {
  Config def = { };
  for (auto const& line : lines) {
    if (line.name == "default") {
      def.defaultDir = line.value;
    } else if (line.name == "sshKey") {
      def.sshKey = line.value;
    } else {
      return format ("Invalid key-value pair: '{} = {}'",
                     line.name,
                     line.value);
    }
  }

  return def;
}
