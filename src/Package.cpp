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

bool Package::operator== (Package const& other) const {
  return pm == other.pm && name == other.name;
}

bool Package::operator< (Package const& other) const {
  return name < other.name;
}


std::ostream& Package::operator<< (std::ostream& out) const {
  out << "{ \"name\": \"" << name << "\", \"packageManager\": \"" << pm
      << "\" }";
  return out;
}


}     // namespace Alpackage
