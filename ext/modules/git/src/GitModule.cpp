#include <Alpackage/Module/Module.hpp>
#include <Alpackage/Package.hpp>
#include <Alpackage/Util/ConfLine.hpp>
#include <Alpackage/Util/EntryReader.hpp>
#include <Alpackage/Util/FileUtils.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <Kal/Concepts/Applicators.hpp>

#include <Kal/XDGBaseDir.hpp>

#include <boost/config.hpp>
#include <fstream>
#include <git2/global.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Config.hpp"
#include "GitRepo.hpp"

namespace Alpackage::Module {

class GitModule : public IAlpackageModule {
  std::vector<GitRepo> pkgs;

  public:
  [[nodiscard]] constexpr bool canSearch ( ) const override { return true; }
  [[nodiscard]] constexpr bool canFind ( ) const override { return true; }
  [[nodiscard]] constexpr bool canInstall ( ) const override { return true; }
  [[nodiscard]] constexpr bool canList ( ) const override { return true; }

  [[nodiscard]] constexpr const char* version ( ) const override {
    return "1.0.0";
  };
  [[nodiscard]] constexpr const char* name ( ) const override { return "Git"; };

  ErrorOr<ModuleError>                init ( ) override {
    Log::info ("Initializing module: %s", name ( ));
    git_libgit2_init ( );
    {
      reckless::scoped_indent indent;

      Log::info ("Loading config from $XDG_CONFIG_HOME/alpackage/git.conf");

      auto confhome = TRY (XDG_CONFIG_HOME ( ));

      auto confLocation
        = TRY (findFileAt (confhome + "/alpackage", "git.conf"));

      std::ifstream in (confLocation);
      auto          confLines = TRY (EntryReader<ConfLine>::parse (&in));
      in.close ( );
      auto conf = TRY (Config::from (confLines));


      Log::info ("Loading local gpkgs from $XDG_CONFIG_HOME/alpackage/gpkg/");

      auto p = TRY (mapDirectoryFiles<GitRepo> (
        confhome + "/alpackage/gpkg/",
        [&conf] (auto& t) -> ErrorOr<GitRepo> {
          std::ifstream in (t.path ( ));
          auto          res = TRY (EntryReader<ConfLine>::parse (&in));
          in.close ( );
          auto ret = TRY (from (res, conf));
          return ret;
        }));

      pkgs   = p;
    }

    return ModuleError::NONE;
  };

  [[nodiscard]] ModuleErrorOr<std::set<Package>> installed ( ) const override {
    return ModuleError::UNIMPLEMENTED;
  };

  [[nodiscard]] ErrorOr<std::set<std::string>> hasUpdates ( ) override {
    auto res = map<std::string, GitRepo> (pkgs, [] (GitRepo& c) {
      auto status = c.checkIfBehind ( );
      if (status.isDefined ( )) {
        auto aheadBehind = status.get ( );
        if (aheadBehind.ahead == 0 && aheadBehind.behind == 0)
          return format ("{}-{}: Is up-to-date.", c.name, c.version);
        else if (aheadBehind.ahead == 0) {
          return format ("{}-{}: Is {} commits behind origin.",
                         c.name,
                         c.version,
                         aheadBehind.behind);
        } else if (aheadBehind.behind == 0) {
          return format ("{}-{}: Is {} commits ahead of origin.",
                         c.name,
                         c.version,
                         aheadBehind.ahead);
        }

        return format ("{}-{}: Is {} commits ahead, {} behind origin.",
                       c.name,
                       c.version,
                       aheadBehind.ahead,
                       aheadBehind.behind);
      }
      return format (
        "Encountered error while checking '{}-{}': \n\tCaused by: {}",
        c.name,
        c.version,
        mkString (status.getErrors ( ), "\n\tCaused by: ", "", ""));
    });

    return std::set<std::string> (res.begin ( ), res.end ( ));
  };

  [[nodiscard]] ModuleErrorOr<std::set<Package>>
    search (std::string const& query) const override {
    return ModuleError::UNIMPLEMENTED;
  }
  [[nodiscard]] ModuleErrorOr<Package>
    find (std::string const& pkgName) const override {
    return ModuleError::UNIMPLEMENTED;
  }
  ModuleError install (std::string const& pkgName) override {
    return ModuleError::UNIMPLEMENTED;
  }


  ~GitModule ( ) override { git_libgit2_shutdown ( ); }
};

extern "C" BOOST_SYMBOL_EXPORT GitModule module;
GitModule                                module;

}     // namespace Alpackage::Module
