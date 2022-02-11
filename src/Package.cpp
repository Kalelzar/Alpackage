#include <Alpackage/Package.hpp>

namespace Alpackage {

Package& Package::operator= (Package const& other) {
  if (&other != this) {
    name      = other.name;
    pm        = other.pm;
    installed = other.installed;
  }
  return *this;
}

Package& Package::operator= (Package&& other) noexcept {
  if (&other != this) {
    name      = std::move (other.name);
    pm        = std::move (other.pm);
    installed = other.installed;
  }
  return *this;
}

bool Package::operator== (Package const& other) const {
  return pm == other.pm && name == other.name;
}

bool Package::operator< (Package const& other) const {
  return name < other.name;
}

ErrorOr<Package> Package::read (std::istream* in) {
  if (in->eof ( )) {
    return format ("Unexpected end of file while reading a Package.");
  }

  static const int nameSize    = 64;
  static const int pkgmngrSize = 16;
  static const int versionSize = 64;
  static const int descSize    = 256;

  char             name[nameSize];
  char             pkgmanager[pkgmngrSize];
  char             version[versionSize];
  char             description[descSize];

  static const int bufferSize = 512;

  char             buffer[bufferSize];
  in->getline (buffer, bufferSize);
  if (in->fail ( )) {
    return format ("({}:{}) Failed to get line from buffer: %s",
                   __FILE__,
                   __LINE__);
  }
  size_t read  = strlen (buffer);

  int    point = 0;
  int    mark  = 0;

  // FIXME: Check that output buffers are large enough to hold what is being
  // put in them
  //       Better yet dynamically scale their size to fit their data.

  while (point < read && buffer[point] != ';') { point++; }
  strncpy (name, buffer + mark, point - mark);
  name[point - mark] = '\0';
  point++;
  mark = point;

  while (point < read && buffer[point] != ';') { point++; }
  strncpy (pkgmanager, buffer + mark, point - mark);
  pkgmanager[point - mark] = '\0';
  point++;
  mark = point;

  while (point < read && buffer[point] != ';') { point++; }
  strncpy (version, buffer + mark, point - mark);
  version[point - mark] = '\0';
  point++;
  mark = point;
  if (mark <= read) {
    strncpy (description, buffer + mark, read - mark);
    description[read - mark] = '\0';
  }

  return Package (name, pkgmanager, true);
}

}     // namespace Alpackage
