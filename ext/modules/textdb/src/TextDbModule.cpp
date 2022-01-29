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

  [[nodiscard]] ErrorOr<bool> init ( ) override {
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
    return true;
  };

  [[nodiscard]] ErrorOr<std::set<Package>> installed ( ) const override {
    return pkgs;
  };

  [[nodiscard]] ErrorOr<std::set<std::string>> hasUpdates ( ) override {
    return format ("Package '{}-{}' doesn't support checking for updates.",
                   name ( ),
                   version ( ));
  };

  [[nodiscard]] ErrorOr<std::set<Package>>
    search (std::string const& query) const override {
    return format ("Package '{}-{}' doesn't support searching packages.",
                   name ( ),
                   version ( ));
  }
  [[nodiscard]] ErrorOr<Package>
    find (std::string const& pkgName) const override {
    return format ("Package '{}-{}' doesn't support finding packages.",
                   name ( ),
                   version ( ));
  }
  [[nodiscard]] ErrorOr<bool> install (std::string const& pkgName) override {
    return format ("Package '{}-{}' doesn't support installing packages.",
                   name ( ),
                   version ( ));
  }
  }
};

extern "C" BOOST_SYMBOL_EXPORT TextDbModule module;
TextDbModule                                module;

}     // namespace Alpackage::Module
