#pragma once

#include <Kal/default.hpp>

#include <forward_list>
#include <ostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace Alpackage {


class Package {
  public:
  std::string name;
  std::string pm;
  bool        installed;

  Package (std::string name, std::string pm, bool installed)
      : name (name)
      , pm (pm)
      , installed (installed) { }

  Package& operator= (Package const& other);

  Package ( )      = delete;

  bool                 operator== (Package const& other) const;

  bool                 operator< (Package const& other) const;

  friend std::ostream& operator<< (std::ostream& out, Package const& p) {
    out << "{ \"name\": \"" << p.name << "\", \"packageManager\": \"" << p.pm
        << "\" }";
    return out;
  }
};

}     // namespace Alpackage

template<>
inline const Alpackage::Package
  defaultValue<Alpackage::Package> = Alpackage::Package ("default",
                                                         "default",
                                                         false);
