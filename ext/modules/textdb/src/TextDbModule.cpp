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
  [[nodiscard]] constexpr bool canSearch ( ) const override { return false; }
  [[nodiscard]] constexpr bool canFind ( ) const override { return false; }
  [[nodiscard]] constexpr bool canInstall ( ) const override { return false; }
  [[nodiscard]] constexpr bool canList ( ) const override { return true; }

  [[nodiscard]] constexpr const char* version ( ) const override {
    return "1.0.1";
  };
  [[nodiscard]] constexpr const char* name ( ) const override {
    return "TextDB";
  };

  [[nodiscard]] ErrorOr<ModuleError> init ( ) override {
    Log::info ("Initializing module: %s", name ( ));
    {
      reckless::scoped_indent indent;
      auto                    configdirs = TRY (XDG_CONFIG_DIRS ( ));
      auto                    confdir
        = TRY_OR_ELSE (findFirstFile (configdirs, "alpackage/textdb.scsv"),
                       {"textdb.scsv"});

      Log::info ("Loading config from '%s'", confdir);
      pkgs = TRY (readConfig (confdir));
    }
    return ModuleError::NONE;
  };

  [[nodiscard]] ModuleErrorOr<std::set<Package>> installed ( ) const override {
    return pkgs;
  };

  [[nodiscard]] ErrorOr<std::set<std::string>> hasUpdates ( ) override {
    return format ("Unsupported operation.");
  };

  [[nodiscard]] ModuleErrorOr<std::set<Package>>
    search (std::string const& query) const override {
    return ModuleError::UNSUPPORTED;
  }
  [[nodiscard]] ModuleErrorOr<Package>
    find (std::string const& pkgName) const override {
    return ModuleError::UNSUPPORTED;
  }
  [[nodiscard]] ModuleError install (std::string const& pkgName) override {
    return ModuleError::UNSUPPORTED;
  }
};

extern "C" BOOST_SYMBOL_EXPORT TextDbModule module;
TextDbModule                                module;

}     // namespace Alpackage::Module
