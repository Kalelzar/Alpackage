#include <Kal/Format.hpp>

#include "Config.hpp"

bool            validate (Config const&){ };
ErrorOr<Config> from (std::vector<ConfLine> const& lines) {
  Config def = { };
  for (auto const& line : lines) {
    if (line.name == "name") {
      def.name = line.value;
    } else if (line.name == "pin") {
      def.pin = line.value;
    } else if (line.name == "description") {
      def.description = line.value;
    } else if (line.name == "remote") {
      def.remote = line.value;
    } else if (line.name == "dir") {
      def.dir = line.value;
    } else if (line.name == "build") {
      def.build = line.value;
    } else if (line.name == "install") {
      def.install = line.value;
    } else if (line.name == "version") {
      def.version = line.value;
    } else {
      return format ("Invalid key-value pair: '{} = {}'",
                     line.name,
                     line.value);
    }
  }

  return def;
}
