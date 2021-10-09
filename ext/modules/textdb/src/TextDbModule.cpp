#include <Alpackage/Module/Module.hpp>
#include <Alpackage/Package.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <Kal/XDGBaseDir.hpp>

#include <boost/config.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Config.hpp"

namespace Alpackage::Module {


class TextDbModule : public IAlpackageModule {

  private:
  std::set<Package> pkgs;

  public:
  virtual constexpr bool        canSearch ( ) const override { return false; }
  virtual constexpr bool        canFind ( ) const override { return false; }
  virtual constexpr bool        canInstall ( ) const override { return false; }
  virtual constexpr bool        canList ( ) const override { return true; }

  virtual constexpr const char* version ( ) const override { return "1.0.1"; };
  virtual constexpr const char* name ( ) const override { return "TextDB"; };

  virtual ModuleError           init ( ) override {
    Log::info ("Initializing module: %s", name ( ));
    try {
      reckless::scoped_indent indent;
      Option<std::string>     confdir
        = findFirstFile (XDG_CONFIG_DIRS ( ), "alpackage/textdb.scsv");
      if (!confdir) confdir = "textdb.scsv";

      Log::info ("Loading config from '%s'", confdir.get ( ));
      pkgs = readConfig (confdir.get ( ));
    } catch (char const* s) { throw std::runtime_error (s); }
    return ModuleError::NONE;
  };

  virtual ModuleErrorOr<std::set<Package>> installed ( ) const override {
    return pkgs;
  };

  virtual ModuleErrorOr<std::set<Package>>
    search (std::string const& query) const override {
    return ModuleError::UNSUPPORTED;
  }
  virtual ModuleErrorOr<Package>
    find (std::string const& pkgName) const override {
    return ModuleError::UNSUPPORTED;
  }
  virtual ModuleError install (std::string const& pkgName) override {
    return ModuleError::UNSUPPORTED;
  }
};

extern "C" BOOST_SYMBOL_EXPORT TextDbModule module;
TextDbModule                                module;

}     // namespace Alpackage::Module
