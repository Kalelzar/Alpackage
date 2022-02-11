#pragma once

#include <Alpackage/Package.hpp>
#include <Alpackage/Util/Logging.hpp>

//#include <Kal/Concepts/TypeConversion.hpp>

#include <Kal/Action.hpp>
#include <Kal/ActionConsumer.hpp>
#include <Kal/ActionProvider.hpp>
#include <Kal/ErrorOr.hpp>
#include <Kal/Option.hpp>
#include <Kal/default.hpp>

#include <boost/config.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

namespace Alpackage::Module {

class BOOST_SYMBOL_VISIBLE IAlpackageModule
    : public Kal::Action::IActionProvider
    , public Kal::Action::IActionConsumer {
  protected:
  [[nodiscard]] ErrorOr<std::vector<Kal::Action::Action>>
    generateProvideList ( ) override = 0;
  [[nodiscard]] ErrorOr<void>
    dispatch (Kal::Action::Action const& action) override = 0;
  public:
  [[nodiscard]] ErrorOr<std::vector<Kal::Action::Action>>
                                              provides ( ) const override = 0;

  [[nodiscard]] virtual constexpr bool        canSearch ( ) const         = 0;
  [[nodiscard]] virtual constexpr bool        canFind ( ) const           = 0;
  [[nodiscard]] virtual constexpr bool        canInstall ( ) const        = 0;
  [[nodiscard]] virtual constexpr bool        canList ( ) const           = 0;

  [[nodiscard]] virtual constexpr const char* version ( ) const           = 0;
  [[nodiscard]] virtual constexpr const char* name ( ) const              = 0;

  virtual ErrorOr<bool>                       init ( )                    = 0;

  [[nodiscard]] virtual ErrorOr<std::set<Package>> installed ( ) const    = 0;
  [[nodiscard]] virtual ErrorOr<std::set<Package>>
    search (std::string const& query) const = 0;
  [[nodiscard]] virtual ErrorOr<Package>
    find (std::string const& pkgName) const                                = 0;


  [[nodiscard]] virtual ErrorOr<std::set<std::string>> hasUpdates ( )      = 0;

  [[nodiscard]] virtual ErrorOr<bool> install (std::string const& pkgName) = 0;

  [[nodiscard]] virtual ErrorOr<bool> update (std::string const& pkg)      = 0;

  [[nodiscard]] virtual ErrorOr<bool> install (Package const& pkg) {
    return install (pkg.name);
  }

  [[nodiscard]] virtual ErrorOr<bool> update (Package const& pkg) {
    return update (pkg.name);
  }


  virtual ~IAlpackageModule ( ) = default;
  ;
};
}     // namespace Alpackage::Module
