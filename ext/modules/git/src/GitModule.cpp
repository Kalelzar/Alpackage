#include <Alpackage/Module/Module.hpp>
#include <Alpackage/Package.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <Kal/XDGBaseDir.hpp>

#include <boost/config.hpp>
#include <git2/global.h>
#include <iostream>
#include <stdexcept>
#include <string>

namespace Alpackage::Module {

class GitModule : public IAlpackageModule {

  public:
  virtual constexpr bool canSearch ( ) const override { return true; }
  virtual constexpr bool canFind ( ) const override { return true; }
  virtual constexpr bool canInstall ( ) const override { return true; }
  virtual constexpr bool canList ( ) const override { return true; }     /// ???

  virtual constexpr const char* version ( ) const override { return "1.0.0"; };
  virtual constexpr const char* name ( ) const override { return "Git"; };

  virtual ModuleError           init ( ) override {
    Log::info ("Initializing module: %s", name ( ));
    git_libgit2_init ( );
    try {
      reckless::scoped_indent indent;
    } catch (char const* s) { throw std::runtime_error (s); }
    return ModuleError::NONE;
  };

  virtual ModuleErrorOr<std::set<Package>> installed ( ) const override {
    return ModuleError::UNIMPLEMENTED;
  };

  virtual ModuleErrorOr<std::set<Package>>
    search (std::string const& query) const override {
    return ModuleError::UNIMPLEMENTED;
  }
  virtual ModuleErrorOr<Package>
    find (std::string const& pkgName) const override {
    return ModuleError::UNIMPLEMENTED;
  }
  virtual ModuleError install (std::string const& pkgName) override {
    return ModuleError::UNIMPLEMENTED;
  }


  ~GitModule ( ) { git_libgit2_shutdown ( ); }
};

}     // namespace Alpackage::Module
