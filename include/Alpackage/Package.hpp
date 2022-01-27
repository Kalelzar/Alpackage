#pragma once

#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>
#include <Kal/default.hpp>

#include <cstring>
#include <forward_list>
#include <ostream>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


namespace Alpackage {


class Package {
  public:
  std::string name;
  std::string pm;
  bool        installed;

  Package (std::string name, std::string pm, bool installed)
      : name (std::move (name))
      , pm (std::move (pm))
      , installed (installed) { }

  Package& operator= (Package const& other);

  Package ( )      = delete;

  bool                 operator== (Package const& other) const;

  bool                 operator< (Package const& other) const;

  friend std::ostream& operator<< (std::ostream& out, Package const& p) {
    out << R"({ "name": ")" << p.name << R"(", "packageManager": ")" << p.pm
        << "\" }";
    return out;
  }

  static ErrorOr<Alpackage::Package> read (std::istream* in);
};

}     // namespace Alpackage

template<>
inline const Alpackage::Package
  defaultValue<Alpackage::Package> = Alpackage::Package ("default",
                                                         "default",
                                                         false);
