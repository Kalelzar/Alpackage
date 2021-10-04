#pragma once

#include <Kal/default.hpp>
#include <forward_list>
#include <ostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace Alpackage {


struct Package {
  std::string name;
  std::string pm;
  bool        installed;

  Package (std::string name, std::string pm, bool installed)
      : name (name)
      , pm (pm)
      , installed (installed) { }

  Package& operator= (Package const& other) {
    if (&other != this) {
      name      = other.name;
      pm        = other.pm;
      installed = other.installed;
    }
    return *this;
  }

  Package ( ) = delete;

  bool operator== (Package const& other) const {
    return pm == other.pm && name == other.name;
  }

  bool operator< (Package const& other) const { return name < other.name; }

  friend std::ostream& operator<< (std::ostream& out, Package const& instance) {
    out << "{ \"name\": \"" << instance.name << "\", \"packageManager\": \""
        << instance.pm << "\" }";
    return out;
  }
};

std::unordered_map<std::string, std::forward_list<Package>> packages;

// FIXME: Using a map for this is expensive and inefficient
// due to the number of duplicated entries that can exist.
// What data structure can be used in order to model a many-to-many
// relationship?
std::unordered_map<std::string, std::forward_list<Package>> equivalences;

std::vector<Package> resolveEquivalent (Package);

std::vector<std::pair<Package, Package>>
  diff (std::vector<Package> const& left,
        std::vector<Package> const& right);     // {
//   // OPTIMIZE: Consider this:
//   // 1) Do we need to keep the original package lists? Maybe move all
//   elements
//   //    instead of copying everything
//   // 2) Would the benefits of storing pointers to avoid copies outweigh the
//   //    negative of worse cache locality. Is copying Package really that
//   //    expensive?
//   // 3) This algorithm is fairy slow. Can it be improved in some fashion?
//   std::vector<Package>       result;
//   std::set<Package, Package> lefts;
//   std::set<Package, Package> rights;
//   for (auto& package : left) {
//     auto equivs = resolveEquivalent (package);
//     lefts.insert (package);
//     for (auto& eqv : equivs) { lefts.insert (eqv); }
//   }
//   for (auto& package : right) {
//     auto equivs = resolveEquivalent (package);
//     rights.insert (package);
//     for (auto& eqv : equivs) { rights.insert (eqv); }
//   }

//   for (auto& candidate : lefts) {
//     if (rights.contains (candidate)) {
//       // TODO: Add to BOTH set.
//       // Remove from rights so we don't have to do this again.
//       rights.erase (candidate);
//     } else {
//       // TODO: Add to LEFT set.
//     }
//   }

//   // At this point all that remains in 'rights' are the elements not in left
//   // so we can just set the RIGHT set to 'rights'

//   // TODO: Set the RIGHT set

//   return { };
// }

}     // namespace Alpackage

template<>
inline const Alpackage::Package
  defaultValue<Alpackage::Package> = Alpackage::Package ("default",
                                                         "default",
                                                         false);
