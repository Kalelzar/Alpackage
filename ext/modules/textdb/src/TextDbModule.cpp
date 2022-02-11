#include <Alpackage/Module/Module.hpp>
#include <Alpackage/Package.hpp>
#include <Alpackage/Util/Logging.hpp>

#include "Kal/Action.hpp"
#include <Kal/XDGBaseDir.hpp>

#include <boost/config.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Config.hpp"

namespace Alpackage::Module {


class TextDbModule : public IAlpackageModule {

  private:
  std::set<Package>                pkgs;
  std::vector<Kal::Action::Action> actions;

  protected:
  [[nodiscard]] ErrorOr<std::vector<Kal::Action::Action>>
    generateProvideList ( ) override {
#define MAKE_ACTION(NAME, ...)                                                 \
  TRY (Kal::Action::ActionFactory::the ( ).make (                              \
    NAME,                                                                      \
    "textdb",                                                                  \
    std::vector<Kal::Action::ActionArgument> __VA_ARGS__))
    return std::vector<Kal::Action::Action>{MAKE_ACTION ("name", { }),
                                            MAKE_ACTION ("version", { }),
                                            MAKE_ACTION ("list", { })};
#undef MAKE_ACTION
  }

  [[nodiscard]] ErrorOr<void>
    dispatch (Kal::Action::Action const& action) override {
#define DISPATCH_ACTION(NAME, ...)                                             \
  if (action.id == Kal::Action::ActionFactory::the ( ).reserve (NAME)) {       \
    __VA_ARGS__                                                                \
  } else
    DISPATCH_ACTION ("version", Log::info ("Dispatch `version'");)
    DISPATCH_ACTION ("name", Log::info ("Dispatch `name'");)
    DISPATCH_ACTION ("list", Log::info ("Dispatch `list'");) {
      return format ("Cannot dispatch '{}'. Operation not supported", action);
    }
    return { };
#undef DISPATCH_ACTION
  }

  public:
  [[nodiscard]] ErrorOr<std::vector<Kal::Action::Action>>
    provides ( ) const override {
    return actions;
  };

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
      actions         = TRY (generateProvideList ( ));
      auto configdirs = TRY (XDG_CONFIG_DIRS ( ));
      auto confdir
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

  [[nodiscard]] ErrorOr<bool> update (std::string const& pkgName) override {
    return format ("Package '{}-{}' doesn't support installing packages.",
                   name ( ),
                   version ( ));
  }
};

extern "C" BOOST_SYMBOL_EXPORT TextDbModule module;
TextDbModule                                module;

}     // namespace Alpackage::Module
